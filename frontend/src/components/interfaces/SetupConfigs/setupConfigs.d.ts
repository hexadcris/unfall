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

import { SystemRef } from "opGUI-client-frontend";

export interface ISimulationId {
    id: number;
    selected: boolean;
}

export interface IAgent extends SystemRef {
    selected: boolean;
}

export interface ISetupConfig {
    simulations: ISimulationId[];
    agents: IAgent[];
    pathToPcm: string;
    configSet: string;
    pathToConvertedCases: string;
    searchTerm: string;
    fileFromList: string;
    anchorEl: null | HTMLElement;
    allSelected: boolean;
    isExportDisabled: boolean;
}
export const SIMULATIONS = 'simulations';