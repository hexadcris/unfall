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

#include "OAIPathRequest.h"
#include "OAISelectedExperimentsRequest.h"


class SEND_PCM_FILE_TEST : public ::testing::Test {
  public:
    QString workSpacePath;
    QString testDir;
    QString testDirFullPath;
    OpenAPI::OAIPathRequest request;
    QString pathToConvertedCasesOriginal;
    QString pathToConfigFile;
    QString destinationFilePathTestDB;
    OpenAPI::OAISelectedExperimentsRequest response200Expected;

    void SetUp() override;
    
    void TearDown() override;
};

