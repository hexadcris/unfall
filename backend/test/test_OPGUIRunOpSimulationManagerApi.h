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

#pragma once

#include <gtest/gtest.h>

#include "OPGUICore.h"
#include "OAIPathRequest.h"
#include "OAIDefault200Response.h"
#include "OPGUICoreGlobalConfig.h"


class RUN_OP_SIMULATION_MANAGER_TEST : public ::testing::Test {
  public:
    QString openPassCoreDir;
    QString configFileFullPath;
    QString backupFilePath;
    QString opSimulationExe;

    OpenAPI::OAIDefault200Response response;
    OpenAPI::OAIDefault200Response response200Expected;
    
    void SetUp() override;
    
    void TearDown() override;

    void checkLogValues(const QString& logContent, 
                    const QString& expectedLogLevel, 
                    const QString& expectedLogFile, 
                    const QString& expectedLibrary, 
                    const QString& expectedNumSimulations,
                    const QString& expectedSimulationExec);
};
