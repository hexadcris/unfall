/*
 * Copyright (c) 2023 Hexad GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * opGUI - OpenAPI 3.0
 * The version of the OpenAPI document: 3.0.1
 *
 */


#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QHostAddress>
#include <QRegExp>
#include <QSharedPointer>
#include <QObject>
#include <QFile>
#include <QString>



#include <thread>
#ifdef __linux__
#include <signal.h>
#include <unistd.h>
#endif

#include "OPGUIInit.h"
#include "OPGUICoreGlobalConfig.h"
#include "OPGUIQtLogger.h"
#include <OAIApiRouter.h>
#include <qhttpengine/server.h>
#include <qhttpengine/qiodevicecopier.h>
#include <OAIApiRouterOP.h>
#include <qhttpengine/filesystemhandler.h>
#include <ServeIndexHtml.h>


#ifdef __linux__
void catchUnixSignals(QList<int> quitSignals) {
    auto handler = [](int sig) -> void {
        // blocking and not async-signal-safe func are valid
        qDebug() << "\nquit the application by signal " << sig;
        QCoreApplication::quit();
    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig : quitSignals)
        sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    for (auto sig : quitSignals)
        sigaction(sig, &sa, nullptr);
}
#endif

int main(int argc, char * argv[])
{

    QCoreApplication a(argc, argv);
#ifdef __linux__
    QList<int> sigs({SIGQUIT, SIGINT, SIGTERM, SIGHUP});
    catchUnixSignals(sigs);
#endif
    if (!OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful()) {
        qDebug() << "Failed to initialize the global configuration.";
        return 1;  
    }

    OPGUIQtLogger::setLogFilename(OPGUICoreGlobalConfig::getInstance().pathLogFile());
    if( OPGUIInit::Initialise_OPGUI())
    {
        LOG_INFO("Application initialised successfully");
    }
    else
    {
        LOG_ERROR("One or more errors have been encountered during initialisation, please check config file and fix the errors.");
        qDebug() << "Failed to initialize the global configuration.";
        return 1;  
    }
    
    // Obtain the values
    QHostAddress address = QHostAddress(OPGUICoreGlobalConfig::getInstance().backendIP());
    quint16 port = OPGUICoreGlobalConfig::getInstance().backendPort();

    QHttpEngine::QObjectHandler rootHandler;
    ServeIndexHtml serveIndexHtml;
    rootHandler.registerMethod("", &serveIndexHtml, &ServeIndexHtml::serve);

    // Build the hierarchy of handlers
    QHttpEngine::FilesystemHandler fileHandler(OPGUICoreGlobalConfig::getInstance().pathHome() + "/" + "opgui/backend/build/dist/assets");
    QHttpEngine::FilesystemHandler fileHandlerFonts(OPGUICoreGlobalConfig::getInstance().pathHome() + "/" + "opgui/backend/build/dist/fonts");

    QSharedPointer<OpenAPI::OAIApiRequestHandler> restHandler(new OpenAPI::OAIApiRequestHandler());

    rootHandler.addSubHandler(QRegExp("assets/"), &fileHandler);
    rootHandler.addSubHandler(QRegExp("fonts/"), &fileHandlerFonts);
    rootHandler.addSubHandler(QRegExp("api/"), restHandler.data());

    QHttpEngine::Server server(&rootHandler);

    auto router = QSharedPointer<OAIApiRouterOP>::create();
    router->setUpRoutes();

    QObject::connect(restHandler.data(), &OpenAPI::OAIApiRequestHandler::requestReceived, [&](QHttpEngine::Socket *socket) {
        router->processRequest(socket);
    });

    qDebug() << "[INFO]Serving on " << address.toString() << ":" << port;
    // Attempt to listen on the specified port
    if (!server.listen(address, port)) {
        qCritical("[ERROR]Unable to listen on the specified port.");
        return 1;
    }

    int execResult = a.exec();

    return execResult;
}
