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

#ifndef OAI_OAIVerifyPathApiHandler_H
#define OAI_OAIVerifyPathApiHandler_H

#include <QObject>

#include "OAIError404.h"
#include "OAIError500.h"
#include "OAIPathRequest.h"
#include "OAIVerifyPath_200_response.h"
#include <QString>

namespace OpenAPI {

class OAIVerifyPathApiHandler : public QObject
{
    Q_OBJECT

public:
    OAIVerifyPathApiHandler();
    virtual ~OAIVerifyPathApiHandler();


public slots:
    virtual void verifyPath(OAIPathRequest oai_path_request);
    

};

}

#endif // OAI_OAIVerifyPathApiHandler_H
