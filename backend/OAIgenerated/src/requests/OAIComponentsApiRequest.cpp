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
#include "OAIComponentsApiRequest.h"

namespace OpenAPI {

OAIComponentsApiRequest::OAIComponentsApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIComponentsApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAIComponentsApiRequest::~OAIComponentsApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    qDebug() << "OAIComponentsApiRequest::~OAIComponentsApiRequest()";
}

QMap<QString, QString>
OAIComponentsApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAIComponentsApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAIComponentsApiRequest::getRawSocket(){
    return socket;
}


void OAIComponentsApiRequest::apiComponentsGetRequest(){
    qDebug() << "/api/components";
    connect(this, &OAIComponentsApiRequest::apiComponentsGet, handler.data(), &OAIComponentsApiHandler::apiComponentsGet);

    


    emit apiComponentsGet();
}



void OAIComponentsApiRequest::apiComponentsGetResponse(const QList<OAIComponentUI>& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIComponentsApiRequest::apiComponentsGetError(const QList<OAIComponentUI>& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIComponentsApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIComponentsApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
