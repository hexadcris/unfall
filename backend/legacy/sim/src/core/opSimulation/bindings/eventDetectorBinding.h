/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  EventDetectorBinding.h
//! @brief This file contains the interface to the event detector libraries.
//-----------------------------------------------------------------------------

#pragma once

#include "common/callbacks.h"
#include "common/opExport.h"
#include "include/worldInterface.h"
#include "include/scenarioInterface.h"
#include "include/stochasticsInterface.h"

namespace core
{

class EventDetectorLibrary;
class EventDetector;
class EventNetworkInterface;
class FrameworkConfig;

class SIMULATIONCOREEXPORT EventDetectorBinding
{
public:
    EventDetectorBinding(CallbackInterface *callbacks);
    EventDetectorBinding(const EventDetectorBinding&) = delete;
    EventDetectorBinding(EventDetectorBinding&&) = delete;
    EventDetectorBinding& operator=(const EventDetectorBinding&) = delete;
    EventDetectorBinding& operator=(EventDetectorBinding&&) = delete;
    virtual ~EventDetectorBinding();

    //-----------------------------------------------------------------------------
    //! Gets the event detector instance library either from the already stored libraries
    //! or create a new instance (which is then also stored), then create a new event
    //! detector using the provided parameters.
    //!
    //! @param[in]  libraryPath         Path to the library
    //! @param[in]  scenario            Scenario
    //! @param[in]  eventNetwork        Interface of the eventNetwork
    //! @param[in]  world               World instance
    //! @param[in]  stochastics         The stochastics object
    //! @return                         Vector of created EventDetectors from the library
    //-----------------------------------------------------------------------------
    std::vector<const EventDetector *> Instantiate(const std::string libraryPath,
                                             const ScenarioInterface *scenario,
                                             EventNetworkInterface *eventNetwork,
                                             WorldInterface *world,
                                             StochasticsInterface *stochastics);

    //-----------------------------------------------------------------------------
    //! Unloads the event detector binding by deleting all stored event detector libraries.
    //-----------------------------------------------------------------------------
    void Unload();

private:
    void ValidateEventDetector(EventDetector* eventDetector, std::vector<EventDetector *> *eventDetectorList,  std::string eventDetectorType);

    EventDetectorLibrary* library = nullptr;
    CallbackInterface *callbacks {nullptr};
};

} // namespace core


