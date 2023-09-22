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

#include "gtest/gtest.h"
#include <QString>

#include "OPGUICore.h"
#include "OPGUISystemEditor.h"

class EXPORT_COMPONENT_TEST : public ::testing::Test {
  public:
    OPGUISystemEditor::SystemEditor systemEditor;

    bool parseComponentJson(const QString& jsonStr, OpenAPI::OAIComponentUI& oai_component);
    void SetUp() override;
    void TearDown() override;
};
