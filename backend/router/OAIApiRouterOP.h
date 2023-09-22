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

#ifndef OAI_APIROUTER_OP_H
#define OAI_APIROUTER_OP_H

#include <QSharedPointer>

#include "OAIApiRouter.h"
#include "OAIConvertToConfigsApiHandlerOP.h"
#include "OAIDeleteInformationApiHandlerOP.h"
#include "OAIExportOpsimulationManagerXmlApiHandlerOP.h"
#include "OAIExportToSimulationApiHandlerOP.h"
#include "OAIPathToConvertedCasesApiHandlerOP.h"
#include "OAIRunOpSimulationManagerApiHandlerOP.h"
#include "OAISendPCMFileApiHandlerOP.h"
#include "OAIVerifyPathApiHandlerOP.h"
#include "OAIComponentsApiHandlerOP.h"
#include "OAIExportSystemsConfigXmlApiHandlerOP.h"
#include "OAISystemsApiHandlerOP.h"


class OAIApiRouterOP : public OpenAPI::OAIApiRouter
{
public:
    OAIApiRouterOP();
    ~OAIApiRouterOP();
protected:
    void createApiHandlers() override;
};

#endif // OAI_APIROUTER_OP_H