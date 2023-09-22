/* tslint:disable */
/* eslint-disable */
/**
 * opGUI - OpenAPI 3.0
 * Dummy description about the available endpoints Some useful links: - [The opGUI repository](https://gitlab.eclipse.org/eclipse/openpass/opgui) - [The documentation page](https://www.eclipse.org/openpass/content/html/index.html)
 *
 * The version of the OpenAPI document: 1.1.0
 * 
 *
 * NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
 * https://openapi-generator.tech
 * Do not edit the class manually.
 */


import * as runtime from '../runtime';
import type {
  Default200Response,
  OpSimulationManagerXmlRequest,
} from '../models';
import {
    Default200ResponseFromJSON,
    Default200ResponseToJSON,
    OpSimulationManagerXmlRequestFromJSON,
    OpSimulationManagerXmlRequestToJSON,
} from '../models';

export interface ApiExportOpsimulationManagerXmlPostRequest {
    opSimulationManagerXmlRequest: OpSimulationManagerXmlRequest;
}

/**
 * 
 */
export class ExportOpsimulationManagerXmlApi extends runtime.BaseAPI {

    /**
     * send a JSON request to the backend containing all the defined parameters to be exported to the opSimulationManagerXml file
     * export valid data into the opSimulationManagerXml file
     */
    async apiExportOpsimulationManagerXmlPostRaw(requestParameters: ApiExportOpsimulationManagerXmlPostRequest, initOverrides?: RequestInit | runtime.InitOverrideFunction): Promise<runtime.ApiResponse<Default200Response>> {
        if (requestParameters.opSimulationManagerXmlRequest === null || requestParameters.opSimulationManagerXmlRequest === undefined) {
            throw new runtime.RequiredError('opSimulationManagerXmlRequest','Required parameter requestParameters.opSimulationManagerXmlRequest was null or undefined when calling apiExportOpsimulationManagerXmlPost.');
        }

        const queryParameters: any = {};

        const headerParameters: runtime.HTTPHeaders = {};

        headerParameters['Content-Type'] = 'application/json';

        const response = await this.request({
            path: `/api/exportOpsimulationManagerXml`,
            method: 'POST',
            headers: headerParameters,
            query: queryParameters,
            body: OpSimulationManagerXmlRequestToJSON(requestParameters.opSimulationManagerXmlRequest),
        }, initOverrides);

        return new runtime.JSONApiResponse(response, (jsonValue) => Default200ResponseFromJSON(jsonValue));
    }

    /**
     * send a JSON request to the backend containing all the defined parameters to be exported to the opSimulationManagerXml file
     * export valid data into the opSimulationManagerXml file
     */
    async apiExportOpsimulationManagerXmlPost(requestParameters: ApiExportOpsimulationManagerXmlPostRequest, initOverrides?: RequestInit | runtime.InitOverrideFunction): Promise<Default200Response> {
        const response = await this.apiExportOpsimulationManagerXmlPostRaw(requestParameters, initOverrides);
        return await response.value();
    }

}
