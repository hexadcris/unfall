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

#ifndef OAI_CUSTOM_HELPERS
#define OAI_CUSTOM_HELPERS

#include <QNetworkReply>
#include <QJsonDocument>
#include <QByteArray>
#include <QString>

#include "socket.h"

template<typename T>
inline static void handleSocketResponse(T& object, QJsonObject messageObj, int code) {
    QString messageStr("");
    QString defaultError("something went wrong with the server, please contact your administrator.");
    QString successful("operation successful");
    
    code = (code>0 && code<1000)?code:999; //unknown

    QHttpEngine::Socket* socket = object->getRawSocket();
    socket->setStatusCode(code);

    if(code==200) {
        messageStr = successful;
    }else {
        messageStr = defaultError;
    }

    if(messageObj.find("message") != messageObj.end() && messageObj["message"].toString().isEmpty()){
        messageObj.insert("message", messageStr);
    }

    QJsonDocument messageDoc(messageObj);
    QByteArray messageB = messageDoc.toJson();

    //error code is set in the socket, and unused here so set to 0
    QNetworkReply::NetworkError error = QNetworkReply::NoError;
    object->sendCustomResponse(messageB, error);
}


#endif // OAI_CUSTOM_HELPERS