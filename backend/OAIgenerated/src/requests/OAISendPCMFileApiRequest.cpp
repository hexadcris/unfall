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
#include "OAISendPCMFileApiRequest.h"

namespace OpenAPI {

OAISendPCMFileApiRequest::OAISendPCMFileApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAISendPCMFileApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAISendPCMFileApiRequest::~OAISendPCMFileApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    qDebug() << "OAISendPCMFileApiRequest::~OAISendPCMFileApiRequest()";
}

QMap<QString, QString>
OAISendPCMFileApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAISendPCMFileApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAISendPCMFileApiRequest::getRawSocket(){
    return socket;
}


void OAISendPCMFileApiRequest::apiSendPCMFilePostRequest(){
    qDebug() << "/api/sendPCMFile";
    connect(this, &OAISendPCMFileApiRequest::apiSendPCMFilePost, handler.data(), &OAISendPCMFileApiHandler::apiSendPCMFilePost);

    
 
    
    QJsonDocument doc;
    socket->readJson(doc);
    QJsonObject obj = doc.object();
    OAIPathRequest oai_path_request;
    ::OpenAPI::fromJsonValue(oai_path_request, obj);
    

    emit apiSendPCMFilePost(oai_path_request);
}



void OAISendPCMFileApiRequest::apiSendPCMFilePostResponse(const OAISelectedExperimentsRequest& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAISendPCMFileApiRequest::apiSendPCMFilePostError(const OAISelectedExperimentsRequest& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAISendPCMFileApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAISendPCMFileApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
