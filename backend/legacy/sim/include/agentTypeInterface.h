/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentType.h
//! @brief This file contains the internal representation of an agent type as
//!        given by the configuration.
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>

namespace core
{

class ComponentType;
class ChannelType;

class AgentTypeInterface
{
public:
    AgentTypeInterface() = default;
    AgentTypeInterface(int id, int priority);
    AgentTypeInterface(const AgentTypeInterface&) = default;
    AgentTypeInterface(AgentTypeInterface&&) = default;
    AgentTypeInterface& operator=(const AgentTypeInterface&) = default;
    AgentTypeInterface& operator=(AgentTypeInterface&&) = default;
    virtual ~AgentTypeInterface() = default;

    virtual bool AddChannel(int id) = 0;
    virtual bool AddComponent(std::shared_ptr<ComponentType> component) = 0;

    //Getter
    virtual const std::vector<int> &GetChannels() const = 0;
    virtual const std::map<std::string, std::shared_ptr<ComponentType>> &GetComponents() const = 0;
};

} // namespace core
