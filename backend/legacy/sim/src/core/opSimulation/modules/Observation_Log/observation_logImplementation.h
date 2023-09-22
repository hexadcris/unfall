/********************************************************************************
 * Copyright (c) 2017-2022 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/*!
* \file  ObservationLogImplementation.h
* \brief Adds the RunStatistic information to the simulation output.
* \details  Writes the RunStatistic information into the simulation output.
*           Also manages the stop reasons of the simulation.
*/
//-----------------------------------------------------------------------------

#pragma once

#include <set>
#include <string>
#include <tuple>
#include <regex>

#include <QFile>
#include <QTextStream>

#include "common/runtimeInformation.h"
#include "include/observationInterface.h"

#include "observationCyclics.h"
#include "observationFileHandler.h"
#include "runStatistic.h"

namespace core {
class EventNetworkInterface;
}

/*!
 * \brief Reads values of type double from databuffer for each agent
 *
 * DataStore is queried for key. Results are stored in the result map for each agent id where the key matches.
 *
 * \throws std::bad_variant_access if a key of other type than double is queried.
 *
 * \param[in]    key          Key to read at
 * \param[in]    dataBuffer   DataBuffer to read from
 * \param[out]   result       Storage for result
 */
extern void GetAgentValues(const std::string& key, const DataBufferReadInterface* dataBuffer, std::map<std::string, double>& result);

//-----------------------------------------------------------------------------
/** \brief This class adds the RunStatistic information to the simulation output.
*   \details This class inherits the ObservationLogGeneric which creates the basic simulation output
*            and adds the RunStatistic information to the output.
*            This class also manages the stop reasons of the simulation.
*
*   \ingroup ObservationLog
*/
//-----------------------------------------------------------------------------
class ObservationLogImplementation : ObservationInterface
{
public:
    const std::string COMPONENTNAME = "ObservationLog";

    ObservationLogImplementation(core::EventNetworkInterface* eventNetwork,
                                   StochasticsInterface* stochastics,
                                   WorldInterface* world,
                                   const ParameterInterface* parameters,
                                   const CallbackInterface* callbacks,
                                   DataBufferReadInterface *dataBuffer);
    ObservationLogImplementation(const ObservationLogImplementation&) = delete;
    ObservationLogImplementation(ObservationLogImplementation&&) = delete;
    ObservationLogImplementation& operator=(const ObservationLogImplementation&) = delete;
    ObservationLogImplementation& operator=(ObservationLogImplementation&&) = delete;
    virtual ~ObservationLogImplementation() override = default;

    virtual void OpSimulationPreHook() override;
    virtual void OpSimulationPreRunHook() override;
    virtual void OpSimulationUpdateHook(int time, RunResultInterface& runResult) override;
    virtual void OpSimulationPostRunHook(const RunResultInterface& runResult) override;
    virtual void OpSimulationManagerPreHook() override {}
    virtual void OpSimulationManagerPostHook(const std::string&) override {}
    virtual void OpSimulationPostHook() override;

    virtual const std::string OpSimulationResultFile() override
    {
        return "";
    }

private:
    const openpass::common::RuntimeInformation& runtimeInformation;
    core::EventNetworkInterface* eventNetwork;
    DataBufferReadInterface* dataBuffer;
    ObservationFileHandler fileHandler;
    ObservationCyclics cyclics;
    Events events;
    RunStatistic runStatistic = RunStatistic(-1);
    std::vector<std::string> selectedColumns;
    std::vector<std::pair<std::string,std::string>> selectedRegexColumns;
};


