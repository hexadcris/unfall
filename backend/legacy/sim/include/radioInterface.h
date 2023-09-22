/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  RadioInterface.h
//! @brief This is a utility class for Car2X communication. Senders register themselves every timestep by calling the send function.
//!        After that the receivers may call the receive function to get all the senders around them. The radio cloud implementation
//!        is responsible for the calculation which signals a receiver can hear.
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include "common/DetectedObject.h"

class RadioInterface
{
public:
    RadioInterface() = default;
    RadioInterface(const RadioInterface&) = delete;
    RadioInterface(RadioInterface&&) = delete;
    RadioInterface& operator=(const RadioInterface&) = delete;
    RadioInterface& operator=(RadioInterface&&) = delete;
    virtual ~RadioInterface() = default;

    //! Broadcast information and position of the vehicle
    //!
    //! @param[in] positionX    x-position of the sender
    //! @param[in] positionY    y-position of the sender
    //! @param[in] signalStrength   signal strength of the antenna
    //! @param[in] objectInformation    information to broadcast
    virtual void Send(double positionX, double postionY, double signalStrength, DetectedObject objectInformation) = 0;

    //! Call the cloud to return all the information available at a position
    //!
    //! @param[in] positionX    x-position of the receiver
    //! @param[in] positionY    y-position of the receiver
    //! @param[in] sensitivity  how strong the signal needs to be for the receiver to read the signal
    //! @return     list of all the information which can be received at this position
    virtual std::vector<DetectedObject> Receive(double positionX, double positionY, double sensitivity) = 0;

    //! For each new timestep this function clears all signal of the previous timestep
    virtual void Reset() = 0;
};
