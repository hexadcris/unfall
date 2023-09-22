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
#include "OAIVerifyPath_200_response.h"


class VERIFY_PATH_TEST : public ::testing::Test {
  public:
    QString workSpacePath;
    QString testDir;
    QString testDirFullPath;
    OpenAPI::OAIPathRequest request;
    OpenAPI::OAIVerifyPath_200_response response;

    void SetUp() override;
    void TearDown() override;
};