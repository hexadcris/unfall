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

#include "OAIApiRouterOP.h"


    OAIApiRouterOP::OAIApiRouterOP()
    {
        OAIApiRouterOP::createApiHandlers();
    }

    OAIApiRouterOP::~OAIApiRouterOP()
    {
        // Destructor implementation
    }

    void OAIApiRouterOP::createApiHandlers() {
        QSharedPointer<OpenAPI::OAIConvertToConfigsApiHandlerOP> mOAIConvertToConfigsApiHandlerOP = QSharedPointer<OpenAPI::OAIConvertToConfigsApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAIDeleteInformationApiHandlerOP> mOAIDeleteInformationApiHandlerOP = QSharedPointer<OpenAPI::OAIDeleteInformationApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAIExportOpsimulationManagerXmlApiHandlerOP> mOAIExportOpsimulationManagerXmlApiHandlerOP = QSharedPointer<OpenAPI::OAIExportOpsimulationManagerXmlApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAIExportToSimulationApiHandlerOP> mOAIExportToSimulationApiHandlerOP = QSharedPointer<OpenAPI::OAIExportToSimulationApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAIPathToConvertedCasesApiHandlerOP> mOAIPathToConvertedCasesApiHandlerOP = QSharedPointer<OpenAPI::OAIPathToConvertedCasesApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAIRunOpSimulationManagerApiHandlerOP> mOAIRunOpSimulationManagerApiHandlerOP = QSharedPointer<OpenAPI::OAIRunOpSimulationManagerApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAISendPCMFileApiHandlerOP> mOAISendPCMFileApiHandlerOP = QSharedPointer<OpenAPI::OAISendPCMFileApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAIVerifyPathApiHandlerOP> mOAIVerifyPathApiHandlerOP = QSharedPointer<OpenAPI::OAIVerifyPathApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAIComponentsApiHandlerOP> mOAIComponentsApiHandlerOP = QSharedPointer<OpenAPI::OAIComponentsApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAIExportSystemsConfigXmlApiHandlerOP> mOAIExportSystemsConfigXmlApiHandlerOP = QSharedPointer<OpenAPI::OAIExportSystemsConfigXmlApiHandlerOP>::create();
        QSharedPointer<OpenAPI::OAISystemsApiHandlerOP> mOAISystemsApiHandlerOP = QSharedPointer<OpenAPI::OAISystemsApiHandlerOP>::create();
        

        setOAIConvertToConfigsApiHandler(mOAIConvertToConfigsApiHandlerOP);
        setOAIDeleteInformationApiHandler(mOAIDeleteInformationApiHandlerOP);
        setOAIExportOpsimulationManagerXmlApiHandler(mOAIExportOpsimulationManagerXmlApiHandlerOP);
        setOAIExportToSimulationApiHandler(mOAIExportToSimulationApiHandlerOP);
        setOAIPathToConvertedCasesApiHandler(mOAIPathToConvertedCasesApiHandlerOP);
        setOAIRunOpSimulationManagerApiHandler(mOAIRunOpSimulationManagerApiHandlerOP);
        setOAISendPCMFileApiHandler(mOAISendPCMFileApiHandlerOP);
        setOAIVerifyPathApiHandler(mOAIVerifyPathApiHandlerOP);
        setOAIComponentsApiHandler(mOAIComponentsApiHandlerOP);
        setOAIExportSystemsConfigXmlApiHandler(mOAIExportSystemsConfigXmlApiHandlerOP);
        setOAISystemsApiHandler(mOAISystemsApiHandlerOP);
    }
