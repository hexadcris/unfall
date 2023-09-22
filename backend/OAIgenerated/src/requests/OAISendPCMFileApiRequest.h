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

#ifndef OAI_OAISendPCMFileApiRequest_H
#define OAI_OAISendPCMFileApiRequest_H

#include <QObject>
#include <QStringList>
#include <QMultiMap>
#include <QNetworkReply>
#include <QSharedPointer>

#include <qhttpengine/socket.h>
#include "OAIError403.h"
#include "OAIPathRequest.h"
#include "OAISelectedExperimentsRequest.h"
#include <QString>
#include "OAISendPCMFileApiHandler.h"

namespace OpenAPI {

class OAISendPCMFileApiRequest : public QObject
{
    Q_OBJECT

public:
    OAISendPCMFileApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAISendPCMFileApiHandler> handler);
    virtual ~OAISendPCMFileApiRequest();

    void apiSendPCMFilePostRequest();
    

    void apiSendPCMFilePostResponse(const OAISelectedExperimentsRequest& res);
    

    void apiSendPCMFilePostError(const OAISelectedExperimentsRequest& res, QNetworkReply::NetworkError error_type, QString& error_str);
    

    void sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type);

    void sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type);

    QMap<QString, QString> getRequestHeaders() const;

    QHttpEngine::Socket* getRawSocket();

    void setResponseHeaders(const QMultiMap<QString,QString>& headers);

signals:
    void apiSendPCMFilePost(OAIPathRequest oai_path_request);
    

private:
    QMap<QString, QString> requestHeaders;
    QMap<QString, QString> responseHeaders;
    QHttpEngine::Socket  *socket;
    QSharedPointer<OAISendPCMFileApiHandler> handler;

    inline void setSocketResponseHeaders(){
        QHttpEngine::Socket::HeaderMap resHeaders;
        for(auto itr = responseHeaders.begin(); itr != responseHeaders.end(); ++itr) {
            resHeaders.insert(itr.key().toUtf8(), itr.value().toUtf8());
        }
        socket->setHeaders(resHeaders);
    }
};

}

#endif // OAI_OAISendPCMFileApiRequest_H
