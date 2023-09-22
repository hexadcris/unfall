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
/** \file  simulationConfig.h
*	\brief This file stores all information of the simulation configuration.
*/
//-----------------------------------------------------------------------------

#pragma once

#include "include/simulationConfigInterface.h"
#include "profiles.h"
#include "common/opExport.h"
#include "common/spawnPointLibraryDefinitions.h"

namespace Configuration {

//-----------------------------------------------------------------------------
/** \brief Container class for the simulation configuration
* 	\details This class stores all information provided by the simulationConfig.xml
*            and provides access to this information
*/
//-----------------------------------------------------------------------------
class SIMULATIONCOREEXPORT SimulationConfig : public SimulationConfigInterface
{
public:
    virtual const ExperimentConfig &GetExperimentConfig() const override;

    virtual void SetExperimentConfig(const ExperimentConfig& experimentConfig) override;

    virtual const ScenarioConfig &GetScenarioConfig() const override;

    virtual void SetScenarioConfig(const ScenarioConfig& scenarioConfig) override;

    virtual const SpawnPointLibraryInfoCollection& GetSpawnPointsConfig() const override;

    virtual void AddSpawnPointInfo(const SpawnPointLibraryInfo& spawnPointInfo) override;

    virtual const ObservationInstanceCollection& GetObservationConfig() const override;

    virtual void AddObservationInstance(const ObservationInstance& observationInstance) override;

    virtual const EnvironmentConfig &GetEnvironmentConfig() const override;

    virtual void SetEnvironmentConfig(const EnvironmentConfig& environmentConfig) override;

    virtual const std::string& GetProfilesCatalog() const override;

    virtual void SetProfilesCatalog(const std::string& profilesCatalog) override;

private:
    ExperimentConfig experimentConfig {};
    ScenarioConfig scenarioConfig {};
    SpawnPointLibraryInfoCollection spawnPointsConfig {};
    ObservationInstanceCollection observationConfig {};
    EnvironmentConfig environmentConfig {};
    std::string profilesCatalog {};
};

} //namespace Configuration
