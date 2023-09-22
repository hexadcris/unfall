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

#ifndef OAI_SystemsApiHandler_OP_H
#define OAI_SystemsApiHandler_OP_H

#include <QObject>
#include <QString>

#include "OAIError400.h"
#include "OAIError500.h"
#include "OAIPathRequest.h"
#include "OAISystemUI.h"
#include "OAISystemsApiHandler.h"
#include "OPGUICore.h"


namespace OpenAPI {

class OAISystemsApiHandlerOP : public OAISystemsApiHandler
{
    Q_OBJECT

public:
    OAISystemsApiHandlerOP();
    virtual ~OAISystemsApiHandlerOP();


public slots:
    virtual void apiSystemsGet();
    

};

}

#endif // OAI_SystemsApiHandler_OP_H
