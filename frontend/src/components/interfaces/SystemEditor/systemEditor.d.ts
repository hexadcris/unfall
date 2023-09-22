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

import { ComponentUI as Component, Connection, Output } from 'opGUI-client-frontend';
interface ComponentList {
    [key: string]: Component[]
}
type ConnectionList = Connection & { 
    indexComponentInput: number;
    indexComponentOutput: number;
    labels?: OverridableComponent<SvgIconTypeMap<{}, "svg">>  | string;
 };

type ExtendedComponent = Component & { index: number };
interface DragComponent extends Component {
    offsetX: number;
    offsetY: number;
    position: {
        x: number,
        y: number
    };
    height: number;
    width: number;
    color: string;
    dropped: boolean;
    [key: string]: any;
}
type ConnectionComponent = {
    component: DragComponent;
    output: Output;
    index: number;
}

interface Connections {
    id: number;
    source: {
        component: number;
        output: number;
    }
    target: {
        component: number;
        input: number;
    }
}

interface System {
    id: number | string;
    title: string;
    priority: number;
    anchorEl: null | HTMLElement | SVGElement;
    components: Component[];
    connections: Connections[];
    file?: string;
    deleted?: boolean;
}

interface ISystemEditor {
    pathToComponents: string;
    components: ComponentList;
    componentsOriginal: Component[];
    systems: System[];
    deletedSystems: System[];
    hideConnections: boolean;
    selectedSystem: System | null;
    currentStartIndex: number;
    currentEndIndex: number;
    showAccordion: boolean;
    availableElements: any[];
    connections: ConnectionList[];
    newSystemName: string;
    renamingSystemId: number | string | nullÏ;
}

export const BASE_DIVISOR = 2;
