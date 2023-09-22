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

#include <list>
#include <memory>
#include <unordered_map>

#include "common/parameter.h"
#include "common/sensorDefinitions.h"

using StringProbabilities = std::vector<std::pair<std::string, double>>;
using ProfileGroup = std::unordered_map<std::string, openpass::parameter::ParameterSetLevel1>;
using ProfileGroups = std::unordered_map<std::string, ProfileGroup>;

enum class AgentProfileType
{
    Static,
    Dynamic
};

struct AgentProfile
{
    //Dynamic profile
    StringProbabilities driverProfiles {};
    StringProbabilities vehicleProfiles {};

    //Static profile
    std::string systemConfigFile;
    int systemId;
    std::string vehicleModel;

    AgentProfileType type;
};

struct SensorLink
{
    int sensorId {};
    std::string inputId {};
};

struct VehicleComponent
{
    std::string type {};
    StringProbabilities componentProfiles {};
    std::vector<SensorLink> sensorLinks {};
};

struct VehicleProfile
{
    std::string vehicleModel {};
    std::vector<VehicleComponent> vehicleComponents {};
    openpass::sensors::Parameters sensors {};
};

//-----------------------------------------------------------------------------
//! Interface provides access to the profiles catalog
//-----------------------------------------------------------------------------
class ProfilesInterface
{
public:
    virtual ~ProfilesInterface() = default;

    /*!
    * \brief Returns a pointer to the agentProfiles
    * 
    * @return        agentProfiles
    */
    virtual const std::unordered_map<std::string, AgentProfile>& GetAgentProfiles() const = 0;

    /*!
    * \brief Add agentProfile with the specified agentProfileName to the map of agentProfiles if agentProfile name does not yet exist
    * 
    * @param[in]     agentProfileName        name of the agentProfile
    * @param[in]     agentProfile            agentProfile which shall be added
    * @return        true if agentProfile with the specified agentProfile name is added to agentProfiles or false if an equivalent agentProfile name already exist
    */
    virtual bool AddAgentProfile(std::string agentProfileName, AgentProfile agentProfile) = 0;

    /*!
    * \brief Returns a pointer to the map of vehicle profiles
    *
    * @return        vehicleProfiles
    */
    virtual const std::unordered_map<std::string, VehicleProfile>& GetVehicleProfiles() const = 0;

    /*!
    * \brief Add vehicleProfile with the specified profileName to the map of vehicle profiles
    *
    * @param[in]     profileName        name of the vehicleProfile
    * @param[in]     vehicleProfile     vehicleProfile which shall be added
    */
    virtual void AddVehicleProfile(const std::string& profileName, const VehicleProfile& vehicleProfile) = 0;

    /*!
    * \brief Returns a pointer to the map of profile groups
    *
    * @return        ProfileGroups
    */
    virtual const ProfileGroups& GetProfileGroups() const = 0;

    /*!
    * \brief Add profile group to the map of profile groups if profileName does not yet exist within a profile group
    *
    * @param[in]     profileType        type of the profile group
    * @param[in]     profileName        name of the profile
    * @param[in]     parameters         specified profile with the specified name in the specified profile groups
    * @return        true if profile group is added to profile groups or false if an equivalent profile name already exist
    */
    virtual bool AddProfileGroup(std::string profileType, std::string profileName, openpass::parameter::ParameterSetLevel1 parameters) = 0;

    /*!
    * \brief Returns the driver profile probabilities of an agentProfile
    *
    * @param[in]    agentProfileName        Name of the agentProfile from which the probabilities are requested
    * @return       probality map for driver profiles
    */
    virtual const StringProbabilities& GetDriverProbabilities(std::string agentProfileName) const = 0;

    /*!
    * \brief Returns the vehicle profile probabilities of an agentProfile
    *
    * @param[in]    agentProfileName        Name of the agentProfile from which the probabilities are requested
    * @return       probality map for vehicle profiles
    */
    virtual const StringProbabilities& GetVehicleProfileProbabilities(std::string agentProfileName) const = 0;

    //! \brief Returns the profile with the specified name in the specified profile gropus
    //!
    //! \param type     type of the profile group
    //! \param name     name of the profile
    //! \return         specified profile
    virtual const openpass::parameter::ParameterSetLevel1& GetProfile(const std::string& type, const std::string& name) const = 0;

    //! \brief Returns the profile with the specified name in the specified profile gropus
    //!
    //! \param type     type of the profile group
    //! \param name     name of the profile
    //! \return         specified profile
    virtual openpass::parameter::ParameterSetLevel1 CloneProfile(const std::string& type, const std::string& name) const = 0;
};
