/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Channel.h
//! @brief This file contains the internal representation of a channel instance
//!        during a simulation run.
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <string>
#include <tuple>
#include <cstddef>
#include "include/modelInterface.h"
#include "include/componentInterface.h"

namespace core
{

class Agent;
class ChannelBuffer;

class Channel
{
public:
    // index within tuple to identify (link/component pair) of channel targets
    enum class TargetLinkType : std::size_t
    {
        LinkId = 0,
        Component
    };

    Channel(int id);
    Channel(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel& operator=(Channel&&) = delete;
    virtual ~Channel() = default;

    void SetAgent(Agent *agent);
    Agent *GetAgent() const;
    bool SetSource(ComponentInterface *source, int sourceLinkId);
    bool AttachSourceBuffer(ChannelBuffer *buffer);
    bool AddTarget(ComponentInterface *target, int targetLinkId);
    ComponentInterface *GetSource() const;
    const std::vector<std::pair<int, ComponentInterface*>> &GetTargets() const;

private:
    static const int Undefined = -1;

    Agent *agent;
    int id;
    int sourceLinkId;
    ComponentInterface *source;
    std::vector<std::pair<int, ComponentInterface*>> targets;
    ChannelBuffer *buffer;
};

} // namespace core


