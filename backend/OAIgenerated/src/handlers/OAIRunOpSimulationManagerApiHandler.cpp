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

#include "OAIRunOpSimulationManagerApiHandler.h"
#include "OAIRunOpSimulationManagerApiRequest.h"

namespace OpenAPI {

OAIRunOpSimulationManagerApiHandler::OAIRunOpSimulationManagerApiHandler(){

}

OAIRunOpSimulationManagerApiHandler::~OAIRunOpSimulationManagerApiHandler(){

}

void OAIRunOpSimulationManagerApiHandler::apiRunOpSimulationManagerGet() {
    auto reqObj = qobject_cast<OAIRunOpSimulationManagerApiRequest*>(sender());
    if( reqObj != nullptr )
    {
        OAIDefault200Response res;
        reqObj->apiRunOpSimulationManagerGetResponse(res);
    }
}


}
