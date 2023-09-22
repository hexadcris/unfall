/********************************************************************************
 * Copyright (c) 2020 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  observation_LogImplementation */
//-----------------------------------------------------------------------------

#include <unordered_map>
#include <variant>
#include <vector>

#include "common/commonTools.h"
#include "common/openPassUtils.h"

#include "include/dataBufferInterface.h"
#include "include/stochasticsInterface.h"
#include "include/parameterInterface.h"
#include "include/worldInterface.h"

#include "observation_logImplementation.h"

#include "observationCyclics.h"
#include "runStatisticCalculation.h"
#include "runStatistic.h"

ObservationLogImplementation::ObservationLogImplementation(core::EventNetworkInterface* eventNetwork,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        const ParameterInterface* parameters,
        const CallbackInterface* callbacks,
        DataBufferReadInterface *dataBuffer) :
    ObservationInterface(stochastics,
                         world,
                         parameters,
                         callbacks,
                         dataBuffer),
    runtimeInformation(parameters->GetRuntimeInformation()),
    eventNetwork(eventNetwork),
    dataBuffer(dataBuffer),
    fileHandler{*dataBuffer}
{
}

void ObservationLogImplementation::OpSimulationPreHook()
{
    std::string filename{"simulationOutput.xml"};

    try
    {
        filename = GetParameters()->GetParametersString().at("OutputFilename");
    }
    catch (const std::out_of_range&)
    {
        LOG(CbkLogLevel::Warning, "Using default output filename: " + filename);
    }

    try
    {
        fileHandler.SetCsvOutput(GetParameters()->GetParametersBool().at("LoggingCyclicsToCsv"));
    }
    catch (const std::out_of_range&)
    {
        std::string msg = "Mandatory config parameter 'LoggingCyclicsToCsv' is missing";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    std::unordered_map<std::string, std::vector<std::string>> loggingGroups;
    for (const auto& [key, value]: GetParameters()->GetParametersStringVector())
    {
        if (key.substr(0, 13) == "LoggingGroup_")
        {
            const auto groupName = key.substr(13);
            loggingGroups[groupName] = value;
        }
    }
    try
    {
        for (const auto& loggingGroup : GetParameters()->GetParametersStringVector().at("LoggingGroups"))
        {
            try
            {
                const auto& groupColumns = loggingGroups.at(loggingGroup);
                for (const std::string& column : groupColumns)
                {
                    auto pos = column.find('*');
                    if (pos == std::string::npos)
                    {
                        selectedColumns.push_back(column);
                    }
                    else
                    {
                        selectedRegexColumns.push_back({column.substr(0,pos),column.substr(pos+1)});
                    }
                }
            }
            catch (const std::out_of_range&)
            {
                std::string msg = "Undefined LoggingGroup '" + loggingGroup + "'";
                LOG(CbkLogLevel::Error, msg);
                throw std::runtime_error(msg);
            }
        }
    }
    catch (const std::out_of_range&)
    {
        LOG(CbkLogLevel::Error, "No LoggingGroups configured");
    }

    fileHandler.SetOutputLocation(runtimeInformation.directories.output, filename);
    fileHandler.SetSceneryFile(std::get<std::string>(dataBuffer->GetStatic("SceneryFile").at(0)));
    fileHandler.WriteStartOfFile(runtimeInformation.versions.framework.str());
}

void ObservationLogImplementation::OpSimulationPreRunHook()
{
    runStatistic = RunStatistic(GetStochastics()->GetRandomSeed());
    cyclics.Clear();
    events.clear();
}

void GetAgentValues(const std::string& key, const DataBufferReadInterface* dataBuffer, std::map<std::string, double>& result)
{
    const auto agentIds = dataBuffer->GetKeys("Statics/Agents");

    for (const auto& agentId : agentIds)
    {
        const auto tdtResult = dataBuffer->GetCyclic(std::stoi(agentId), key);
        if(tdtResult->size() == 0)
        {
            continue;
        }
        const auto& cyclicRow = tdtResult->begin()->get();
        result[agentId] = std::get<double>(cyclicRow.value);
    }
}

void ObservationLogImplementation::OpSimulationUpdateHook(int time, [[maybe_unused]] RunResultInterface& runResult)
{
    const auto dsCyclics = dataBuffer->GetCyclic(std::nullopt, "*");

    for (const CyclicRow& dsCyclic : *dsCyclics)
    {
        std::visit(openpass::utils::FlatParameter::to_string([this, &dsCyclic, &time](const std::string& valueStr)
        {
            if (std::any_of(selectedColumns.cbegin(), selectedColumns.cend(),
                            [&dsCyclic](const std::string& column)
                            {
                                return dsCyclic.key == column;
                            }) ||
                std::any_of(selectedRegexColumns.cbegin(), selectedRegexColumns.cend(),
                            [&dsCyclic](const auto& column)
                            {
                                return dsCyclic.key.find(column.first) == 0 &&
                                       dsCyclic.key.find(column.second, dsCyclic.key.size() - column.second.size()) == dsCyclic.key.size() - column.second.size();
                            }))
            {
                cyclics.Insert(time, (dsCyclic.entityId < 10 ? "0" : "") + std::to_string(dsCyclic.entityId) + ":" + dsCyclic.key, valueStr);
            }
        }), dsCyclic.value);
    }

    GetAgentValues("TotalDistanceTraveled", dataBuffer, runStatistic.distanceTraveled);

    const auto acyclics = dataBuffer->GetAcyclic(std::nullopt, "*");

    for (const AcyclicRow& event : *acyclics)
    {
        events.push_back({time, event});
    }

    runStatistic.AddStopReason(time,RunStatistic::StopReason::DueToTimeOut);
}

void ObservationLogImplementation::OpSimulationPostRunHook(const RunResultInterface& runResult)
{
    RunStatisticCalculation::DetermineEgoCollision(runStatistic, runResult, GetWorld());
    runStatistic.VisibilityDistance = GetWorld()->GetVisibilityDistance();

    fileHandler.WriteRun(runResult, runStatistic, cyclics, events);
}

void ObservationLogImplementation::OpSimulationPostHook()
{
    fileHandler.WriteEndOfFile();
}
