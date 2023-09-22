/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/opExport.h"
#include "include/profilesInterface.h"

class SIMULATIONCOREEXPORT Profiles : public ProfilesInterface
{
public:
    virtual ~Profiles() override = default;

    virtual const std::unordered_map<std::string, AgentProfile>& GetAgentProfiles() const override;

    virtual bool AddAgentProfile(std::string agentProfileName, AgentProfile agentProfile) override;

    virtual const std::unordered_map<std::string, VehicleProfile>& GetVehicleProfiles() const override;

    virtual void AddVehicleProfile(const std::string& profileName, const VehicleProfile& vehicleProfile) override;

    virtual const ProfileGroups& GetProfileGroups() const override;

    virtual bool AddProfileGroup(std::string profileType, std::string profileName, openpass::parameter::ParameterSetLevel1 parameters) override;

    virtual const StringProbabilities& GetDriverProbabilities(std::string agentProfileName) const override;

    virtual const StringProbabilities& GetVehicleProfileProbabilities(std::string agentProfileName) const override;

    virtual const openpass::parameter::ParameterSetLevel1& GetProfile(const std::string& type, const std::string& name) const override;

    virtual openpass::parameter::ParameterSetLevel1 CloneProfile(const std::string& type, const std::string& name) const override;

private:
    std::unordered_map<std::string, AgentProfile> agentProfiles {};
    std::unordered_map<std::string, VehicleProfile> vehicleProfiles {};
    ProfileGroups profileGroups;
};
