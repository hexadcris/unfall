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
  Error400,
  Error500,
  SystemUI,
} from '../models';
import {
    Default200ResponseFromJSON,
    Default200ResponseToJSON,
    Error400FromJSON,
    Error400ToJSON,
    Error500FromJSON,
    Error500ToJSON,
    SystemUIFromJSON,
    SystemUIToJSON,
} from '../models';

export interface ApiExportSystemsConfigXmlPostRequest {
    systemUI: Array<SystemUI>;
}

/**
 * 
 */
export class ExportSystemsConfigXmlApi extends runtime.BaseAPI {

    /**
     * This endpoint receives an array of systems and returns the name of the file once exported into an xml file
     * Receive an array of systems and exports it to an xml file
     */
    async apiExportSystemsConfigXmlPostRaw(requestParameters: ApiExportSystemsConfigXmlPostRequest, initOverrides?: RequestInit | runtime.InitOverrideFunction): Promise<runtime.ApiResponse<Default200Response>> {
        if (requestParameters.systemUI === null || requestParameters.systemUI === undefined) {
            throw new runtime.RequiredError('systemUI','Required parameter requestParameters.systemUI was null or undefined when calling apiExportSystemsConfigXmlPost.');
        }

        const queryParameters: any = {};

        const headerParameters: runtime.HTTPHeaders = {};

        headerParameters['Content-Type'] = 'application/json';

        const response = await this.request({
            path: `/api/exportSystemsConfigXml`,
            method: 'POST',
            headers: headerParameters,
            query: queryParameters,
            body: requestParameters.systemUI.map(SystemUIToJSON),
        }, initOverrides);

        return new runtime.JSONApiResponse(response, (jsonValue) => Default200ResponseFromJSON(jsonValue));
    }

    /**
     * This endpoint receives an array of systems and returns the name of the file once exported into an xml file
     * Receive an array of systems and exports it to an xml file
     */
    async apiExportSystemsConfigXmlPost(requestParameters: ApiExportSystemsConfigXmlPostRequest, initOverrides?: RequestInit | runtime.InitOverrideFunction): Promise<Default200Response> {
        const response = await this.apiExportSystemsConfigXmlPostRaw(requestParameters, initOverrides);
        return await response.value();
    }

}
