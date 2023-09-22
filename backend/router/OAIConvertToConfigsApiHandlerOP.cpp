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
#include <QNetworkReply>

#include "OAIConvertToConfigsApiHandlerOP.h"
#include "OAIConvertToConfigsApiRequest.h"
#include "OAICustomHelpers.h"
#include "OPGUICore.h"


namespace OpenAPI {

    OAIConvertToConfigsApiHandlerOP::OAIConvertToConfigsApiHandlerOP() {

    }

    OAIConvertToConfigsApiHandlerOP::~OAIConvertToConfigsApiHandlerOP() {

    }

    void OAIConvertToConfigsApiHandlerOP::apiConvertToConfigsPost(OAIConfigsRequest oai_configs_request) {
        auto reqObj = qobject_cast<OAIConvertToConfigsApiRequest*>(sender());
        OAIDefault200Response res;
        if( reqObj != nullptr )
        {
            QString errorMsg; 
            bool success = OPGUICore::api_convert_to_configs(oai_configs_request,res, errorMsg);

            if (success)
            {
                reqObj->apiConvertToConfigsPostResponse(res);
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
