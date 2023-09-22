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

#include "OAIVerifyPathApiHandlerOP.h"
#include "OAIVerifyPathApiRequest.h"
#include "OAIVerifyPath_200_response.h"
#include "OAIError404.h"
#include "OAIError500.h"
#include "OAICustomHelpers.h"
#include <OPGUICore.h>

namespace OpenAPI {

    OAIVerifyPathApiHandlerOP::OAIVerifyPathApiHandlerOP() {

    }

    OAIVerifyPathApiHandlerOP::~OAIVerifyPathApiHandlerOP() {

    }

    void OAIVerifyPathApiHandlerOP::verifyPath(OAIPathRequest oai_path_request) 
    {
        auto reqObj = qobject_cast<OAIVerifyPathApiRequest*>(sender());
        bool succeed =false;
        int code = 0;

        OAIVerifyPath_200_response res;

        succeed = OPGUICore::api_verify_path(oai_path_request,res);
        
        if(succeed){
            reqObj->verifyPathResponse(res);
            return;
        }
        else if(!succeed){
            code=500;
            OAIError500 error;
            error.setMessage("Server error");
            error.setCode(code);
            handleSocketResponse(reqObj, error.asJsonObject(), code);
        }
        
    }

}
