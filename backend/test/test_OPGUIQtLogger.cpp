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

#include <QTextStream>
#include <QDebug>

#include "OPGUIQtlogger.h"
#include "OPGUICoreGlobalConfig.h"
#include "test_OPGUIQtLogger.h"


void TestOPGUIQtLogger::SetUp() {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
    this->originalLogFile =  OPGUICoreGlobalConfig::getInstance().pathLogFile();
    this->logFilePathTest = this->originalLogFile+QStringLiteral("_test");
    this->logFile.setFileName(this->logFilePathTest);
    //remove log file as it will be newly created by logger if does not exist
    if (this->logFile.exists()) {
        this->logFile.remove();
    }
    OPGUICoreGlobalConfig::getInstance().setPathLogFile(this->logFilePathTest);
    OPGUIQtLogger::setLogFilename(this->logFilePathTest);
    OPGUIQtLogger::instance().resetLogger();
}

void TestOPGUIQtLogger::TearDown() {
    if (logFile.isOpen()) {
        logFile.close();
    }
    if (logFile.exists()) {
        logFile.remove();
    }
    OPGUICoreGlobalConfig::getInstance().setPathLogFile(this->originalLogFile);
    OPGUIQtLogger::setLogFilename(this->originalLogFile);
    OPGUIQtLogger::instance().resetLogger();

    //security removal of any .log_new_test file
    QFile targetFile(this->originalLogFile + "_test_new");
    if (targetFile.exists()) {
        targetFile.remove();
    }
}

QStringList TestOPGUIQtLogger::readLogFileContents() {
    QStringList lines;
    if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&logFile);
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        logFile.close();
    }
    else{
        qWarning() << "ERROR::Failed to open file" << logFile.errorString();
        return lines;
    }
    return lines;
}

//...

TEST_F(TestOPGUIQtLogger, TestSingletonPattern) {
    OPGUIQtLogger& instance1 = OPGUIQtLogger::instance();
    OPGUIQtLogger& instance2 = OPGUIQtLogger::instance();
    ASSERT_EQ(&instance1, &instance2) << "Test: Expected two instances of OPGUIQtLogger to point to the same memory address, but they didn't.";
}

TEST_F(TestOPGUIQtLogger, TestFileLogging) {
    QString message = "This is a test INFO message.";
    LOGGER.info(message);

    QStringList logContents = readLogFileContents();
    ASSERT_EQ(logContents.size(), 1) << "Test: Expected 1 line in log file but found: " << logContents.size();
    ASSERT_TRUE(logContents.first().contains(message)) << "Test: Expected message '" << message.toStdString() << "' not found in log contents.";
}

TEST_F(TestOPGUIQtLogger, TestDifferentLevels) {
    QString errorMessage = "This is a test ERROR message.";
    QString warnMessage = "This is a test WARN message.";
    QString debugMessage = "This is a test DEBUG message.";
    LOGGER.error(errorMessage);
    LOGGER.warn(warnMessage);
    LOGGER.debug(debugMessage);

    QStringList logContents = readLogFileContents();
    ASSERT_EQ(logContents.size(), 3) << "Test: Expected 3 lines in log file but found: " << logContents.size();
    ASSERT_TRUE(logContents[0].contains(errorMessage)) << "Test: Expected message '" << errorMessage.toStdString() << "' not found in log contents.";
    ASSERT_TRUE(logContents[1].contains(warnMessage)) << "Test: Expected message '" << warnMessage.toStdString() << "' not found in log contents.";
    ASSERT_TRUE(logContents[2].contains(debugMessage)) << "Test: Expected message '" << debugMessage.toStdString() << "' not found in log contents.";
}

TEST_F(TestOPGUIQtLogger, ResetLoggerTest) {
    OPGUIQtLogger &logger = OPGUIQtLogger::instance();
    logger.info("Log entry before reset");

    logger.resetLogger();
    logger.info("Log entry after reset");

    QStringList logContents = readLogFileContents();
    
    ASSERT_EQ(logContents.size(), 2) << "Test: Expected 2 lines in log file but found: " << logContents.size();
    ASSERT_TRUE(logContents[0].contains("Log entry before reset")) << "Test: 'Log entry before reset' message not found in log contents.";
    ASSERT_TRUE(logContents[1].contains("Log entry after reset")) << "Test: 'Log entry after reset' message not found in log contents.";
}

TEST_F(TestOPGUIQtLogger, SetLogFilenameTest) {
    QString newLogFilename = logFilePathTest + "_new";
    OPGUIQtLogger &logger = OPGUIQtLogger::instance();

    OPGUIQtLogger::setLogFilename(newLogFilename);
    logger.resetLogger();

    QString message = "Logging to the new log file.";
    logger.info(message);

    QFile newLogFile(newLogFilename);
    QStringList lines;

    if (newLogFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&newLogFile);
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        newLogFile.close();
    } else {
        FAIL() << "Test: Failed to open new log file '" << newLogFilename.toStdString() << "' for reading. Error: " << newLogFile.errorString().toStdString();
    }

    ASSERT_EQ(lines.size(), 1) << "Test: Expected 1 line in new log file but found: " << lines.size();
    ASSERT_TRUE(lines.first().contains(message)) << "Test: Expected message '" << message.toStdString() << "' not found in new log file contents.";

    if (newLogFile.exists()) {
        newLogFile.remove();
    }
}



