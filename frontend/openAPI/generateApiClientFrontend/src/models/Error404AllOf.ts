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
 * @interface Error404AllOf
 */
export interface Error404AllOf {
    /**
     * 
     * @type {string}
     * @memberof Error404AllOf
     */
    message: string;
}

/**
 * Check if a given object implements the Error404AllOf interface.
 */
export function instanceOfError404AllOf(value: object): boolean {
    let isInstance = true;
    isInstance = isInstance && "message" in value;

    return isInstance;
}

export function Error404AllOfFromJSON(json: any): Error404AllOf {
    return Error404AllOfFromJSONTyped(json, false);
}

export function Error404AllOfFromJSONTyped(json: any, ignoreDiscriminator: boolean): Error404AllOf {
    if ((json === undefined) || (json === null)) {
        return json;
    }
    return {
        
        'message': json['message'],
    };
}

export function Error404AllOfToJSON(value?: Error404AllOf | null): any {
    if (value === undefined) {
        return undefined;
    }
    if (value === null) {
        return null;
    }
    return {
        
        'message': value.message,
    };
}
