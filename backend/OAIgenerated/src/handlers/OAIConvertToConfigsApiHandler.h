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

#ifndef OAI_OAIConvertToConfigsApiHandler_H
#define OAI_OAIConvertToConfigsApiHandler_H

#include <QObject>

#include "OAIConfigsRequest.h"
#include "OAIDefault200Response.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include <QString>

namespace OpenAPI {

class OAIConvertToConfigsApiHandler : public QObject
{
    Q_OBJECT

public:
    OAIConvertToConfigsApiHandler();
    virtual ~OAIConvertToConfigsApiHandler();


public slots:
    virtual void apiConvertToConfigsPost(OAIConfigsRequest oai_configs_request);
    

};

}

#endif // OAI_OAIConvertToConfigsApiHandler_H
