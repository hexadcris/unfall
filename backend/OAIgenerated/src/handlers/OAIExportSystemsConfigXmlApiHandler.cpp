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

#include "OAIExportSystemsConfigXmlApiHandler.h"
#include "OAIExportSystemsConfigXmlApiRequest.h"

namespace OpenAPI {

OAIExportSystemsConfigXmlApiHandler::OAIExportSystemsConfigXmlApiHandler(){

}

OAIExportSystemsConfigXmlApiHandler::~OAIExportSystemsConfigXmlApiHandler(){

}

void OAIExportSystemsConfigXmlApiHandler::apiExportSystemsConfigXmlPost(QList<OAISystemUI> oai_system_ui) {
    Q_UNUSED(oai_system_ui);
    auto reqObj = qobject_cast<OAIExportSystemsConfigXmlApiRequest*>(sender());
    if( reqObj != nullptr )
    {
        OAIDefault200Response res;
        reqObj->apiExportSystemsConfigXmlPostResponse(res);
    }
}


}
