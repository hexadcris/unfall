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

#ifndef OAI_OAIExportOpsimulationManagerXmlApiHandler_H
#define OAI_OAIExportOpsimulationManagerXmlApiHandler_H

#include <QObject>

#include "OAIDefault200Response.h"
#include "OAIOpSimulationManagerXmlRequest.h"
#include <QString>

namespace OpenAPI {

class OAIExportOpsimulationManagerXmlApiHandler : public QObject
{
    Q_OBJECT

public:
    OAIExportOpsimulationManagerXmlApiHandler();
    virtual ~OAIExportOpsimulationManagerXmlApiHandler();


public slots:
    virtual void apiExportOpsimulationManagerXmlPost(OAIOpSimulationManagerXmlRequest oaiop_simulation_manager_xml_request);
    

};

}

#endif // OAI_OAIExportOpsimulationManagerXmlApiHandler_H
