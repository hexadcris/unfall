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

#ifndef OAI_PATH_TO_CONVERTED_CASES_API_HANDLER_OP_H
#define OAI_PATH_TO_CONVERTED_CASES_API_HANDLER_OP_H

#include <QObject>
#include <QString>

#include "OAIPathToConvertedCasesApiHandler.h"
#include "OAIDefault200Response.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include "OAIPathRequest.h"

namespace OpenAPI {

class OAIPathToConvertedCasesApiHandlerOP : public OAIPathToConvertedCasesApiHandler
{
    Q_OBJECT

public:
    OAIPathToConvertedCasesApiHandlerOP();
    virtual ~OAIPathToConvertedCasesApiHandlerOP();


public slots:
    virtual void apiPathToConvertedCasesPost(OAIPathRequest oai_path_request);
    

};

}

#endif // OAI_PATH_TO_CONVERTED_CASES_API_HANDLER_OP_H

