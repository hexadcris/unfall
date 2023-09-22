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
#include "OAIDeleteInformationApiRequest.h"

namespace OpenAPI {

OAIDeleteInformationApiRequest::OAIDeleteInformationApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIDeleteInformationApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAIDeleteInformationApiRequest::~OAIDeleteInformationApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    qDebug() << "OAIDeleteInformationApiRequest::~OAIDeleteInformationApiRequest()";
}

QMap<QString, QString>
OAIDeleteInformationApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAIDeleteInformationApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAIDeleteInformationApiRequest::getRawSocket(){
    return socket;
}


void OAIDeleteInformationApiRequest::deleteInformationRequest(){
    qDebug() << "/api/delete-information";
    connect(this, &OAIDeleteInformationApiRequest::deleteInformation, handler.data(), &OAIDeleteInformationApiHandler::deleteInformation);

    
 
    
    QJsonDocument doc;
    socket->readJson(doc);
    QJsonObject obj = doc.object();
    OAIPathRequest oai_path_request;
    ::OpenAPI::fromJsonValue(oai_path_request, obj);
    

    emit deleteInformation(oai_path_request);
}



void OAIDeleteInformationApiRequest::deleteInformationResponse(const OAIDefault200Response& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIDeleteInformationApiRequest::deleteInformationError(const OAIDefault200Response& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIDeleteInformationApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDeleteInformationApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
