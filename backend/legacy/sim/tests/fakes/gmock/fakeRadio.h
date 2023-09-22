/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <vector>

#include "gmock/gmock.h"

#include "common/DetectedObject.h"
#include "include/radioInterface.h"

class FakeRadio : public RadioInterface
{
public:
    MOCK_METHOD4(Send, void(double, double, double, DetectedObject));
    MOCK_METHOD3(Receive, std::vector<DetectedObject>(double, double, double));
    MOCK_METHOD0(Reset, void());
};

