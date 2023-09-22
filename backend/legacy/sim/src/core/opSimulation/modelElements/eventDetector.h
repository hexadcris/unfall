/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  EventDetector.h
//! @brief This file contains the internal representation of the a event detector.
//-----------------------------------------------------------------------------

#pragma once

#include <list>

#include "modelElements/parameters.h"
#include "include/eventDetectorInterface.h"
#include "bindings/eventDetectorLibrary.h"
#include "include/worldInterface.h"
#include "common/log.h"

namespace core
{

class EventDetector
{
public:
    EventDetector(EventDetectorInterface *implementation,
                  EventDetectorLibrary *library) :
        library(library),
        implementation(implementation)
    {
        LOG_INTERN(LogLevel::DebugCore) << "created event detector ";
    }
    EventDetector(const EventDetector&) = delete;
    EventDetector(EventDetector&&) = delete;
    EventDetector& operator=(const EventDetector&) = delete;
    EventDetector& operator=(EventDetector&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, releases the event detector from the stored library.
    //-----------------------------------------------------------------------------
    virtual ~EventDetector()
    {
        library->ReleaseEventDetector(this);
        LOG_INTERN(LogLevel::DebugCore) << "destroyed event detector ";
    }

    //-----------------------------------------------------------------------------
    //! Returns the event detector interface.
    //!
    //! @return                         event detector interface
    //-----------------------------------------------------------------------------
    EventDetectorInterface *GetImplementation() const
    {
        return implementation;
    }

    //-----------------------------------------------------------------------------
    //! Returns the event detector library.
    //!
    //! @return                         event detector library
    //-----------------------------------------------------------------------------
    EventDetectorLibrary *GetLibrary()
    {
        return library;
    }

private:
    EventDetectorLibrary *library;
    EventDetectorInterface *implementation;
};

} // namespace core


