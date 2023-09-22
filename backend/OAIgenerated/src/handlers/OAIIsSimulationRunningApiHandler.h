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

#ifndef OAI_OAIIsSimulationRunningApiHandler_H
#define OAI_OAIIsSimulationRunningApiHandler_H

#include <QObject>

#include "OAIBoolean200Response.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include <QString>

namespace OpenAPI {

class OAIIsSimulationRunningApiHandler : public QObject
{
    Q_OBJECT

public:
    OAIIsSimulationRunningApiHandler();
    virtual ~OAIIsSimulationRunningApiHandler();


public slots:
    virtual void apiIsSimulationRunningGet();
    

};

}

#endif // OAI_OAIIsSimulationRunningApiHandler_H
