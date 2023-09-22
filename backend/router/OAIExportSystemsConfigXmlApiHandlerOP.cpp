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

#include "OAIExportSystemsConfigXmlApiHandlerOP.h"
#include "OAIExportSystemsConfigXmlApiRequest.h"
#include "OAICustomHelpers.h"
#include "OPGUICore.h"

namespace OpenAPI {

OAIExportSystemsConfigXmlApiHandlerOP::OAIExportSystemsConfigXmlApiHandlerOP(){

}

OAIExportSystemsConfigXmlApiHandlerOP::~OAIExportSystemsConfigXmlApiHandlerOP(){

}

void OAIExportSystemsConfigXmlApiHandlerOP::apiExportSystemsConfigXmlPost(QList<OAISystemUI> oai_system_ui) {
    Q_UNUSED(oai_system_ui);
    auto reqObj = qobject_cast<OAIExportSystemsConfigXmlApiRequest*>(sender());

    OAIDefault200Response res;

    if( reqObj != nullptr )
    {
        QString errorMsg; 
        bool success = OPGUICore::api_export_systems_config(oai_system_ui,res, errorMsg);

        if (success)
        {
            reqObj->apiExportSystemsConfigXmlPostResponse(res);
        }
        else
        {
            int code=500;
            OAIError500 error;
            error.setMessage("Server error");
            if (!errorMsg.isEmpty()) {
                error.setMessage(error.getMessage() + " - Details: " + errorMsg);
            }
            handleSocketResponse(reqObj, error.asJsonObject(), code);
        }
    }
    else{
            int code=400;
            OAIError400 error;
            error.setMessage("Bad client request");
            handleSocketResponse(reqObj, error.asJsonObject(), code);
    }
}


}
