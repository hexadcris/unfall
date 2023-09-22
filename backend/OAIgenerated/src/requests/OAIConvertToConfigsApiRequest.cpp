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
#include "OAIConvertToConfigsApiRequest.h"

namespace OpenAPI {

OAIConvertToConfigsApiRequest::OAIConvertToConfigsApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIConvertToConfigsApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAIConvertToConfigsApiRequest::~OAIConvertToConfigsApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    qDebug() << "OAIConvertToConfigsApiRequest::~OAIConvertToConfigsApiRequest()";
}

QMap<QString, QString>
OAIConvertToConfigsApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAIConvertToConfigsApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAIConvertToConfigsApiRequest::getRawSocket(){
    return socket;
}


void OAIConvertToConfigsApiRequest::apiConvertToConfigsPostRequest(){
    qDebug() << "/api/convertToConfigs";
    connect(this, &OAIConvertToConfigsApiRequest::apiConvertToConfigsPost, handler.data(), &OAIConvertToConfigsApiHandler::apiConvertToConfigsPost);

    
 
    
    QJsonDocument doc;
    socket->readJson(doc);
    QJsonObject obj = doc.object();
    OAIConfigsRequest oai_configs_request;
    ::OpenAPI::fromJsonValue(oai_configs_request, obj);
    

    emit apiConvertToConfigsPost(oai_configs_request);
}



void OAIConvertToConfigsApiRequest::apiConvertToConfigsPostResponse(const OAIDefault200Response& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIConvertToConfigsApiRequest::apiConvertToConfigsPostError(const OAIDefault200Response& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIConvertToConfigsApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIConvertToConfigsApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
