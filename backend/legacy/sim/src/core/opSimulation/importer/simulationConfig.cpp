/********************************************************************************
 * Copyright (c) 2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  simulationConfig.cpp */
//-----------------------------------------------------------------------------

#include "simulationConfig.h"

namespace Configuration
{

const ExperimentConfig &SimulationConfig::GetExperimentConfig() const
{
    return experimentConfig;
}

void SimulationConfig::SetExperimentConfig(const ExperimentConfig& experimentConfig)
{
    this->experimentConfig = experimentConfig;
}

const ScenarioConfig &SimulationConfig::GetScenarioConfig() const
{
    return scenarioConfig;
}

void SimulationConfig::SetScenarioConfig(const ScenarioConfig& scenarioConfig)
{
    this->scenarioConfig = scenarioConfig;
}

const SpawnPointLibraryInfoCollection& SimulationConfig::GetSpawnPointsConfig() const
{
    return spawnPointsConfig;
}

void SimulationConfig::AddSpawnPointInfo(const SpawnPointLibraryInfo& spawnPointInfo)
{
    spawnPointsConfig.emplace_back(spawnPointInfo);
}

const ObservationInstanceCollection& SimulationConfig::GetObservationConfig() const
{
    return observationConfig;
}

void SimulationConfig::AddObservationInstance(const ObservationInstance& observationInstance)
{
    observationConfig.emplace_back(observationInstance);
}

const EnvironmentConfig &SimulationConfig::GetEnvironmentConfig() const
{
    return environmentConfig;
}

void SimulationConfig::SetEnvironmentConfig(const EnvironmentConfig& environmentConfig)
{
    this->environmentConfig = environmentConfig;
}

const std::string &SimulationConfig::GetProfilesCatalog() const
{
    return profilesCatalog;
}

void SimulationConfig::SetProfilesCatalog(const std::string& profilesCatalog)
{
    this->profilesCatalog = profilesCatalog;
}

} //namespace Configuration
