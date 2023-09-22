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

#include "OAIPathToConvertedCasesApiHandler.h"
#include "OAIPathToConvertedCasesApiRequest.h"

namespace OpenAPI {

OAIPathToConvertedCasesApiHandler::OAIPathToConvertedCasesApiHandler(){

}

OAIPathToConvertedCasesApiHandler::~OAIPathToConvertedCasesApiHandler(){

}

void OAIPathToConvertedCasesApiHandler::apiPathToConvertedCasesPost(OAIPathRequest oai_path_request) {
    Q_UNUSED(oai_path_request);
    auto reqObj = qobject_cast<OAIPathToConvertedCasesApiRequest*>(sender());
    if( reqObj != nullptr )
    {
        OAIDefault200Response res;
        reqObj->apiPathToConvertedCasesPostResponse(res);
    }
}


}
