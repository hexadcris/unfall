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

import { exists, mapValues } from '../runtime';
/**
 * 
 * @export
 * @interface SelectedExperimentsRequest
 */
export interface SelectedExperimentsRequest {
    /**
     * 
     * @type {Array<string>}
     * @memberof SelectedExperimentsRequest
     */
    selectedExperiments: Array<string>;
}

/**
 * Check if a given object implements the SelectedExperimentsRequest interface.
 */
export function instanceOfSelectedExperimentsRequest(value: object): boolean {
    let isInstance = true;
    isInstance = isInstance && "selectedExperiments" in value;

    return isInstance;
}

export function SelectedExperimentsRequestFromJSON(json: any): SelectedExperimentsRequest {
    return SelectedExperimentsRequestFromJSONTyped(json, false);
}

export function SelectedExperimentsRequestFromJSONTyped(json: any, ignoreDiscriminator: boolean): SelectedExperimentsRequest {
    if ((json === undefined) || (json === null)) {
        return json;
    }
    return {
        
        'selectedExperiments': json['selectedExperiments'],
    };
}

export function SelectedExperimentsRequestToJSON(value?: SelectedExperimentsRequest | null): any {
    if (value === undefined) {
        return undefined;
    }
    if (value === null) {
        return null;
    }
    return {
        
        'selectedExperiments': value.selectedExperiments,
    };
}

