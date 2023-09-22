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

#include <QObject>
#include <QStringList>

#include <qhttpengine/socket.h>

class ServeIndexHtml : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void serve(QHttpEngine::Socket *socket);
};

