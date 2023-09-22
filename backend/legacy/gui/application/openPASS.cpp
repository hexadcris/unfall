/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *               2017-2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <QApplication>
#include <QDir>
#include <QTimer>

#include "Models/PluginManagerModel.h"
#include "Models/ServiceManagerModel.h"
#include "openPASS/RandomInterface.h"

int main(int argc, char *argv[])
{
    // Initialize random interface
    RandomInterface::initialize();

    // Initialize application
    QApplication application(argc, argv);
    application.setApplicationName(QStringLiteral(APPLICATION_NAME));
    application.setApplicationVersion(QStringLiteral(APPLICATION_VERSION));

    QString subdirLibGuiCmake = SUBDIR_LIB_GUI;
    QDir subdirLibGui = QDir(QApplication::applicationDirPath()).filePath(subdirLibGuiCmake);
    application.addLibraryPath(subdirLibGui.absolutePath());

    // Initialize models
    ServiceManagerModel services;
    PluginManagerModel plugins(&services);

    // Schedule application
    QTimer::singleShot(0, [&plugins, &subdirLibGui]() {
        // Load plugins

        plugins.loadDirectory(subdirLibGui);

        // Emit signal 'started'
        Q_EMIT plugins.started();
    });

    // Execute application
    return application.exec();
}
