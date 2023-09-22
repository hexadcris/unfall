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

#include "OAIExportToSimulationApiHandler.h"
#include "OAIExportToSimulationApiRequest.h"

namespace OpenAPI {

OAIExportToSimulationApiHandler::OAIExportToSimulationApiHandler(){

}

OAIExportToSimulationApiHandler::~OAIExportToSimulationApiHandler(){

}

void OAIExportToSimulationApiHandler::apiExportToSimulationsPost(OAISelectedExperimentsRequest oai_selected_experiments_request) {
    Q_UNUSED(oai_selected_experiments_request);
    auto reqObj = qobject_cast<OAIExportToSimulationApiRequest*>(sender());
    if( reqObj != nullptr )
    {
        OAIDefault200Response res;
        reqObj->apiExportToSimulationsPostResponse(res);
    }
}


}
