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

#ifndef OAI_OAIVerifyPathApiRequest_H
#define OAI_OAIVerifyPathApiRequest_H

#include <QObject>
#include <QStringList>
#include <QMultiMap>
#include <QNetworkReply>
#include <QSharedPointer>

#include <qhttpengine/socket.h>
#include "OAIError404.h"
#include "OAIError500.h"
#include "OAIPathRequest.h"
#include "OAIVerifyPath_200_response.h"
#include <QString>
#include "OAIVerifyPathApiHandler.h"

namespace OpenAPI {

class OAIVerifyPathApiRequest : public QObject
{
    Q_OBJECT

public:
    OAIVerifyPathApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIVerifyPathApiHandler> handler);
    virtual ~OAIVerifyPathApiRequest();

    void verifyPathRequest();
    

    void verifyPathResponse(const OAIVerifyPath_200_response& res);
    

    void verifyPathError(const OAIVerifyPath_200_response& res, QNetworkReply::NetworkError error_type, QString& error_str);
    

    void sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type);

    void sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type);

    QMap<QString, QString> getRequestHeaders() const;

    QHttpEngine::Socket* getRawSocket();

    void setResponseHeaders(const QMultiMap<QString,QString>& headers);

signals:
    void verifyPath(OAIPathRequest oai_path_request);
    

private:
    QMap<QString, QString> requestHeaders;
    QMap<QString, QString> responseHeaders;
    QHttpEngine::Socket  *socket;
    QSharedPointer<OAIVerifyPathApiHandler> handler;

    inline void setSocketResponseHeaders(){
        QHttpEngine::Socket::HeaderMap resHeaders;
        for(auto itr = responseHeaders.begin(); itr != responseHeaders.end(); ++itr) {
            resHeaders.insert(itr.key().toUtf8(), itr.value().toUtf8());
        }
        socket->setHeaders(resHeaders);
    }
};

}

#endif // OAI_OAIVerifyPathApiRequest_H
