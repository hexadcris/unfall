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

#ifndef OAI_OAIDeleteInformationApiHandler_H
#define OAI_OAIDeleteInformationApiHandler_H

#include <QObject>

#include "OAIDefault200Response.h"
#include "OAIError404.h"
#include "OAIPathRequest.h"
#include <QString>

namespace OpenAPI {

class OAIDeleteInformationApiHandler : public QObject
{
    Q_OBJECT

public:
    OAIDeleteInformationApiHandler();
    virtual ~OAIDeleteInformationApiHandler();


public slots:
    virtual void deleteInformation(OAIPathRequest oai_path_request);
    

};

}

#endif // OAI_OAIDeleteInformationApiHandler_H
