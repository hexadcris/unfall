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

#include "OAIComponentsApiHandlerOP.h"
#include "OAIComponentsApiRequest.h"
#include "OAIComponentUI.h"
#include "OAIError500.h"
#include "OAICustomHelpers.h"
#include "OPGUICore.h"

namespace OpenAPI {

    OAIComponentsApiHandlerOP::OAIComponentsApiHandlerOP() {

    }

    OAIComponentsApiHandlerOP::~OAIComponentsApiHandlerOP() {

    }

    void OAIComponentsApiHandlerOP::apiComponentsGet() {
        auto reqObj = qobject_cast<OAIComponentsApiRequest*>(sender());
        QList<OAIComponentUI> res;

        if (reqObj != nullptr)
        {
            QString errorMsg; // Create a QString to hold error messages
            bool success = OPGUICore::api_get_components(res, errorMsg); // Pass errorMsg

            if (success)
            {
                reqObj->apiComponentsGetResponse(res);
            }
            else
            {
                int code=500;
                OAIError500 error;
                error.setMessage("Server error");
                if (!errorMsg.isEmpty()) {
                    error.setMessage(error.getMessage() + " - Details: " + errorMsg);
                }
                error.setCode(code);
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
