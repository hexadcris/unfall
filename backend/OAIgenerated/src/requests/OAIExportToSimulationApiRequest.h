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

#ifndef OAI_OAIExportToSimulationApiRequest_H
#define OAI_OAIExportToSimulationApiRequest_H

#include <QObject>
#include <QStringList>
#include <QMultiMap>
#include <QNetworkReply>
#include <QSharedPointer>

#include <qhttpengine/socket.h>
#include "OAIDefault200Response.h"
#include "OAIError400.h"
#include "OAIError500.h"
#include "OAISelectedExperimentsRequest.h"
#include <QString>
#include "OAIExportToSimulationApiHandler.h"

namespace OpenAPI {

class OAIExportToSimulationApiRequest : public QObject
{
    Q_OBJECT

public:
    OAIExportToSimulationApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIExportToSimulationApiHandler> handler);
    virtual ~OAIExportToSimulationApiRequest();

    void apiExportToSimulationsPostRequest();
    

    void apiExportToSimulationsPostResponse(const OAIDefault200Response& res);
    

    void apiExportToSimulationsPostError(const OAIDefault200Response& res, QNetworkReply::NetworkError error_type, QString& error_str);
    

    void sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type);

    void sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type);

    QMap<QString, QString> getRequestHeaders() const;

    QHttpEngine::Socket* getRawSocket();

    void setResponseHeaders(const QMultiMap<QString,QString>& headers);

signals:
    void apiExportToSimulationsPost(OAISelectedExperimentsRequest oai_selected_experiments_request);
    

private:
    QMap<QString, QString> requestHeaders;
    QMap<QString, QString> responseHeaders;
    QHttpEngine::Socket  *socket;
    QSharedPointer<OAIExportToSimulationApiHandler> handler;

    inline void setSocketResponseHeaders(){
        QHttpEngine::Socket::HeaderMap resHeaders;
        for(auto itr = responseHeaders.begin(); itr != responseHeaders.end(); ++itr) {
            resHeaders.insert(itr.key().toUtf8(), itr.value().toUtf8());
        }
        socket->setHeaders(resHeaders);
    }
};

}

#endif // OAI_OAIExportToSimulationApiRequest_H
