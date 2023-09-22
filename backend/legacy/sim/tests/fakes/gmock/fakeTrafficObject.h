/********************************************************************************
 * Copyright (c) 2022 BMW bitte hier!
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/globalDefinitions.h"
#include "gmock/gmock.h"
#include "fakeWorldObject.h"
#include "include/trafficObjectInterface.h"
//#include "include/egoAgentInterface.h"

using OpenDriveId = std::string;

class FakeTrafficObject : public FakeWorldObject, public TrafficObjectInterface
{
  public:
    MOCK_CONST_METHOD0(GetOpenDriveId, OpenDriveId());
    MOCK_CONST_METHOD0(GetZOffset, double());
    MOCK_CONST_METHOD0(GetIsCollidable, bool());
};