
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

#ifndef OAI_DELETE_INFORMATION_API_HANDLER_OP_H
#define OAI_DELETE_INFORMATION_API_HANDLER_OP_H

#include <QObject>
#include <QString>

#include "OAIDeleteInformationApiHandler.h"
#include "OAIDeleteInformationApiRequest.h"

namespace OpenAPI {

class OAIDeleteInformationApiHandlerOP : public OAIDeleteInformationApiHandler
{
    Q_OBJECT

public:
    OAIDeleteInformationApiHandlerOP();
    virtual ~OAIDeleteInformationApiHandlerOP();


public slots:
    virtual void deleteInformation(OAIPathRequest oai_path_request);
    

};

}

#endif // OAI_DELETE_INFORMATION_API_HANDLER_OP_H

