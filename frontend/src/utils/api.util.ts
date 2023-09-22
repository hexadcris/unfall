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
    Configuration,
    VerifyPathApi,
    ExportOpsimulationManagerXmlApi,
    OpSimulationManagerXmlRequest,
    RunOpSimulationManagerApi,
    DeleteInformationApi,
    IsSimulationRunningApi,
    ComponentsApi,
    SystemsApi,
    ExportSystemsConfigXmlApi,
    SystemUI,
    SendPCMFileApi,
    ConvertToConfigsApi,
    ExportToSimulationApi,
    PathToConvertedCasesApi,
    SystemRef
} from 'opGUI-client-frontend';
import { VITE_API_DEV_URL, VITE_API_LOCAL_URL, VITE_ENVIRONMENT } from './constants';
const environment = VITE_ENVIRONMENT;
const configuration = new Configuration({
    basePath: environment === 'dev' ? VITE_API_DEV_URL : VITE_API_LOCAL_URL
});
const verifyPathApi = new VerifyPathApi(configuration);
const exportOpsimulationManagerXmlApi = new ExportOpsimulationManagerXmlApi(configuration);
const runOpSimulationManagerApi = new RunOpSimulationManagerApi(configuration);
const deleteInformationApi = new DeleteInformationApi(configuration);
const isSimulationRunningApi = new IsSimulationRunningApi(configuration);
const componentsApi = new ComponentsApi(configuration);
const systemsApi = new SystemsApi(configuration);
const exportSystemsConfigXmlApi = new ExportSystemsConfigXmlApi(configuration);
const sendPCMFileApi = new SendPCMFileApi(configuration);
const convertToConfigsApi = new ConvertToConfigsApi(configuration);
const exportToSimulationApi = new ExportToSimulationApi(configuration);
const pathToConvertedCasesApi = new PathToConvertedCasesApi(configuration);

export const isSimRunning = async () => {
    try {
        const res = await isSimulationRunningApi.apiIsSimulationRunningGet();
        return res.response;
    } catch (error) {
        console.log(error);
    }

}
export const verifyPath = async (path: string) => {
    try {
        const res = await verifyPathApi.verifyPath({ pathRequest: { path: path } });
        return res;
    } catch (e) {
        console.log(e);
    }
};

export const exportXml = async (data: OpSimulationManagerXmlRequest) => {
    try {
        const res = await exportOpsimulationManagerXmlApi.apiExportOpsimulationManagerXmlPost({ opSimulationManagerXmlRequest: data })
        return res;
    } catch (e) {
        console.error(e);
    }
}

export const runSimulationManager = async () => {
    try {
        const res = await runOpSimulationManagerApi.apiRunOpSimulationManagerGet();
        console.log('Execution opSimManager OK!')
        return res.response;
    } catch (error) {
        console.log('Execution opSimManager Failed!')
    }
}

export const deleteInformation = async (path: string) => {
    try {
        const res = await deleteInformationApi.deleteInformation({ pathRequest: { path: path } });
        return res;
    } catch (error) {
        console.log(error);
    }
}
export const getComponentsApi = async () => {
    try {
        const res = await componentsApi.apiComponentsGet();
        return res;
    } catch (error) {
        console.log(error);
        console.log('Get Components Failed');
    }
}

export const loadSystemsApi = async () => {
    try {
        const res = await systemsApi.apiSystemsGet();
        return res;
    } catch (error) {
        console.log(error);
    }
}

export const exportSystemsApi = async (systems: SystemUI[]) => {
    try {
        const res = exportSystemsConfigXmlApi.apiExportSystemsConfigXmlPost({ systemUI: systems });
        return res;
    } catch (error) {
        console.log(error);
    }
}

export const sendPCMFile = async (path: string) => {
    try {
        const res = await sendPCMFileApi.apiSendPCMFilePost({ pathRequest: { path: path } });
        return res;
    } catch (error) {
        console.log(error);

    }
}
export const convertToConfigs = async (selectedExperiments: Array<string>, selectedSystems: SystemRef[]) => {
    try {
        const res = await convertToConfigsApi.apiConvertToConfigsPost({ configsRequest: {selectedExperiments, selectedSystems} });
        return res;
    } catch (error) {
        console.log(error);

    }
}
export const exportToSimulation = async (selectedExperiments: Array<string>) => {
    try {
        const res = await exportToSimulationApi.apiExportToSimulationsPost({ selectedExperimentsRequest: { selectedExperiments: selectedExperiments } });
        return res;
    } catch (error) {
        console.log(error);
    }
}
export const pathToConvertedCasesPost = async (path: string) => {
    try {
        const res = await pathToConvertedCasesApi.apiPathToConvertedCasesPost({ pathRequest: { path: path } });
        return res;
    } catch (error) {
        console.log(error);
    }
}
