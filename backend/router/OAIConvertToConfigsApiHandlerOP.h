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

#ifndef OAI_CONVERT_TO_CONFIGS_API_HANDLER_OP_H
#define OAI_CONVERT_TO_CONFIGS_API_HANDLER_OP_H

#include <QObject>
#include <QString>

#include "OAIConvertToConfigsApiHandler.h"
#include "OAIDefault200Response.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include "OAISelectedExperimentsRequest.h"

namespace OpenAPI {

class OAIConvertToConfigsApiHandlerOP : public OAIConvertToConfigsApiHandler
{
    Q_OBJECT

    public:
        OAIConvertToConfigsApiHandlerOP();
        virtual ~OAIConvertToConfigsApiHandlerOP();


    public slots:
        virtual void apiConvertToConfigsPost(OAIConfigsRequest oai_configs_request);
};

}

#endif // OAI_CONVERT_TO_CONFIGS_API_HANDLER_OP_H

