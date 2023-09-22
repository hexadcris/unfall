/********************************************************************************
 * Copyright (c) 2017-2018 ITK Engineering GmbH
 *               2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  simulationConfig.h
//! @brief This file contains the representation of the simulation
//!        configuration.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <optional>

namespace SimulationManager {
namespace Configuration {

class SimulationConfig
{
public:
    SimulationConfig(std::optional<std::string> logFile,
                std::optional<std::string> configs,
                std::optional<std::string> results) :
        logFile{logFile.value_or(defaultLogFile)},
        configs{configs.value_or(defaultConfigs)},
        results{results.value_or(defaultResults)}
    {}

    const std::string logFile;
    const std::string configs;
    const std::string results;

private:
    static constexpr char defaultLogFile[] = "opSimulation.log";
    static constexpr char defaultConfigs[] = "configs";
    static constexpr char defaultResults[] = "results";
};

} // namespace Configuration
} // namespace SimulationManager
