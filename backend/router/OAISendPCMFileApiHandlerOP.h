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

#ifndef OAI_SEND_PCM_API_HANDLER_OP_H
#define OAI_SEND_PCM_API_HANDLER_OP_H

#include <QObject>
#include <QString>

#include "OAISendPCMFileApiHandler.h"
#include "OAIDefault200Response.h"
#include "OAISelectedExperimentsRequest.h"
#include "OAIError403.h"
#include "OAIPathRequest.h"

namespace OpenAPI {

class OAISendPCMFileApiHandlerOP : public OAISendPCMFileApiHandler
{
    Q_OBJECT

public:
    OAISendPCMFileApiHandlerOP();
    virtual ~OAISendPCMFileApiHandlerOP();


public slots:
    virtual void apiSendPCMFilePost(OAIPathRequest oai_path_request);
};

}

#endif // OAI_SEND_PCM_API_HANDLER_OP_H

