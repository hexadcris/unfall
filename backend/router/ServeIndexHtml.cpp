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
#include <QFile>
#include <QObject>

#include <qhttpengine/qiodevicecopier.h>
#include "ServeIndexHtml.h"
#include "OPGUICoreGlobalConfig.h"

void ServeIndexHtml::serve(QHttpEngine::Socket *socket){

    QFile *file = new QFile(OPGUICoreGlobalConfig::getInstance().pathHome() + "/" + "opgui/backend/build/dist/index.html");
    file->open(QIODevice::ReadOnly);
    QHttpEngine::QIODeviceCopier *copier = new QHttpEngine::QIODeviceCopier(file, socket);

    QObject::connect(copier, &QHttpEngine::QIODeviceCopier::finished, [socket]() {
        if (socket->isOpen()){
            socket->setStatusCode(QHttpEngine::Socket::OK);
            socket->setHeader("Content-Type", "text/html");
            socket->writeHeaders();
            socket->close();
        }
    });
    copier->start();
}