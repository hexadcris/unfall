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

#ifndef OAI_OAIExportSystemsConfigXmlApiHandler_OP_H
#define OAI_OAIExportSystemsConfigXmlApiHandler_OP_H

#include <QObject>
#include <QList>
#include <QString>

#include "OAIExportSystemsConfigXmlApiHandler.h"
#include "OAIDefault200Response.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include "OAISystemUI.h"


namespace OpenAPI {

class OAIExportSystemsConfigXmlApiHandlerOP : public OAIExportSystemsConfigXmlApiHandler
{
    Q_OBJECT

public:
    OAIExportSystemsConfigXmlApiHandlerOP();
    virtual ~OAIExportSystemsConfigXmlApiHandlerOP();


public slots:
    virtual void apiExportSystemsConfigXmlPost(QList<OAISystemUI> oai_system_ui);
    

};

}

#endif // OAI_OAIExportSystemsConfigXmlApiHandler_OP_H
