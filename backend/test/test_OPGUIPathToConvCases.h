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

#include <QString>
#include <gtest/gtest.h>

#include "OAIPathRequest.h"
#include "OAIDefault200Response.h"


//TODO: ADD TEST FOR SIM OBJECT STARTED

class PATH_TO_CONVERTED_CASES_TEST : public ::testing::Test {
  public:
    QString testDir;
    QString testDirFullPath;
    OpenAPI::OAIPathRequest request;
    OpenAPI::OAIDefault200Response response;
    OpenAPI::OAIDefault200Response response200Expected;
    QString pathToConvertedCasesOriginal;
    QString pathToConfigFile;

    QString getConvertedCasesPath(const QString &configFilePath);

    bool removeConvertedCasesPath(const QString &configFilePath);

    void SetUp() override;
    
    void TearDown() override;
};
