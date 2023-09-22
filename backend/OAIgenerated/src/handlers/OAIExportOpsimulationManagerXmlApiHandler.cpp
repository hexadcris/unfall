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

#include "OAIExportOpsimulationManagerXmlApiHandler.h"
#include "OAIExportOpsimulationManagerXmlApiRequest.h"

namespace OpenAPI {

OAIExportOpsimulationManagerXmlApiHandler::OAIExportOpsimulationManagerXmlApiHandler(){

}

OAIExportOpsimulationManagerXmlApiHandler::~OAIExportOpsimulationManagerXmlApiHandler(){

}

void OAIExportOpsimulationManagerXmlApiHandler::apiExportOpsimulationManagerXmlPost(OAIOpSimulationManagerXmlRequest oaiop_simulation_manager_xml_request) {
    Q_UNUSED(oaiop_simulation_manager_xml_request);
    auto reqObj = qobject_cast<OAIExportOpsimulationManagerXmlApiRequest*>(sender());
    if( reqObj != nullptr )
    {
        OAIDefault200Response res;
        reqObj->apiExportOpsimulationManagerXmlPostResponse(res);
    }
}


}
