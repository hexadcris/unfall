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

#ifndef OAI_OAIExportOpsimulationManagerXmlApiRequest_H
#define OAI_OAIExportOpsimulationManagerXmlApiRequest_H

#include <QObject>
#include <QStringList>
#include <QMultiMap>
#include <QNetworkReply>
#include <QSharedPointer>

#include <qhttpengine/socket.h>
#include "OAIDefault200Response.h"
#include "OAIOpSimulationManagerXmlRequest.h"
#include <QString>
#include "OAIExportOpsimulationManagerXmlApiHandler.h"

namespace OpenAPI {

class OAIExportOpsimulationManagerXmlApiRequest : public QObject
{
    Q_OBJECT

public:
    OAIExportOpsimulationManagerXmlApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIExportOpsimulationManagerXmlApiHandler> handler);
    virtual ~OAIExportOpsimulationManagerXmlApiRequest();

    void apiExportOpsimulationManagerXmlPostRequest();
    

    void apiExportOpsimulationManagerXmlPostResponse(const OAIDefault200Response& res);
    

    void apiExportOpsimulationManagerXmlPostError(const OAIDefault200Response& res, QNetworkReply::NetworkError error_type, QString& error_str);
    

    void sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type);

    void sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type);

    QMap<QString, QString> getRequestHeaders() const;

    QHttpEngine::Socket* getRawSocket();

    void setResponseHeaders(const QMultiMap<QString,QString>& headers);

signals:
    void apiExportOpsimulationManagerXmlPost(OAIOpSimulationManagerXmlRequest oaiop_simulation_manager_xml_request);
    

private:
    QMap<QString, QString> requestHeaders;
    QMap<QString, QString> responseHeaders;
    QHttpEngine::Socket  *socket;
    QSharedPointer<OAIExportOpsimulationManagerXmlApiHandler> handler;

    inline void setSocketResponseHeaders(){
        QHttpEngine::Socket::HeaderMap resHeaders;
        for(auto itr = responseHeaders.begin(); itr != responseHeaders.end(); ++itr) {
            resHeaders.insert(itr.key().toUtf8(), itr.value().toUtf8());
        }
        socket->setHeaders(resHeaders);
    }
};

}

#endif // OAI_OAIExportOpsimulationManagerXmlApiRequest_H
