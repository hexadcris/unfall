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

#ifndef OPGUIQTLOGGER_H
#define OPGUIQTLOGGER_H

#include <QFile>
#include <QMutex>
#include <QString>

class OPGUIQtLogger {
public:
    static void setLogFilename(const QString& filename);
    static OPGUIQtLogger& instance();

    virtual void write(const QString &message, const QString &level = "INFO");
    virtual void info(const QString &message);
    virtual void error(const QString &message);
    virtual void warn(const QString &message);
    virtual void debug(const QString &message);
    void resetLogger();

private:
    QFile logFile;
    QMutex mutex; // For thread safety
    static QString logFilename;

    

    // Delete copy and assignment constructors
    OPGUIQtLogger(const OPGUIQtLogger&) = delete;
    OPGUIQtLogger& operator=(const OPGUIQtLogger&) = delete;
protected:
    OPGUIQtLogger(const QString &filename);  // Protected constructor for the singleton pattern (to be able to mock)
};

// Macros for easier access:
#define LOGGER OPGUIQtLogger::instance()
#define LOG_INFO(message) LOGGER.info(message)
#define LOG_ERROR(message) LOGGER.error(message)
#define LOG_WARN(message) LOGGER.warn(message)
#define LOG_DEBUG(message) LOGGER.debug(message)

#endif // OPGUIQTLOGGER_H
