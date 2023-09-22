/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "include/eventNetworkInterface.h"
#include "include/scenarioInterface.h"
#include "include/stochasticsInterface.h"

namespace core
{

//Forward declarations
class EventDetector;

class EventDetectorNetworkInterface
{
public:
    EventDetectorNetworkInterface() = default;
    ~EventDetectorNetworkInterface() = default;

    virtual bool Instantiate(const std::string libraryPath,
                     const ScenarioInterface *scenario,
                     EventNetworkInterface* eventNetwork,
                     StochasticsInterface *stochastics) = 0;

    virtual void Clear() = 0;

    virtual const std::vector<const EventDetector*> GetEventDetectors() const = 0;

    virtual void ResetAll() = 0;
};

}// namespace core


