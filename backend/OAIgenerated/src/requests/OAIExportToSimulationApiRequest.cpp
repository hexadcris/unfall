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
#include "OAIExportToSimulationApiRequest.h"

namespace OpenAPI {

OAIExportToSimulationApiRequest::OAIExportToSimulationApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIExportToSimulationApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAIExportToSimulationApiRequest::~OAIExportToSimulationApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    qDebug() << "OAIExportToSimulationApiRequest::~OAIExportToSimulationApiRequest()";
}

QMap<QString, QString>
OAIExportToSimulationApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAIExportToSimulationApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAIExportToSimulationApiRequest::getRawSocket(){
    return socket;
}


void OAIExportToSimulationApiRequest::apiExportToSimulationsPostRequest(){
    qDebug() << "/api/exportToSimulations";
    connect(this, &OAIExportToSimulationApiRequest::apiExportToSimulationsPost, handler.data(), &OAIExportToSimulationApiHandler::apiExportToSimulationsPost);

    
 
    
    QJsonDocument doc;
    socket->readJson(doc);
    QJsonObject obj = doc.object();
    OAISelectedExperimentsRequest oai_selected_experiments_request;
    ::OpenAPI::fromJsonValue(oai_selected_experiments_request, obj);
    

    emit apiExportToSimulationsPost(oai_selected_experiments_request);
}



void OAIExportToSimulationApiRequest::apiExportToSimulationsPostResponse(const OAIDefault200Response& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIExportToSimulationApiRequest::apiExportToSimulationsPostError(const OAIDefault200Response& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIExportToSimulationApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIExportToSimulationApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
