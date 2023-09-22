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

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>


#include "OAIApiRouter.h"
#include "OAIComponentsApiRequest.h"
#include "OAIConvertToConfigsApiRequest.h"
#include "OAIDeleteInformationApiRequest.h"
#include "OAIExportOpsimulationManagerXmlApiRequest.h"
#include "OAIExportSystemsConfigXmlApiRequest.h"
#include "OAIExportToSimulationApiRequest.h"
#include "OAIIsSimulationRunningApiRequest.h"
#include "OAIPathToConvertedCasesApiRequest.h"
#include "OAIRunOpSimulationManagerApiRequest.h"
#include "OAISendPCMFileApiRequest.h"
#include "OAISystemsApiRequest.h"
#include "OAIVerifyPathApiRequest.h"


namespace OpenAPI {

OAIApiRouter::OAIApiRouter() {
    createApiHandlers();
}

OAIApiRouter::~OAIApiRouter(){

}

void OAIApiRouter::createApiHandlers() { 
    mOAIComponentsApiHandler = QSharedPointer<OAIComponentsApiHandler>::create();
    mOAIConvertToConfigsApiHandler = QSharedPointer<OAIConvertToConfigsApiHandler>::create();
    mOAIDeleteInformationApiHandler = QSharedPointer<OAIDeleteInformationApiHandler>::create();
    mOAIExportOpsimulationManagerXmlApiHandler = QSharedPointer<OAIExportOpsimulationManagerXmlApiHandler>::create();
    mOAIExportSystemsConfigXmlApiHandler = QSharedPointer<OAIExportSystemsConfigXmlApiHandler>::create();
    mOAIExportToSimulationApiHandler = QSharedPointer<OAIExportToSimulationApiHandler>::create();
    mOAIIsSimulationRunningApiHandler = QSharedPointer<OAIIsSimulationRunningApiHandler>::create();
    mOAIPathToConvertedCasesApiHandler = QSharedPointer<OAIPathToConvertedCasesApiHandler>::create();
    mOAIRunOpSimulationManagerApiHandler = QSharedPointer<OAIRunOpSimulationManagerApiHandler>::create();
    mOAISendPCMFileApiHandler = QSharedPointer<OAISendPCMFileApiHandler>::create();
    mOAISystemsApiHandler = QSharedPointer<OAISystemsApiHandler>::create();
    mOAIVerifyPathApiHandler = QSharedPointer<OAIVerifyPathApiHandler>::create();
}


void OAIApiRouter::setOAIComponentsApiHandler(QSharedPointer<OAIComponentsApiHandler> handler){
    mOAIComponentsApiHandler = handler;
}
void OAIApiRouter::setOAIConvertToConfigsApiHandler(QSharedPointer<OAIConvertToConfigsApiHandler> handler){
    mOAIConvertToConfigsApiHandler = handler;
}
void OAIApiRouter::setOAIDeleteInformationApiHandler(QSharedPointer<OAIDeleteInformationApiHandler> handler){
    mOAIDeleteInformationApiHandler = handler;
}
void OAIApiRouter::setOAIExportOpsimulationManagerXmlApiHandler(QSharedPointer<OAIExportOpsimulationManagerXmlApiHandler> handler){
    mOAIExportOpsimulationManagerXmlApiHandler = handler;
}
void OAIApiRouter::setOAIExportSystemsConfigXmlApiHandler(QSharedPointer<OAIExportSystemsConfigXmlApiHandler> handler){
    mOAIExportSystemsConfigXmlApiHandler = handler;
}
void OAIApiRouter::setOAIExportToSimulationApiHandler(QSharedPointer<OAIExportToSimulationApiHandler> handler){
    mOAIExportToSimulationApiHandler = handler;
}
void OAIApiRouter::setOAIIsSimulationRunningApiHandler(QSharedPointer<OAIIsSimulationRunningApiHandler> handler){
    mOAIIsSimulationRunningApiHandler = handler;
}
void OAIApiRouter::setOAIPathToConvertedCasesApiHandler(QSharedPointer<OAIPathToConvertedCasesApiHandler> handler){
    mOAIPathToConvertedCasesApiHandler = handler;
}
void OAIApiRouter::setOAIRunOpSimulationManagerApiHandler(QSharedPointer<OAIRunOpSimulationManagerApiHandler> handler){
    mOAIRunOpSimulationManagerApiHandler = handler;
}
void OAIApiRouter::setOAISendPCMFileApiHandler(QSharedPointer<OAISendPCMFileApiHandler> handler){
    mOAISendPCMFileApiHandler = handler;
}
void OAIApiRouter::setOAISystemsApiHandler(QSharedPointer<OAISystemsApiHandler> handler){
    mOAISystemsApiHandler = handler;
}
void OAIApiRouter::setOAIVerifyPathApiHandler(QSharedPointer<OAIVerifyPathApiHandler> handler){
    mOAIVerifyPathApiHandler = handler;
}

void OAIApiRouter::setUpRoutes() {
    
    Routes.insert(QString("%1 %2").arg("GET").arg("/api/components").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIComponentsApiRequest(socket, mOAIComponentsApiHandler);
            reqObj->apiComponentsGetRequest();
    });
    Routes.insert(QString("%1 %2").arg("POST").arg("/api/convertToConfigs").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIConvertToConfigsApiRequest(socket, mOAIConvertToConfigsApiHandler);
            reqObj->apiConvertToConfigsPostRequest();
    });
    Routes.insert(QString("%1 %2").arg("POST").arg("/api/delete-information").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIDeleteInformationApiRequest(socket, mOAIDeleteInformationApiHandler);
            reqObj->deleteInformationRequest();
    });
    Routes.insert(QString("%1 %2").arg("POST").arg("/api/exportOpsimulationManagerXml").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIExportOpsimulationManagerXmlApiRequest(socket, mOAIExportOpsimulationManagerXmlApiHandler);
            reqObj->apiExportOpsimulationManagerXmlPostRequest();
    });
    Routes.insert(QString("%1 %2").arg("POST").arg("/api/exportSystemsConfigXml").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIExportSystemsConfigXmlApiRequest(socket, mOAIExportSystemsConfigXmlApiHandler);
            reqObj->apiExportSystemsConfigXmlPostRequest();
    });
    Routes.insert(QString("%1 %2").arg("POST").arg("/api/exportToSimulations").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIExportToSimulationApiRequest(socket, mOAIExportToSimulationApiHandler);
            reqObj->apiExportToSimulationsPostRequest();
    });
    Routes.insert(QString("%1 %2").arg("GET").arg("/api/isSimulationRunning").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIIsSimulationRunningApiRequest(socket, mOAIIsSimulationRunningApiHandler);
            reqObj->apiIsSimulationRunningGetRequest();
    });
    Routes.insert(QString("%1 %2").arg("POST").arg("/api/pathToConvertedCases").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIPathToConvertedCasesApiRequest(socket, mOAIPathToConvertedCasesApiHandler);
            reqObj->apiPathToConvertedCasesPostRequest();
    });
    Routes.insert(QString("%1 %2").arg("GET").arg("/api/runOpSimulationManager").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIRunOpSimulationManagerApiRequest(socket, mOAIRunOpSimulationManagerApiHandler);
            reqObj->apiRunOpSimulationManagerGetRequest();
    });
    Routes.insert(QString("%1 %2").arg("POST").arg("/api/sendPCMFile").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAISendPCMFileApiRequest(socket, mOAISendPCMFileApiHandler);
            reqObj->apiSendPCMFilePostRequest();
    });
    Routes.insert(QString("%1 %2").arg("GET").arg("/api/systems").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAISystemsApiRequest(socket, mOAISystemsApiHandler);
            reqObj->apiSystemsGetRequest();
    });
    Routes.insert(QString("%1 %2").arg("POST").arg("/api/verify-path").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIVerifyPathApiRequest(socket, mOAIVerifyPathApiHandler);
            reqObj->verifyPathRequest();
    });
}

void OAIApiRouter::processRequest(QHttpEngine::Socket *socket){
    if( handleRequest(socket) ){
        return;
    }
    if( handleRequestAndExtractPathParam(socket) ){
        return;
    }
    socket->setStatusCode(QHttpEngine::Socket::NotFound);
    if(socket->isOpen()){
        socket->writeHeaders();
        socket->close();
    }
}

bool OAIApiRouter::handleRequest(QHttpEngine::Socket *socket){
    auto reqPath = QString("%1 %2").arg(fromQHttpEngineMethod(socket->method())).arg(socket->path()).toLower();
    if ( Routes.contains(reqPath) ) {
        Routes.value(reqPath).operator()(socket);
        return true;
    }
    return false;
}

bool OAIApiRouter::handleRequestAndExtractPathParam(QHttpEngine::Socket *socket){
    auto reqPath = QString("%1 %2").arg(fromQHttpEngineMethod(socket->method())).arg(socket->path()).toLower();
    return false;
}

}
