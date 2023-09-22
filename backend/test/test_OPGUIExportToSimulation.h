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

#include <OPGUICoreGlobalConfig.h>
#include "OAIDefault200Response.h"
#include "OAISelectedExperimentsRequest.h"

class OPGUICoreGlobalConfig;

class EXPORT_TO_SIMULATION_TEST : public ::testing::Test {
  public:
    QString workSpacePath;
    QString testDir;
    QString testDirFullPath;
    OpenAPI::OAISelectedExperimentsRequest request;
    QString pathToConvertedCasesOriginal;
    QString pathToConfigFile;
    QString destinationFilePathTestDB;
    OpenAPI::OAIDefault200Response response;
    OpenAPI::OAIDefault200Response response200Expected;

    OPGUICoreGlobalConfig* config;

    void InitSimulation(const QString &pcm_db_path);

    void DeleteSimulation();

    void SetUp() override;
    
    void TearDown() override;

    
};





