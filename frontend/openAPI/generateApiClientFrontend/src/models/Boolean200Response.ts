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
 * @interface Boolean200Response
 */
export interface Boolean200Response {
    /**
     * 
     * @type {boolean}
     * @memberof Boolean200Response
     */
    response: boolean;
}

/**
 * Check if a given object implements the Boolean200Response interface.
 */
export function instanceOfBoolean200Response(value: object): boolean {
    let isInstance = true;
    isInstance = isInstance && "response" in value;

    return isInstance;
}

export function Boolean200ResponseFromJSON(json: any): Boolean200Response {
    return Boolean200ResponseFromJSONTyped(json, false);
}

export function Boolean200ResponseFromJSONTyped(json: any, ignoreDiscriminator: boolean): Boolean200Response {
    if ((json === undefined) || (json === null)) {
        return json;
    }
    return {
        
        'response': json['response'],
    };
}

export function Boolean200ResponseToJSON(value?: Boolean200Response | null): any {
    if (value === undefined) {
        return undefined;
    }
    if (value === null) {
        return null;
    }
    return {
        
        'response': value.response,
    };
}

