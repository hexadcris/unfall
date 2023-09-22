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

#ifndef OAI_OAIExportToSimulationApiHandler_H
#define OAI_OAIExportToSimulationApiHandler_H

#include <QObject>

#include "OAIDefault200Response.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include "OAISelectedExperimentsRequest.h"
#include <QString>

namespace OpenAPI {

class OAIExportToSimulationApiHandler : public QObject
{
    Q_OBJECT

public:
    OAIExportToSimulationApiHandler();
    virtual ~OAIExportToSimulationApiHandler();


public slots:
    virtual void apiExportToSimulationsPost(OAISelectedExperimentsRequest oai_selected_experiments_request);
    

};

}

#endif // OAI_OAIExportToSimulationApiHandler_H
