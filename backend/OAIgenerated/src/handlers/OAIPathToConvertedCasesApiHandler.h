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

#ifndef OAI_OAIPathToConvertedCasesApiHandler_H
#define OAI_OAIPathToConvertedCasesApiHandler_H

#include <QObject>

#include "OAIDefault200Response.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include "OAIPathRequest.h"
#include <QString>

namespace OpenAPI {

class OAIPathToConvertedCasesApiHandler : public QObject
{
    Q_OBJECT

public:
    OAIPathToConvertedCasesApiHandler();
    virtual ~OAIPathToConvertedCasesApiHandler();


public slots:
    virtual void apiPathToConvertedCasesPost(OAIPathRequest oai_path_request);
    

};

}

#endif // OAI_OAIPathToConvertedCasesApiHandler_H
