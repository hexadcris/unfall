/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "scenario.h"
#include <algorithm>
#include <qglobal.h>

namespace Configuration {

const std::string& Scenario::GetVehicleCatalogPath() const
{
    return vehicleCatalogPath;
}

void Scenario::SetVehicleCatalogPath(const std::string& catalogPath)
{
    this->vehicleCatalogPath = catalogPath;
}

const std::string& Scenario::GetPedestrianCatalogPath() const
{
    return pedestrianCatalogPath;
}

void Scenario::SetPedestrianCatalogPath(const std::string& catalogPath)
{
    this->pedestrianCatalogPath = catalogPath;
}

const std::string& Scenario::GetTrajectoryCatalogPath() const
{
    return trajectoryCatalogPath;
}

void Scenario::SetTrajectoryCatalogPath(const std::string& catalogPath)
{
    trajectoryCatalogPath = catalogPath;
}

const std::string& Scenario::GetSceneryPath() const
{
    return sceneryPath;
}

void Scenario::SetSceneryPath(const std::string& sceneryPath)
{
    this->sceneryPath = sceneryPath;
}

const SceneryDynamicsInterface &Scenario::GetSceneryDynamics() const
{
    return sceneryDynamics;
}

void Scenario::AddTrafficSignalController(const openScenario::TrafficSignalController &controller)
{
    sceneryDynamics.AddTrafficSignalController(controller);
}

void Scenario::AddScenarioEntity(const ScenarioEntity& entity)
{
    entities.push_back(entity);
}

void Scenario::AddScenarioGroupsByEntityNames(const std::map<std::string, std::vector<std::string>> &groupDefinitions)
{
    std::vector<ScenarioEntity*> groupEntities;
    for (auto groupDefinition : groupDefinitions)
    {
        for (const auto &memberName : groupDefinition.second)
        {
            const auto groupEntityIterator = std::find_if(entities.begin(),
                                                          entities.end(),
                                                          [&memberName](const auto entity)
                                                          {
                                                            return entity.name == memberName;
                                                          });
            groupEntities.push_back(&(*groupEntityIterator));
        }

        scenarioGroups.insert({groupDefinition.first, groupEntities});
    }
}

const std::vector<ScenarioEntity> &Scenario::GetEntities() const
{
    return entities;
}

const std::vector<ScenarioEntity*> &Scenario::GetScenarioEntities() const
{
    try
    {
        return scenarioGroups.at("ScenarioAgents");
    }
    catch (const std::out_of_range& err)
    {
        Q_UNUSED(err);
        throw std::runtime_error("ScenarioAgents group not found.");
    }
}

const std::map<std::string, std::vector<ScenarioEntity*>> &Scenario::GetScenarioGroups() const
{
    return scenarioGroups;
}

void Scenario::AddConditionalEventDetector(const openScenario::ConditionalEventDetectorInformation &eventDetectorInformation)
{
    eventDetectorInformations.emplace_back(eventDetectorInformation);
}

void Scenario::AddAction(const openScenario::Action action, const std::string eventName)
{
    actions.emplace_back(action, eventName);
}

const std::vector<openScenario::ConditionalEventDetectorInformation>& Scenario::GetEventDetectorInformations() const
{
    return eventDetectorInformations;
}

std::vector<openScenario::ManipulatorInformation> Scenario::GetActions() const
{
    return actions;
}

int Scenario::GetEndTime() const
{
    // we add plus one here to align with the "greater_than" rule
    // Time is parsed in seconds, but we use ms internally (* 1000)
    return static_cast<int>((std::rint(endTimeInSeconds * 1000))) + 1;
}

void Scenario::SetEndTime(const double endTime)
{
    this->endTimeInSeconds = endTime;
}

void Scenario::SetEnvironment(const openScenario::EnvironmentAction& environment)
{
    sceneryDynamics.SetEnvironment(environment);
}

} // namespace core
