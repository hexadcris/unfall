/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  simulationConfigInterface.h
//! @brief This file contains the interface to retrieve the parameters of the simulation configuration.
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <unordered_map>
#include <vector>

#include "include/parameterInterface.h"
#include "common/globalDefinitions.h"
#include "common/spawnPointLibraryDefinitions.h"
#include "common/observationLibraryDefinitions.h"

using IntProbabilities = std::vector<std::pair<int, double>>;
using DoubleProbabilities = std::vector<std::pair<double, double>>;
using StringProbabilities = std::vector<std::pair<std::string, double>>;
using StochasticDistributionProbabilities = std::vector<std::pair<openpass::parameter::StochasticDistribution, double>>;

struct ExperimentConfig
{
    using Libraries = std::unordered_map<std::string, std::string>;

    int experimentId;
    int numberOfInvocations;
    std::uint32_t randomSeed;
    Libraries libraries;
};

struct ScenarioConfig
{
    std::string scenarioPath {};                    //!< Path of the scenario file
};

struct EnvironmentConfig
{
    StringProbabilities timeOfDays {};
    IntProbabilities visibilityDistances {};
    DoubleProbabilities frictions {};
    StringProbabilities weathers {};
    std::string trafficRules {};
    TurningRates turningRates {};
};

//-----------------------------------------------------------------------------
//! Interface provides access to the simulation configuration parameters
//-----------------------------------------------------------------------------
class SimulationConfigInterface
{
public:
    SimulationConfigInterface() = default;
    SimulationConfigInterface(const SimulationConfigInterface&) = delete;
    SimulationConfigInterface(SimulationConfigInterface&&) = delete;
    SimulationConfigInterface& operator=(const SimulationConfigInterface&) = delete;
    SimulationConfigInterface& operator=(SimulationConfigInterface&&) = delete;
    virtual ~SimulationConfigInterface() = default;

    /*!
    * \brief Returns a reference to the experimentConfig
    *
    * @return        experimentConfig
    */
    virtual const ExperimentConfig& GetExperimentConfig() const = 0;

    /*!
     * \brief Sets experiment configuration to the one passed as an argument.
     *
     * @param[in]      experimentConfig      the experiment configuration to be adopted     
     */
    virtual void SetExperimentConfig(const ExperimentConfig& experimentConfig) = 0;

    /*!
    * \brief Returns a reference to the scenarioConfig
    *
    * @return        scenarioConfig
    */
    virtual const ScenarioConfig& GetScenarioConfig() const = 0;

    /*!
     * \brief Sets scenario configuration to the one passed as an argument.
     *
     * @param[in]      scenarioConfig      the scenario configuration to be adopted
     */
    virtual void SetScenarioConfig(const ScenarioConfig& scenarioConfig) = 0;

    /*!
    * \brief Returns a reference to the spawnPointsConfig
    *
    * @return        spawnPointsConfig
    */
    virtual const SpawnPointLibraryInfoCollection& GetSpawnPointsConfig() const = 0;

    /*!
    * \brief Add spawnPointInfo to the spawnPointsConfig
    *
    * @param[in]     spawnPointInfo        the new spawnPointInfo
    */
     virtual void AddSpawnPointInfo(const SpawnPointLibraryInfo& spawnPointInfo) = 0;

    /*!
    * \brief Returns a reference to the observationConfig
    *
    * @return        observationConfig
    */
    virtual const ObservationInstanceCollection& GetObservationConfig() const = 0;

    /*!
    * \brief Add observation instance to the observationConfig
    *
    * @param[in]     observationInstance    the new observation instance
    */
    virtual void AddObservationInstance(const ObservationInstance& observationInstance) = 0;

    /*!
    * \brief Returns a reference to the environmentConfig
    *
    * @return        environmentConfig
    */
    virtual const EnvironmentConfig& GetEnvironmentConfig() const = 0;

    /*!
    * \brief Sets environment configuration to the one passed as an argument.
    *
    * @param[in]      environmentConfig      the environment configuration to be adopted
    */
    virtual void SetEnvironmentConfig(const EnvironmentConfig& environmentConfig) = 0;

    /*!
     * \brief Returns the name of the profiles catalog
     */
    virtual const std::string& GetProfilesCatalog() const = 0;

    /*!
     * \brief Sets the name of the profiles catalog
     */
    virtual void SetProfilesCatalog(const std::string& profilesCatalog) = 0;
};
