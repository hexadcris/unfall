/********************************************************************************
 * Copyright (c) 2016 ITK Engineering GmbH
 *               2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentFactoryInterface.h
//! @brief This file contains the interface for communicating between framework
//!        and world.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <list>

#include "include/agentBlueprintInterface.h"

/**
* \brief Agent Interface within the openPASS framework.
* \details This interface provides access to agent parameters, properties, attributes and dynamic states.
*/

namespace core {

class Agent;

class AgentFactoryInterface
{
public:
    AgentFactoryInterface() = default;
    virtual ~AgentFactoryInterface() = default;

    //-----------------------------------------------------------------------------
    //! Clear factory for next simulation cycle
    //-----------------------------------------------------------------------------
    virtual void Clear() = 0;

    //-----------------------------------------------------------------------------
    //! Creates a new agent based on the provided parameters, then adds it to the
    //! agent network in the world representation. Also adds agents during runtime.
    //!
    //! @param[in]  agentBlueprint      agentBlueprint contains all necessary
    //!                                 informations to create an agent
    //!
    //! @return                         The added agent
    //-----------------------------------------------------------------------------
    virtual Agent *AddAgent(AgentBlueprintInterface* agentBlueprint) = 0;
};

} //namespace core
