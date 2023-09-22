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

#include "OAIComponentsApiHandler.h"
#include "OAIComponentsApiRequest.h"

namespace OpenAPI {

OAIComponentsApiHandler::OAIComponentsApiHandler(){

}

OAIComponentsApiHandler::~OAIComponentsApiHandler(){

}

void OAIComponentsApiHandler::apiComponentsGet() {
    auto reqObj = qobject_cast<OAIComponentsApiRequest*>(sender());
    if( reqObj != nullptr )
    {
        QList<OAIComponentUI> res;
        reqObj->apiComponentsGetResponse(res);
    }
}


}
