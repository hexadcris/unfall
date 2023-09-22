/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "gmock/gmock.h"

#include "OWL/DataTypes.h"

namespace OWL::Fakes {
class Junction : public OWL::Interfaces::Junction
{
public:
    MOCK_CONST_METHOD0(GetId, const std::string& ());
    MOCK_METHOD1(AddConnectingRoad, void (const OWL::Interfaces::Road* connectingRoad));
    MOCK_CONST_METHOD0(GetConnectingRoads, const OWL::Interfaces::Roads& ());
    MOCK_METHOD2(AddPriority, void (const std::string& high, const std::string& low));
    MOCK_CONST_METHOD0(GetPriorities, const Priorities& ());
    MOCK_METHOD2(AddIntersectionInfo, void (const std::string&, const IntersectionInfo&));
    MOCK_CONST_METHOD0(GetIntersections, const std::map<std::string, std::vector<IntersectionInfo>>& ());
};
}
