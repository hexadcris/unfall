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

import { SimulationConfig } from "opGUI-client-frontend";

export interface Simulation extends SimulationConfig {
    id: number;
    selected: boolean;
    [key: string]: any;
}

declare global {
    interface Window {
        [key: string]: any
    }
}

export enum PATHS {
    HOME = '/',
    GLOBAL_SETUP = '/global-setup',
    SYSTEM_EDITOR = '/system-editor',
    SETUP_CONFIGS = '/setup-config',
    SIMULATION_MANAGER = '/simulation-manager',
    SIMULATION_REPORT = '/simulation-report',
    POST_PROCESSING = '/post-processing',
    ERROR = '/error',
}

export enum INPUT_FILE {
    dir = 'directory',
    file = 'file'
}

export const tabNames = [
    {
        name: "Homepage",
        pathTo: PATHS.HOME
    },
    {
        name: "System Editor",
        pathTo: PATHS.SYSTEM_EDITOR,
    },
    {
        name: "Pre",
        pathTo: PATHS.SETUP_CONFIGS,
        tooltipText: "Prepare the simulations configurations"
    },
    {
        name: "Run",
        pathTo: PATHS.SIMULATION_MANAGER,
        tooltipText: "Run the simulations"
    },
    {
        name: "Monitor",
        pathTo: PATHS.SIMULATION_REPORT,
        tooltipText: "Simulation Report"
    },
    {
        name: "Post",
        pathTo: PATHS.POST_PROCESSING,
        tooltipText: "Post-Processing"
    },
    {
        name: "Global-setup",
        pathTo: PATHS.GLOBAL_SETUP
    },

];

export interface RouteType {
    path: string
    element: React.ComponentType<any>;
}

export interface ModalButton {
    title: string
}

export interface IGenericRecord {
    [key: string]: string | number | boolean | any;
}
export enum Variant {
    contained = "contained",
    outlined = "outlined"
}

export enum ColorVariant {
    warning = "warning"
}
interface ActionButton {
    title: string;
    action?: (props?: IGenericRecord) => void;
    variant?: Variant;
    testId?: string;
    disabled?: boolean;
}

export interface IModal {
    title: string;
    description: string;
    active: boolean;
    buttons: ActionButton[];
    descriptionTitle?: string;
    icon?: OverridableComponent<SvgIconTypeMap<{}, "svg">> & { muiName: string; };
    iconVariant?: ColorVariant;
    loadingBar: boolean;
}