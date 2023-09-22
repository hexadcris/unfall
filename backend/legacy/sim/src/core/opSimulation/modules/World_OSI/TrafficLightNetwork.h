/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "OWL/DataTypes.h"
#include "common/openPassTypes.h"
#include "include/dataBufferInterface.h"
#include "WorldData.h"

using AcyclicPublisher = std::function<void(openpass::type::EntityId id, openpass::type::FlatParameterKey key, const openpass::databuffer::Acyclic& value)>;

//! Holds a set of linked traffic lights (i.e. for one junction) and manages their states
class TrafficLightController
{
public:
    //! \brief Updates the states of all traffic lights of this controller
    //!
    //! \param time     current simulation timestamp
    void UpdateStates(int time);

    //! Each controller consist of a sequence of phases, that define the state of each traffic light
    struct Phase
    {
        int duration;
        std::vector<std::pair<OWL::Interfaces::TrafficLight*, CommonTrafficLight::State>> states;
        std::string stateDescriptor;
        OWL::Id osiId;
    };

    TrafficLightController(std::vector<Phase>&& phases, double delay);
    inline Phase& getCurrentPhase () const { return *currentPhase; }
    inline const int getLastTime() const { return lastTime; }
    bool getTimeToPublish() const {return timeToPublish; }
    void setTimeToPublishFalse() {timeToPublish = false; }

private:
    std::vector<Phase> phases;
    std::vector<Phase>::iterator currentPhase;
    int timeRemainingInCurrentPhase;
    int lastTime;
    bool timeToPublish = true;
};

//!Holds all TrafficLightControllers and triggers them each timestep
class TrafficLightNetwork
{

public:
    TrafficLightNetwork();

    //! \brief Updates the states of all traffic lights in the network
    //!
    //! \param time     current simulation timestamp
    void UpdateStates(int time);

    //! \brief Adds a new controller to the network
    //!
    //! \param controller   new controller to add
    void AddController(TrafficLightController&& controller);

    void PublishGlobalData(AcyclicPublisher publish);

    void registerOsiTrafficLightId(const OWL::Id id) { osiTrafficLightIds.emplace(id); }
    const std::vector<OWL::Id> getOsiTrafficLightIds() const {return std::vector<OWL::Id> (osiTrafficLightIds.begin(), osiTrafficLightIds.end()) ;}

private:
    std::vector<TrafficLightController> controllers;
    std::set<OWL::Id> osiTrafficLightIds;
};
