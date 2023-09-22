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

#ifndef OAI_COMPONENTS_API_HANDLER_OP_H
#define OAI_COMPONENTS_API_HANDLER_OP_H

#include <QObject>

#include "OAIComponentsApiHandler.h"

namespace OpenAPI {

class OAIComponentsApiHandlerOP : public OAIComponentsApiHandler
{
    Q_OBJECT

public:
    OAIComponentsApiHandlerOP();
    virtual ~OAIComponentsApiHandlerOP();


public slots:
    virtual void apiComponentsGet();
    

};

}

#endif // OAI_COMPONENTS_API_HANDLER_OP_H

