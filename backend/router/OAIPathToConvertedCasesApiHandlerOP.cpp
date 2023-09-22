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

#include "OAIError500.h"
#include "OAIError400.h"
#include "OAICustomHelpers.h"
#include "OAIPathToConvertedCasesApiHandlerOP.h"
#include "OAIPathToConvertedCasesApiRequest.h"
#include "OPGUICore.h"


namespace OpenAPI {

    OAIPathToConvertedCasesApiHandlerOP::OAIPathToConvertedCasesApiHandlerOP() {

    }

    OAIPathToConvertedCasesApiHandlerOP::~OAIPathToConvertedCasesApiHandlerOP() {

    }

    void OAIPathToConvertedCasesApiHandlerOP::apiPathToConvertedCasesPost(OAIPathRequest oai_path_request) {
        OAIDefault200Response res;
        QString errorMsg;
        auto reqObj = qobject_cast<OAIPathToConvertedCasesApiRequest*>(sender());
        
        if( reqObj != nullptr )
        {
             
            bool success = OPGUICore::api_path_to_converted_cases(oai_path_request,res,errorMsg);
            if (success)
            {
                reqObj->apiPathToConvertedCasesPostResponse(res);
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
