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

#ifndef OAI_EXPORT_OP_SIMULATION_MANAGER_XML_API_HANDLER_OP_H
#define OAI_EXPORT_OP_SIMULATION_MANAGER_XML_API_HANDLER_OP_H

#include <QObject>
#include <QString>

#include "OAIExportOpsimulationManagerXmlApiHandler.h"
#include "OAIDefault200Response.h"
#include "OAIOpSimulationManagerXmlRequest.h"

namespace OpenAPI {

class OAIExportOpsimulationManagerXmlApiHandlerOP : public OAIExportOpsimulationManagerXmlApiHandler
{
    Q_OBJECT

public:
    OAIExportOpsimulationManagerXmlApiHandlerOP();
    virtual ~OAIExportOpsimulationManagerXmlApiHandlerOP();


public slots:
    //virtual void exportOpsimulationManagerXmlPost(OAIOpSimulationManagerXmlRequest oaiop_simulation_manager_xml_request);
    virtual void apiExportOpsimulationManagerXmlPost(OAIOpSimulationManagerXmlRequest oaiop_simulation_manager_xml_request);
    

};

}

#endif // OAI_EXPORT_OP_SIMULATION_MANAGER_XML_API_HANDLER_OP_H

