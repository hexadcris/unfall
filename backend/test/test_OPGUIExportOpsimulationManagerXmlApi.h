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

#include "OAIOpSimulationManagerXmlRequest.h"
#include "OAIDefault200Response.h"

class XML_EXPORT_TEST : public ::testing::Test {
  public:
    QString openPassCoreDir;
    QString configFileFullPath;
    QString backupFilePath;
    OpenAPI::OAIOpSimulationManagerXmlRequest request;
    OpenAPI::OAIDefault200Response response;
    OpenAPI::OAIDefault200Response response200Expected;
    
    void SetUp() override;

    void TearDown() override;
};




