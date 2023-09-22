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

#include "OAIRunOpSimulationManagerApiHandlerOP.h"
#include "OAIRunOpSimulationManagerApiRequest.h"
#include "OAICustomHelpers.h"
#include "OAIDefault200Response.h"
#include "OAIError500.h"
#include "OPGUICore.h"

namespace OpenAPI {

    OAIRunOpSimulationManagerApiHandlerOP::OAIRunOpSimulationManagerApiHandlerOP() {

    }

    OAIRunOpSimulationManagerApiHandlerOP::~OAIRunOpSimulationManagerApiHandlerOP() {

    }

    void OAIRunOpSimulationManagerApiHandlerOP::apiRunOpSimulationManagerGet() {
        auto reqObj = qobject_cast<OAIRunOpSimulationManagerApiRequest*>(sender());
        OAIDefault200Response res;
        QString errorMsg;

        if (reqObj != nullptr)
        {
            bool success = OPGUICore::api_run_opSimulationManager(res,errorMsg);
            if (success)
            {
                std::string succesMsg = "The opSimulationManager execution has started.";
                reqObj->apiRunOpSimulationManagerGetResponse(res);
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
    }

}
