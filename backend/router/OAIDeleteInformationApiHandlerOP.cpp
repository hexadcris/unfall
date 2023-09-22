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

#include "OAIDeleteInformationApiHandlerOP.h"
#include "OAIDefault200Response.h"
#include "OAIError404.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include "OAIPathRequest.h"
#include "OAICustomHelpers.h"
#include "OPGUICore.h"


namespace OpenAPI {

    OAIDeleteInformationApiHandlerOP::OAIDeleteInformationApiHandlerOP() {

    }

    OAIDeleteInformationApiHandlerOP::~OAIDeleteInformationApiHandlerOP() {

    }

    void OAIDeleteInformationApiHandlerOP::deleteInformation(OAIPathRequest oai_path_request) 
    {
        auto reqObj = qobject_cast<OAIDeleteInformationApiRequest*>(sender());
        QString errorMsg;

        if( reqObj != nullptr )
        {
            OAIDefault200Response res;

            auto result = OPGUICore::api_delete_information(oai_path_request,res,errorMsg);

            if(result)
            {
                reqObj->deleteInformationResponse(res);
            }
            else if(!errorMsg.isEmpty() && errorMsg.contains("Invalid Path, Path does not exist")){
                int code=404;
                OAIError404 error;
                error.setMessage("Server error");
                if (!errorMsg.isEmpty()) {
                    error.setMessage(error.getMessage() + " - Details: " + errorMsg);
                }
                handleSocketResponse(reqObj, error.asJsonObject(), code);
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
        else
        {
            int code=400;
            OAIError400 error;
            error.setMessage("Bad client request");
            handleSocketResponse(reqObj, error.asJsonObject(), code);
        } 
        
    }
}
