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
  Error403,
  PathRequest,
  SelectedExperimentsRequest,
} from '../models';
import {
    Error403FromJSON,
    Error403ToJSON,
    PathRequestFromJSON,
    PathRequestToJSON,
    SelectedExperimentsRequestFromJSON,
    SelectedExperimentsRequestToJSON,
} from '../models';

export interface ApiSendPCMFilePostRequest {
    pathRequest: PathRequest;
}

/**
 * 
 */
export class SendPCMFileApi extends runtime.BaseAPI {

    /**
     * send a .db file to the backend to it will be processed and return a list of ID\'s of the experiments contained
     * send a PCM formatted file to the backend to be processed
     */
    async apiSendPCMFilePostRaw(requestParameters: ApiSendPCMFilePostRequest, initOverrides?: RequestInit | runtime.InitOverrideFunction): Promise<runtime.ApiResponse<SelectedExperimentsRequest>> {
        if (requestParameters.pathRequest === null || requestParameters.pathRequest === undefined) {
            throw new runtime.RequiredError('pathRequest','Required parameter requestParameters.pathRequest was null or undefined when calling apiSendPCMFilePost.');
        }

        const queryParameters: any = {};

        const headerParameters: runtime.HTTPHeaders = {};

        headerParameters['Content-Type'] = 'application/json';

        const response = await this.request({
            path: `/api/sendPCMFile`,
            method: 'POST',
            headers: headerParameters,
            query: queryParameters,
            body: PathRequestToJSON(requestParameters.pathRequest),
        }, initOverrides);

        return new runtime.JSONApiResponse(response, (jsonValue) => SelectedExperimentsRequestFromJSON(jsonValue));
    }

    /**
     * send a .db file to the backend to it will be processed and return a list of ID\'s of the experiments contained
     * send a PCM formatted file to the backend to be processed
     */
    async apiSendPCMFilePost(requestParameters: ApiSendPCMFilePostRequest, initOverrides?: RequestInit | runtime.InitOverrideFunction): Promise<SelectedExperimentsRequest> {
        const response = await this.apiSendPCMFilePostRaw(requestParameters, initOverrides);
        return await response.value();
    }

}
