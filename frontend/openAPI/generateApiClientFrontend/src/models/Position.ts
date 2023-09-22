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
 * @interface Position
 */
export interface Position {
    /**
     * 
     * @type {number}
     * @memberof Position
     */
    x: number;
    /**
     * 
     * @type {number}
     * @memberof Position
     */
    y: number;
}

/**
 * Check if a given object implements the Position interface.
 */
export function instanceOfPosition(value: object): boolean {
    let isInstance = true;
    isInstance = isInstance && "x" in value;
    isInstance = isInstance && "y" in value;

    return isInstance;
}

export function PositionFromJSON(json: any): Position {
    return PositionFromJSONTyped(json, false);
}

export function PositionFromJSONTyped(json: any, ignoreDiscriminator: boolean): Position {
    if ((json === undefined) || (json === null)) {
        return json;
    }
    return {
        
        'x': json['x'],
        'y': json['y'],
    };
}

export function PositionToJSON(value?: Position | null): any {
    if (value === undefined) {
        return undefined;
    }
    if (value === null) {
        return null;
    }
    return {
        
        'x': value.x,
        'y': value.y,
    };
}

