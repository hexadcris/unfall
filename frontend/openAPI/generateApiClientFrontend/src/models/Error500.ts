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
 * @interface Error500
 */
export interface Error500 {
    /**
     * 
     * @type {number}
     * @memberof Error500
     */
    code: number;
    /**
     * 
     * @type {string}
     * @memberof Error500
     */
    message: string;
}

/**
 * Check if a given object implements the Error500 interface.
 */
export function instanceOfError500(value: object): boolean {
    let isInstance = true;
    isInstance = isInstance && "code" in value;
    isInstance = isInstance && "message" in value;

    return isInstance;
}

export function Error500FromJSON(json: any): Error500 {
    return Error500FromJSONTyped(json, false);
}

export function Error500FromJSONTyped(json: any, ignoreDiscriminator: boolean): Error500 {
    if ((json === undefined) || (json === null)) {
        return json;
    }
    return {
        
        'code': json['code'],
        'message': json['message'],
    };
}

export function Error500ToJSON(value?: Error500 | null): any {
    if (value === undefined) {
        return undefined;
    }
    if (value === null) {
        return null;
    }
    return {
        
        'code': value.code,
        'message': value.message,
    };
}
