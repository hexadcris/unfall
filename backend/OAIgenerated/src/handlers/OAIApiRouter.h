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

#ifndef OAI_APIROUTER_H
#define OAI_APIROUTER_H

#include <functional>
#include <QObject>
#include <QStringList>
#include <QSharedPointer>
#include <QList>
#include <QMultiMap>
#include <QRegularExpression>

#include <qhttpengine/socket.h>
#include <qhttpengine/handler.h>
#include <qhttpengine/qobjecthandler.h>

#include "OAIComponentsApiHandler.h"
#include "OAIConvertToConfigsApiHandler.h"
#include "OAIDeleteInformationApiHandler.h"
#include "OAIExportOpsimulationManagerXmlApiHandler.h"
#include "OAIExportSystemsConfigXmlApiHandler.h"
#include "OAIExportToSimulationApiHandler.h"
#include "OAIIsSimulationRunningApiHandler.h"
#include "OAIPathToConvertedCasesApiHandler.h"
#include "OAIRunOpSimulationManagerApiHandler.h"
#include "OAISendPCMFileApiHandler.h"
#include "OAISystemsApiHandler.h"
#include "OAIVerifyPathApiHandler.h"


namespace OpenAPI {

class OAIApiRequestHandler : public  QHttpEngine::QObjectHandler
{
    Q_OBJECT
signals:
    void requestReceived(QHttpEngine::Socket *socket);

protected:
    virtual void process(QHttpEngine::Socket *socket, const QString &path){
        Q_UNUSED(path);

        // If the slot requires all data to be received, check to see if this is
        // already the case, otherwise, wait until the rest of it arrives
        if (socket->bytesAvailable() >= socket->contentLength()) {
            emit requestReceived(socket);
        } else {
            connect(socket, &QHttpEngine::Socket::readChannelFinished, [this, socket]() {
                emit requestReceived(socket);
            });
        }
    }
};

class OAIApiRouter : public QObject
{
    Q_OBJECT
public:
    OAIApiRouter();
    virtual ~OAIApiRouter();

    void setUpRoutes();
    void processRequest(QHttpEngine::Socket *socket);
    
    void setOAIComponentsApiHandler(QSharedPointer<OAIComponentsApiHandler> handler);
    void setOAIConvertToConfigsApiHandler(QSharedPointer<OAIConvertToConfigsApiHandler> handler);
    void setOAIDeleteInformationApiHandler(QSharedPointer<OAIDeleteInformationApiHandler> handler);
    void setOAIExportOpsimulationManagerXmlApiHandler(QSharedPointer<OAIExportOpsimulationManagerXmlApiHandler> handler);
    void setOAIExportSystemsConfigXmlApiHandler(QSharedPointer<OAIExportSystemsConfigXmlApiHandler> handler);
    void setOAIExportToSimulationApiHandler(QSharedPointer<OAIExportToSimulationApiHandler> handler);
    void setOAIIsSimulationRunningApiHandler(QSharedPointer<OAIIsSimulationRunningApiHandler> handler);
    void setOAIPathToConvertedCasesApiHandler(QSharedPointer<OAIPathToConvertedCasesApiHandler> handler);
    void setOAIRunOpSimulationManagerApiHandler(QSharedPointer<OAIRunOpSimulationManagerApiHandler> handler);
    void setOAISendPCMFileApiHandler(QSharedPointer<OAISendPCMFileApiHandler> handler);
    void setOAISystemsApiHandler(QSharedPointer<OAISystemsApiHandler> handler);
    void setOAIVerifyPathApiHandler(QSharedPointer<OAIVerifyPathApiHandler> handler);
private:
    QMap<QString, std::function<void(QHttpEngine::Socket *)>> Routes;
    QMultiMap<QString, std::function<void(QHttpEngine::Socket *)>> RoutesWithPathParam;

    bool handleRequest(QHttpEngine::Socket *socket);
    bool handleRequestAndExtractPathParam(QHttpEngine::Socket *socket);

    
    QSharedPointer<OAIComponentsApiHandler> mOAIComponentsApiHandler;
    QSharedPointer<OAIConvertToConfigsApiHandler> mOAIConvertToConfigsApiHandler;
    QSharedPointer<OAIDeleteInformationApiHandler> mOAIDeleteInformationApiHandler;
    QSharedPointer<OAIExportOpsimulationManagerXmlApiHandler> mOAIExportOpsimulationManagerXmlApiHandler;
    QSharedPointer<OAIExportSystemsConfigXmlApiHandler> mOAIExportSystemsConfigXmlApiHandler;
    QSharedPointer<OAIExportToSimulationApiHandler> mOAIExportToSimulationApiHandler;
    QSharedPointer<OAIIsSimulationRunningApiHandler> mOAIIsSimulationRunningApiHandler;
    QSharedPointer<OAIPathToConvertedCasesApiHandler> mOAIPathToConvertedCasesApiHandler;
    QSharedPointer<OAIRunOpSimulationManagerApiHandler> mOAIRunOpSimulationManagerApiHandler;
    QSharedPointer<OAISendPCMFileApiHandler> mOAISendPCMFileApiHandler;
    QSharedPointer<OAISystemsApiHandler> mOAISystemsApiHandler;
    QSharedPointer<OAIVerifyPathApiHandler> mOAIVerifyPathApiHandler;
protected:
    // override this method to provide custom class derived from ApiHandler classes
    virtual void createApiHandlers();

private :
    inline QString fromQHttpEngineMethod(QHttpEngine::Socket::Method method){
        switch( method ){
            case QHttpEngine::Socket::Method::OPTIONS:
                return QStringLiteral("OPTIONS");
            case QHttpEngine::Socket::Method::GET:
                return QStringLiteral("GET");
            case QHttpEngine::Socket::Method::HEAD:
                return QStringLiteral("HEAD");
            case QHttpEngine::Socket::Method::POST:
                return QStringLiteral("POST");
            case QHttpEngine::Socket::Method::PUT:
                return QStringLiteral("PUT");
            case QHttpEngine::Socket::Method::DELETE:
                return QStringLiteral("DELETE");
            case QHttpEngine::Socket::Method::TRACE:
                return QStringLiteral("TRACE");
            case QHttpEngine::Socket::Method::CONNECT:
                return QStringLiteral("CONNECT");
        }
        return QStringLiteral("");
    }

    inline QRegularExpressionMatch getRequestMatch(QString serverTemplatePath, QString requestPath){
        QRegularExpression parExpr( R"(\{([^\/\s]+)\})" );
        serverTemplatePath.replace( parExpr, R"((?<\1>[^\/\s]+))" );
        serverTemplatePath.append("[\\/]?$");
        QRegularExpression pathExpr( serverTemplatePath );
        return pathExpr.match( requestPath );
    }

};


}

#endif // OAI_APIROUTER_H
