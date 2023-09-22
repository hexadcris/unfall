/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <algorithm>
#include <iostream>
#include <QDomDocument>
#include <QFile>

#include "agentType.h"
#include "componentType.h"
#include "common/log.h"
#include "modelElements/parameters.h"

namespace core
{

bool AgentType::AddComponent(std::shared_ptr<ComponentType> component)
{
    if(!components.insert({component->GetName(), component}).second)
    {
        LOG_INTERN(LogLevel::Warning) << "Componentname does already exist:" << component->GetName();
    }

    return true;
}


bool AgentType::AddChannel(int id)
{
    auto findIter = std::find(channels.begin(), channels.end(), id);
    if(channels.end() != findIter)
    {
        LOG_INTERN(LogLevel::Warning) << "components must be unique (channel id " << std::to_string(*findIter) << ")";
        return false;
    }

    channels.push_back(id);
    return true;
}

const std::vector<int> &AgentType::GetChannels() const
{
    return channels;
}

const std::map<std::string, std::shared_ptr<ComponentType> > &AgentType::GetComponents() const
{
    return components;
}

} // namespace core
