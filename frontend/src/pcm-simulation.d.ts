/*
 * Copyright (c) 2023 Hexad GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 */

import {
    OpSimulationManagerXml as opSimXml,
    SimulationConfig as simConfig
}
    from 'opGUI-client-frontend';

export interface SimulationConfig {
    logFileSimulation: simConfig.logFileSimulation,
    configurations: simConfig.configurations,
    results: simConfig.results,
    numberOfExecutions: simConfig.numberOfExecutions
}

export interface OpSimulationManagerXml {
    logLevel: opSimXml.logLevel,
    logFileSimulationManager: opSimXml.logFileSimulationManager,
    simulation: opSimXml.simulation,
    libraries: opSimXml.libraries
    simulationConfigs: SimulationConfig | SimulationConfig[]
}

export const SIMULATION_CONFIG_FIELDS_XML = 3;
export const SIMULATION_CONFIG_XML = 'simulationConfig';