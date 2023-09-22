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
#include <QDebug>

#include "OAIHelpers.h"
#include "OAIExportOpsimulationManagerXmlApiRequest.h"

namespace OpenAPI {

OAIExportOpsimulationManagerXmlApiRequest::OAIExportOpsimulationManagerXmlApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIExportOpsimulationManagerXmlApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAIExportOpsimulationManagerXmlApiRequest::~OAIExportOpsimulationManagerXmlApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    qDebug() << "OAIExportOpsimulationManagerXmlApiRequest::~OAIExportOpsimulationManagerXmlApiRequest()";
}

QMap<QString, QString>
OAIExportOpsimulationManagerXmlApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAIExportOpsimulationManagerXmlApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAIExportOpsimulationManagerXmlApiRequest::getRawSocket(){
    return socket;
}


void OAIExportOpsimulationManagerXmlApiRequest::apiExportOpsimulationManagerXmlPostRequest(){
    qDebug() << "/api/exportOpsimulationManagerXml";
    connect(this, &OAIExportOpsimulationManagerXmlApiRequest::apiExportOpsimulationManagerXmlPost, handler.data(), &OAIExportOpsimulationManagerXmlApiHandler::apiExportOpsimulationManagerXmlPost);

    
 
    
    QJsonDocument doc;
    socket->readJson(doc);
    QJsonObject obj = doc.object();
    OAIOpSimulationManagerXmlRequest oaiop_simulation_manager_xml_request;
    ::OpenAPI::fromJsonValue(oaiop_simulation_manager_xml_request, obj);
    

    emit apiExportOpsimulationManagerXmlPost(oaiop_simulation_manager_xml_request);
}



void OAIExportOpsimulationManagerXmlApiRequest::apiExportOpsimulationManagerXmlPostResponse(const OAIDefault200Response& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIExportOpsimulationManagerXmlApiRequest::apiExportOpsimulationManagerXmlPostError(const OAIDefault200Response& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIExportOpsimulationManagerXmlApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIExportOpsimulationManagerXmlApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
