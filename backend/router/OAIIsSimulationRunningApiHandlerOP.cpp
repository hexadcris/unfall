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

#include "OAIIsSimulationRunningApiHandlerOP.h"
#include "OAIIsSimulationRunningApiRequest.h"
#include "OAICustomHelpers.h"
#include "OPGUICore.h"
#include "OAIError400.h"
#include "OAIError500.h"

namespace OpenAPI {

OAIIsSimulationRunningApiHandlerOP::OAIIsSimulationRunningApiHandlerOP(){

}

OAIIsSimulationRunningApiHandlerOP::~OAIIsSimulationRunningApiHandlerOP(){

}

void OAIIsSimulationRunningApiHandlerOP::apiIsSimulationRunningGet() {
    auto reqObj = qobject_cast<OAIIsSimulationRunningApiRequest*>(sender());

    if( reqObj != nullptr )
    {
        OAIBoolean200Response res;
        QString errorMsg; 
        //bool success = OPGUICore::api_is_simulation_running(res);
        bool success = true;
            
        if(success)
        {
            reqObj->apiIsSimulationRunningGetResponse(res);
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
