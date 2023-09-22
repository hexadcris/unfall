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
#include "OAIExportSystemsConfigXmlApiRequest.h"

namespace OpenAPI {

OAIExportSystemsConfigXmlApiRequest::OAIExportSystemsConfigXmlApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIExportSystemsConfigXmlApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAIExportSystemsConfigXmlApiRequest::~OAIExportSystemsConfigXmlApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    qDebug() << "OAIExportSystemsConfigXmlApiRequest::~OAIExportSystemsConfigXmlApiRequest()";
}

QMap<QString, QString>
OAIExportSystemsConfigXmlApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAIExportSystemsConfigXmlApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAIExportSystemsConfigXmlApiRequest::getRawSocket(){
    return socket;
}


void OAIExportSystemsConfigXmlApiRequest::apiExportSystemsConfigXmlPostRequest(){
    qDebug() << "/api/exportSystemsConfigXml";
    connect(this, &OAIExportSystemsConfigXmlApiRequest::apiExportSystemsConfigXmlPost, handler.data(), &OAIExportSystemsConfigXmlApiHandler::apiExportSystemsConfigXmlPost);

    
 
    QJsonDocument doc;
    QList<OAISystemUI> oai_system_ui;
    if(socket->readJson(doc)){
        QJsonArray jsonArray = doc.array();
        foreach(QJsonValue obj, jsonArray) {
            OAISystemUI o;
            ::OpenAPI::fromJsonValue(o, obj);
            oai_system_ui.append(o);
        }
    }
    

    emit apiExportSystemsConfigXmlPost(oai_system_ui);
}



void OAIExportSystemsConfigXmlApiRequest::apiExportSystemsConfigXmlPostResponse(const OAIDefault200Response& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIExportSystemsConfigXmlApiRequest::apiExportSystemsConfigXmlPostError(const OAIDefault200Response& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIExportSystemsConfigXmlApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIExportSystemsConfigXmlApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
