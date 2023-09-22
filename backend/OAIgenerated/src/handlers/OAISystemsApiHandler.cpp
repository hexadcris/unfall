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

#include "OAISystemsApiHandler.h"
#include "OAISystemsApiRequest.h"

namespace OpenAPI {

OAISystemsApiHandler::OAISystemsApiHandler(){

}

OAISystemsApiHandler::~OAISystemsApiHandler(){

}

void OAISystemsApiHandler::apiSystemsGet() {
    auto reqObj = qobject_cast<OAISystemsApiRequest*>(sender());
    if( reqObj != nullptr )
    {
        QList<OAISystemUI> res;
        reqObj->apiSystemsGetResponse(res);
    }
}


}
