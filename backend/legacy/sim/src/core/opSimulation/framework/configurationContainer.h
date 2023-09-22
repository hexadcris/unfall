/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  ConfigurationContainer.h
*   \brief This file stores all configurations
*/
//-----------------------------------------------------------------------------

#pragma once

#include <unordered_map>

#include "simulationConfig.h"
#include "simulationConfigImporter.h"
#include "include/configurationContainerInterface.h"
#include "profilesImporter.h"
#include "scenery.h"
#include "sceneryImporter.h"
#include "systemConfigImporter.h"
#include "systemConfig.h"
#include "scenario.h"
#include "scenarioImporter.h"
#include "systemConfigImporter.h"
#include "vehicleModels.h"
#include "vehicleModelsImporter.h"
#include "configurationFiles.h"

namespace Configuration {

//-----------------------------------------------------------------------------
/** \brief This class imports and stores all configurations
*   \details
*/
//-----------------------------------------------------------------------------
class SIMULATIONCOREEXPORT ConfigurationContainer : public ConfigurationContainerInterface
{
public:
    ConfigurationContainer(const ConfigurationFiles& configurationFiles, const openpass::common::RuntimeInformation& runtimeInformation) :
        configurationFiles{configurationFiles},
        runtimeInformation(runtimeInformation)
    {}

    virtual ~ConfigurationContainer() override = default;

    /*!
    * \brief This functions imports all confiugrations
    *
    * \details This function imports all configurations and stores them.
    *
    * @return        true, if successful
    */
    bool ImportAllConfigurations() override;

    /*!
    * \brief Returns a pointer to the SystemConfigBlueprint
    *
    * @return        SystemConfigBlueprint pointer
    */
    std::shared_ptr<SystemConfigInterface> GetSystemConfigBlueprint() const override;

    /*!
    * \brief Returns a pointer to the SimulationConfig
    *
    * @return        SimulationConfig pointer
    */
    const SimulationConfigInterface* GetSimulationConfig() const override;

    /*!
    * \brief Returns a pointer to the Profiles
    *
    * @return        Profiles pointer
    */
    const ProfilesInterface* GetProfiles() const override;

    /*!
    * \brief Returns a pointer to the Scenery
    *
    * @return        Scenery pointer
    */
    const SceneryInterface* GetScenery() const override;

    /*!
    * \brief Returns a pointer to the Scenario
    *
    * @return        Scenario pointer
    */
    const ScenarioInterface* GetScenario() const override;

    /*!
    * \brief Returns imported systemConfigs
    *
    * @return        systemConfigs map
    */
    const std::map<std::string, std::shared_ptr<SystemConfigInterface>>& GetSystemConfigs() const override;

    /*!
    * \brief Returns a pointer to the VehicleModels
    *
    * @return        VehicleModels pointer
    */
    const VehicleModelsInterface* GetVehicleModels() const override;

    /*!
    * \brief Returns the RunTimeInformation
    *
    * @return        RunTimeInformation
    */
    const openpass::common::RuntimeInformation& GetRuntimeInformation() const override;

private:
    const ConfigurationFiles& configurationFiles;

    std::shared_ptr<SystemConfig> systemConfigBlueprint;
    SimulationConfig simulationConfig;
    Scenery scenery;
    Scenario scenario;
    std::map<std::string, std::shared_ptr<SystemConfigInterface>> systemConfigs;
    VehicleModels vehicleModels;
    Profiles profiles;
    openpass::common::RuntimeInformation runtimeInformation;
};

} //namespace Configuration
