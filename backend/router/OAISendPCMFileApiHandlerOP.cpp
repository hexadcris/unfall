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

#include "OAICustomHelpers.h"
#include "OAISendPCMFileApiHandlerOP.h"
#include "OAISendPCMFileApiRequest.h"
#include "OPGUICore.h"
#include "OAIError500.h"
#include "OAIError400.h"

namespace OpenAPI {

    OAISendPCMFileApiHandlerOP::OAISendPCMFileApiHandlerOP() {

    }

    OAISendPCMFileApiHandlerOP::~OAISendPCMFileApiHandlerOP() {

    }

    void OAISendPCMFileApiHandlerOP::apiSendPCMFilePost(OAIPathRequest oai_path_request) {
        QString errorMsg;
        OAISelectedExperimentsRequest resExperiments;
        auto reqObj = qobject_cast<OAISendPCMFileApiRequest*>(sender());
        if( reqObj != nullptr )
        {
            bool success = OPGUICore::api_send_PCM_file(oai_path_request,resExperiments,errorMsg);
            if (success)
            {
                reqObj->apiSendPCMFilePostResponse(resExperiments);
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
