/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  acquirePositionEvent.h
* @brief This file contains all functions for AcquirePositionEvents.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------

#pragma once

#include <src/common/openScenarioDefinitions.h>
#include <utility>

#include "basicEvent.h"
namespace openpass::events {

class AcquirePositionEvent : public OpenScenarioEvent
{
public:
    AcquirePositionEvent(int time,
                         const std::string &eventName,
                         const std::string &source,
                         int agentId,
                         openScenario::Position position) :
        OpenScenarioEvent(time, eventName, source, {}, {{agentId}}), position(std::move(position))
    {
    }

    static constexpr char TOPIC[]{"OpenSCENARIO/Position"};
    const openScenario::Position position;
};

} // namespace openpass::events
