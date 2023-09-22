/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <stdexcept>

#include "profiles.h"
#include <stdexcept>

const std::unordered_map<std::string, AgentProfile>& Profiles::GetAgentProfiles() const
{
    return agentProfiles;
}

bool Profiles::AddAgentProfile(std::string agentProfileName, AgentProfile agentProfile)
{
    return agentProfiles.insert({agentProfileName, agentProfile}).second;
}

const std::unordered_map<std::string, VehicleProfile>& Profiles::GetVehicleProfiles() const
{
    return vehicleProfiles;
}

void Profiles::AddVehicleProfile(const std::string& profileName, const VehicleProfile& vehicleProfile)
{
    vehicleProfiles.insert({profileName, vehicleProfile});
}

const ProfileGroups& Profiles::GetProfileGroups() const
{
    return profileGroups;
}

bool Profiles::AddProfileGroup(std::string profileType, std::string profileName, openpass::parameter::ParameterSetLevel1 parameters)
{
    return profileGroups[profileType].emplace(profileName, parameters).second;
}

const StringProbabilities& Profiles::GetDriverProbabilities(std::string agentProfileName) const
{
    try
    {
        return agentProfiles.at(agentProfileName).driverProfiles;
    }
    catch (const std::out_of_range&)
    {
        throw std::runtime_error("Cannot retrieve driver probabilities. Unknown agent profile " + agentProfileName);
    }
}

const StringProbabilities& Profiles::GetVehicleProfileProbabilities(std::string agentProfileName) const
{
    try
    {
        return agentProfiles.at(agentProfileName).vehicleProfiles;
    }
    catch (const std::out_of_range&)
    {
        throw std::runtime_error("Cannot retrieve vehicle probabilities. Unknown agent profile " + agentProfileName);
    }
}

const openpass::parameter::ParameterSetLevel1& Profiles::GetProfile(const std::string& type, const std::string& name) const
{
    try
    {
        return profileGroups.at(type).at(name);
    }
        catch(const std::exception&)
    {
        throw std::runtime_error("Could not find Profile: " + name + " in ProfileGroup: " + type);
    }
}

openpass::parameter::ParameterSetLevel1 Profiles::CloneProfile(const std::string& type, const std::string& name) const
{
    return GetProfile(type, name);
}