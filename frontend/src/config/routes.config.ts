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

import GlobalSetup from "../components/interfaces/GlobalSetup/GlobalSetup";
import Home from "../components/interfaces/Home/Home";
import PostProcessing from "../components/interfaces/PostProcessing/PostProcessing";
import SetupConfig from "../components/interfaces/SetupConfigs/SetupConfig";
import SimulationManager from "../components/interfaces/SimulationManager/SimulationManager";
import SimulationReport from "../components/interfaces/SimulationReport/SimulationReport";
import SystemEditor from "../components/interfaces/SystemEditor/SystemEditor";
import { PATHS, RouteType } from "../index.d";

export const appRoutes: RouteType[] = [
    {
        path: PATHS.HOME,
        element: Home
    },
    {
        path: PATHS.GLOBAL_SETUP,
        element: GlobalSetup
    },
    {
        path: PATHS.SYSTEM_EDITOR,
        element: SystemEditor
    },
    {
        path: PATHS.SETUP_CONFIGS,
        element: SetupConfig
    },
    {
        path: PATHS.SIMULATION_MANAGER,
        element: SimulationManager
    },
    {
        path: PATHS.SIMULATION_REPORT,
        element: SimulationReport
    },
    {
        path: PATHS.POST_PROCESSING,
        element: PostProcessing
    }
];

