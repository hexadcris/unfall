/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/opExport.h"
#include "include/eventDetectorNetworkInterface.h"
#include "include/worldInterface.h"

namespace core
{

//Forward declarations
class EventDetector;
class EventDetectorBinding;
class EventNetworkInterface;

class SIMULATIONCOREEXPORT EventDetectorNetwork : public EventDetectorNetworkInterface
{
public:
    EventDetectorNetwork(EventDetectorBinding *eventDetectorBinding,
                         WorldInterface *world);
    EventDetectorNetwork(const EventDetectorNetwork&) = delete;
    EventDetectorNetwork(EventDetectorNetwork&&) = delete;
    EventDetectorNetwork& operator=(const EventDetectorNetwork&) = delete;
    EventDetectorNetwork& operator=(EventDetectorNetwork&&) = delete;
    virtual ~EventDetectorNetwork();

    //-----------------------------------------------------------------------------
    //! Instantiates the eventdetector network.
    //!
    //! @param[in]  libraryPath         Path to the library
    //! @param[in]  scenario            Scenario
    //! @param[in]  eventNetwork        Interface of the eventNetwork
    //! @param[in]  stochastics         The stochastics object
    //! @return                         true, if successful
    //-----------------------------------------------------------------------------
    bool Instantiate(const std::string libraryPath,
                     const ScenarioInterface *scenario,
                     EventNetworkInterface* eventNetwork,
                     StochasticsInterface *stochastics);

    //-----------------------------------------------------------------------------
    //! Clears all eventdetectors
    //-----------------------------------------------------------------------------
    void Clear();

    //-----------------------------------------------------------------------------
    //! Returns all eventdetectors
    //!
    //! @return     Vector of created EventDetectors from the library
    //-----------------------------------------------------------------------------
    const std::vector<const EventDetector*> GetEventDetectors() const;

    //-----------------------------------------------------------------------------
    //! Resets all eventdetectors to their initial state.
    //-----------------------------------------------------------------------------
    void ResetAll();

private:

    EventDetectorBinding *eventDetectorBinding;
    std::vector<const EventDetector*> eventDetectors;
    WorldInterface *world;
};

}// namespace core


