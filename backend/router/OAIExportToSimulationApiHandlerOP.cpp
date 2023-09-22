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
#include "OAIExportToSimulationApiHandlerOP.h"
#include "OAIExportToSimulationApiRequest.h"
#include "OPGUICore.h"


namespace OpenAPI {

    OAIExportToSimulationApiHandlerOP::OAIExportToSimulationApiHandlerOP() {

    }

    OAIExportToSimulationApiHandlerOP::~OAIExportToSimulationApiHandlerOP() {

    }

    void OAIExportToSimulationApiHandlerOP::apiExportToSimulationsPost(OAISelectedExperimentsRequest oai_selected_experiments_request) {
        auto reqObj = qobject_cast<OAIExportToSimulationApiRequest*>(sender());
        OAIDefault200Response res;

        if( reqObj != nullptr )
        {
            QString errorMsg; 
            auto success = OPGUICore::api_export_to_simulations(oai_selected_experiments_request,res,errorMsg);

            if (success)
            {
                reqObj->apiExportToSimulationsPostResponse(res);
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
