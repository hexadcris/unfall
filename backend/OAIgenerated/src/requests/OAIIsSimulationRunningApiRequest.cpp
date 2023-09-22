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
#include "OAIIsSimulationRunningApiRequest.h"

namespace OpenAPI {

OAIIsSimulationRunningApiRequest::OAIIsSimulationRunningApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIIsSimulationRunningApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAIIsSimulationRunningApiRequest::~OAIIsSimulationRunningApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    qDebug() << "OAIIsSimulationRunningApiRequest::~OAIIsSimulationRunningApiRequest()";
}

QMap<QString, QString>
OAIIsSimulationRunningApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAIIsSimulationRunningApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAIIsSimulationRunningApiRequest::getRawSocket(){
    return socket;
}


void OAIIsSimulationRunningApiRequest::apiIsSimulationRunningGetRequest(){
    qDebug() << "/api/isSimulationRunning";
    connect(this, &OAIIsSimulationRunningApiRequest::apiIsSimulationRunningGet, handler.data(), &OAIIsSimulationRunningApiHandler::apiIsSimulationRunningGet);

    


    emit apiIsSimulationRunningGet();
}



void OAIIsSimulationRunningApiRequest::apiIsSimulationRunningGetResponse(const OAIBoolean200Response& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIIsSimulationRunningApiRequest::apiIsSimulationRunningGetError(const OAIBoolean200Response& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIIsSimulationRunningApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIIsSimulationRunningApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
