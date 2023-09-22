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

#ifndef TEST_OPGUIQTLOGGER_H
#define TEST_OPGUIQTLOGGER_H

#include <QThread>
#include <gtest/gtest.h>
#include <QString>
#include <QFile>
#include <QStringList>

class TestOPGUIQtLogger : public ::testing::Test {
protected:
    QString originalLogFile;
    QString logFilePathTest;
    QFile logFile;

    void SetUp() override;
    void TearDown() override;
    QStringList readLogFileContents();
};

#endif // TEST_OPGUIQTLOGGER_H

