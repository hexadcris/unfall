/********************************************************************************
 * Copyright (c) 2019-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gmock/gmock.h"

#include "include/roadInterface/roadObjectInterface.h"

class FakeRoadObject : public RoadObjectInterface
{
public:
    MOCK_CONST_METHOD0(GetType, RoadObjectType());
    MOCK_CONST_METHOD0(GetId, std::string());
    MOCK_CONST_METHOD0(GetS, double());
    MOCK_CONST_METHOD0(GetT, double());
    MOCK_CONST_METHOD0(GetZOffset, double());
    MOCK_CONST_METHOD1(IsValidForLane, bool(int));
    MOCK_CONST_METHOD0(GetLength, double());
    MOCK_CONST_METHOD0(GetWidth, double());
    MOCK_CONST_METHOD0(GetHdg, double());
    MOCK_CONST_METHOD0(GetHeight, double());
    MOCK_CONST_METHOD0(GetPitch, double());
    MOCK_CONST_METHOD0(GetRoll, double());
    MOCK_CONST_METHOD0(GetName, std::string());
    MOCK_CONST_METHOD0(IsContinuous, bool());
};

