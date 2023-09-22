
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

#include <QThread>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QIODevice>

#include "OPGUIQtLogger.h"

QString OPGUIQtLogger::logFilename = QStringLiteral("");

void OPGUIQtLogger::setLogFilename(const QString& filename) {
    logFilename = filename;
}

OPGUIQtLogger& OPGUIQtLogger::instance() {
    static OPGUIQtLogger instance(logFilename);
    return instance;
}

void OPGUIQtLogger::resetLogger() {
        if(logFile.isOpen()) {
            logFile.close();
        }
        logFile.setFileName(logFilename);
        if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
            qWarning() << "logger::Failed to open log file:" << logFilename;
        }
}

void OPGUIQtLogger::write(const QString &message, const QString &level) {
    QMutexLocker locker(&mutex); // Locks the mutex. Will be unlocked when this object is destroyed (i.e., when the function exits).
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    if (logFile.isOpen()) {
        QTextStream out(&logFile);
        out << timestamp << " [" << level << "] " << message << "\n";
    }
    // Console Logging
    QTextStream stdoutStream(stdout);
    QTextStream stderrStream(stderr);
    
    // ANSI Escape codes for coloring
    const QString RESET_COLOR = "\033[0m";
    QString colorPrefix;

    if (level == "INFO") {
        colorPrefix = "\033[32m";  // Green for INFO
    } else if (level == "DEBUG") {
        colorPrefix = "\033[34m";  // Blue for DEBUG
    } else if (level == "WARN") {
        colorPrefix = "\033[33m";  // Yellow for WARN
    } else if (level == "ERROR") {
        colorPrefix = "\033[31m";  // Red for ERROR
    } else {
        colorPrefix = RESET_COLOR;
    }

    QString coloredLevel = colorPrefix + "[" + level + "]" + RESET_COLOR;
    QString formattedMessage = timestamp + " " + coloredLevel + " " + message;

    if (level == "INFO" || level == "DEBUG") {
        stdoutStream << formattedMessage << "\n";
    } else {
        stderrStream << formattedMessage << "\n";
    }
}

void OPGUIQtLogger::info(const QString &message) {
    write(message, "INFO");
}

void OPGUIQtLogger::error(const QString &message) {
    write(message, "ERROR");
}

void OPGUIQtLogger::warn(const QString &message) {
    write(message, "WARN");
}

void OPGUIQtLogger::debug(const QString &message) {
    write(message, "DEBUG");
}


OPGUIQtLogger::OPGUIQtLogger(const QString &filename) {
    logFile.setFileName(filename);
    if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "logger::Failed to open log file:" << filename;
    }
    
}



