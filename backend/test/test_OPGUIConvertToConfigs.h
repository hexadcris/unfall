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
#include <QString>

#include <OPGUICoreGlobalConfig.h>
#include "OAIDefault200Response.h"
#include "OAIConfigsRequest.h"

class OPGUICoreGlobalConfig;

class CONVERT_TO_CONFIGS_TEST : public ::testing::Test {
  public:
    QString testDirFullPath;
    QString destinationFilePathTestDB;
    QString testDirSystemsPath;
    QString testSystemsFilePath;
    QString filePathSys1;
    QString filePathSys2;
    OpenAPI::OAIDefault200Response response;
    OpenAPI::OAIDefault200Response response200Expected;
    

    OPGUICoreGlobalConfig* config;

    void InitSimulation(const QString &pcm_db_path);

    void verifyConfigsDirectoryStructure(const QString &basePath);

    bool parseConfigsReqJson(const QString& jsonStr, OpenAPI::OAIConfigsRequest& oai_configs_request);

    void DeleteSimulation();

    void SetUp() override;
    
    void TearDown() override;

    
};





