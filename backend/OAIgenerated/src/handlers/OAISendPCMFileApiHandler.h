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

#ifndef OAI_OAISendPCMFileApiHandler_H
#define OAI_OAISendPCMFileApiHandler_H

#include <QObject>

#include "OAIError403.h"
#include "OAIPathRequest.h"
#include "OAISelectedExperimentsRequest.h"
#include <QString>

namespace OpenAPI {

class OAISendPCMFileApiHandler : public QObject
{
    Q_OBJECT

public:
    OAISendPCMFileApiHandler();
    virtual ~OAISendPCMFileApiHandler();


public slots:
    virtual void apiSendPCMFilePost(OAIPathRequest oai_path_request);
    

};

}

#endif // OAI_OAISendPCMFileApiHandler_H
