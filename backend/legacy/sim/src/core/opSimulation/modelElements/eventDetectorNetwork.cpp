/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "eventDetector.h"
#include "bindings/eventDetectorBinding.h"
#include "eventDetectorNetwork.h"
#include "common/log.h"

namespace core {

EventDetectorNetwork::EventDetectorNetwork(EventDetectorBinding* eventDetectorBinding,
        WorldInterface* world):
    eventDetectorBinding(eventDetectorBinding),
    world(world)
{
}

EventDetectorNetwork::~EventDetectorNetwork()
{
    Clear();
}

bool EventDetectorNetwork::Instantiate(const std::string libraryPath,
                                       const ScenarioInterface* scenario,
                                       EventNetworkInterface* eventNetwork,
                                       StochasticsInterface* stochastics)
{
    if (!scenario)
    {
        return false;
    }

    //Instantiate all detectors
    try
    {
        eventDetectors = eventDetectorBinding->Instantiate(libraryPath,
                         scenario,
                         eventNetwork,
                         world,
                         stochastics);
    }
    catch (const std::runtime_error& error)
    {
        LOG_INTERN(LogLevel::Error) << "Could not instantiate all EventDetectors: " << error.what();
        return false;
    }

    return true;
}

void EventDetectorNetwork::Clear()
{
    for (auto eventDetector : eventDetectors)
    {
        delete eventDetector;
        eventDetector = nullptr;
    }
}

const std::vector<const EventDetector*> EventDetectorNetwork::GetEventDetectors() const
{
    return eventDetectors;
}

void EventDetectorNetwork::ResetAll()
{
    for (const auto eventDetector : GetEventDetectors())
    {
        eventDetector->GetImplementation()->Reset();
    }
}

} // namespace core
