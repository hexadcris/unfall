/********************************************************************************
 * Copyright (c) 2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/roadInterface/connectionInterface.h"

class FakeConnection : public ConnectionInterface
{
  public:
    MOCK_METHOD2(AddLink, void(int, int));
    MOCK_CONST_METHOD0(GetConnectingRoadId, const std::string&());
    MOCK_CONST_METHOD0(GetIncommingRoadId, const std::string&());
    MOCK_CONST_METHOD0(GetLinks, const std::map<int, int>&());
    MOCK_CONST_METHOD0(GetContactPoint, ContactPointType());
};
