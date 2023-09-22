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
#include "include/profilesInterface.h"

class FakeProfiles : public ProfilesInterface {
 public:
  MOCK_CONST_METHOD0(GetAgentProfiles,
      const std::unordered_map<std::string, AgentProfile> &());
  MOCK_METHOD2(AddAgentProfile,
      bool (std::string, AgentProfile));
  MOCK_CONST_METHOD0(GetVehicleProfiles,
      const std::unordered_map<std::string, VehicleProfile> &());
  MOCK_METHOD2(AddVehicleProfile,
      void (const std::string&, const VehicleProfile&));
  MOCK_CONST_METHOD0(GetProfileGroups,
      const ProfileGroups &());
  MOCK_METHOD3(AddProfileGroup,
      bool (std::string, std::string, openpass::parameter::ParameterSetLevel1));
  MOCK_CONST_METHOD1(GetDriverProbabilities,
      const StringProbabilities &(std::string));
  MOCK_CONST_METHOD1(GetVehicleProfileProbabilities,
      const StringProbabilities &(std::string));
  MOCK_CONST_METHOD2(GetProfile,
      const openpass::parameter::ParameterSetLevel1& (const std::string&, const std::string&));
  MOCK_CONST_METHOD2(CloneProfile,
      openpass::parameter::ParameterSetLevel1 (const std::string&, const std::string&));
};
