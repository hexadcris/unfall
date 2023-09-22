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

#ifndef OAI_RUN_OP_SIMULATION_MANAGER_API_HANDLER_OP_H
#define OAI_RUN_OP_SIMULATION_MANAGER_API_HANDLER_OP_H

#include <QObject>
#include <QString>

#include "OAIRunOpSimulationManagerApiHandler.h"

namespace OpenAPI {

class OAIRunOpSimulationManagerApiHandlerOP : public OAIRunOpSimulationManagerApiHandler
{
    Q_OBJECT

public:
    OAIRunOpSimulationManagerApiHandlerOP();
    virtual ~OAIRunOpSimulationManagerApiHandlerOP();


public slots:
    virtual void apiRunOpSimulationManagerGet();
    

};

}

#endif // OAI_RUN_OP_SIMULATION_MANAGER_API_HANDLER_OP_H

