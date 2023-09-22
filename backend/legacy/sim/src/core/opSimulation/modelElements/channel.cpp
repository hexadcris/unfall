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

#include <iostream>
#include <algorithm>

#include "agent.h"
#include "channel.h"
#include "common/log.h"

namespace core
{

Channel::Channel(int id) :
    agent(nullptr),
    id(id),
    sourceLinkId(Undefined),
    source(nullptr),
    buffer(nullptr)
{}

void Channel::SetAgent(Agent *agent)
{
    this->agent = agent;
}

Agent *Channel::GetAgent() const
{
    return agent;
}

bool Channel::SetSource(ComponentInterface *source, int sourceLinkId)
{
    if(this->source || Undefined != this->sourceLinkId)
    {
        LOG_INTERN(LogLevel::Warning) << "trying to override channel source";
        return false;
    }

    this->source = source;
    this->sourceLinkId = sourceLinkId;

    return true;
}

bool Channel::AttachSourceBuffer(ChannelBuffer *buffer)
{
    if(this->buffer)
    {
        LOG_INTERN(LogLevel::Warning) << "trying to override channel source";
        return false;
    }

    this->buffer = buffer;
    return true;
}

bool Channel::AddTarget(ComponentInterface *target, int targetLinkId)
{
    for(auto &item : targets)
    {
        // do not compare linkIds since two links in different components each may have the same name
        if(std::get<static_cast<size_t>(TargetLinkType::Component)>(item) == target)
        {
            LOG_INTERN(LogLevel::Warning) << "targets must be unique";
            return false;
        }
    }

    targets.push_back(std::make_pair(targetLinkId, target));
    return true;
}

ComponentInterface *Channel::GetSource() const
{
    return source;
}

const std::vector<std::pair<int, ComponentInterface*>> &Channel::GetTargets() const
{
    return targets;
}

} // namespace core
