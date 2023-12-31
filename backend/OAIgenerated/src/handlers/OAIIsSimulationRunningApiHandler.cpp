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

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QDebug>

#include "OAIIsSimulationRunningApiHandler.h"
#include "OAIIsSimulationRunningApiRequest.h"

namespace OpenAPI {

OAIIsSimulationRunningApiHandler::OAIIsSimulationRunningApiHandler(){

}

OAIIsSimulationRunningApiHandler::~OAIIsSimulationRunningApiHandler(){

}

void OAIIsSimulationRunningApiHandler::apiIsSimulationRunningGet() {
    auto reqObj = qobject_cast<OAIIsSimulationRunningApiRequest*>(sender());
    if( reqObj != nullptr )
    {
        OAIBoolean200Response res;
        reqObj->apiIsSimulationRunningGetResponse(res);
    }
}


}
