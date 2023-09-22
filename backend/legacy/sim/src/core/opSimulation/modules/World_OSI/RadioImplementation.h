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
//! @file  RadioImplementation.h
//! @brief This file contains the implementation of RadioInterface.h
//-----------------------------------------------------------------------------

#pragma once

#include "include/radioInterface.h"
#include <vector>
#include <memory>

//! Single signal sent by a sender
struct RadioSignal
{
    double positionX;
    double positionY;
    double signalStrength;
    DetectedObject objectInformation;
};

class RadioImplementation : public RadioInterface
{
public:
    RadioImplementation() = default;

    //-----------------------------------------------------------------------------
    //! Broadcasts object-metadata and sensor informations to cloud.
    //! @param[in] positionX            x position of sender
    //! @param[in] postionY             y position of sender
    //! @param[in] signalStrength       signal strength of sender (transmitting power [W])
    //! @param[in] objectInformation    sensor data
    //-----------------------------------------------------------------------------
    void Send(double positionX, double postionY, double signalStrength, DetectedObject objectInformation) override;

    //-----------------------------------------------------------------------------
    //! Retrieve available information at current postion from cloud.
    //! @param[in] positionX   x position of receiver
    //! @param[in] positionY   y position of receiver
    //! @param[in] sensitivity   sensitivity of receiver (implemented as surface power density [W/m2])
    //! @return data of senders "visible" at current position
    //-----------------------------------------------------------------------------
    std::vector<DetectedObject> Receive(double positionX, double positionY, double sensitivity) override;

    //-----------------------------------------------------------------------------
    //! Resets the cloud for next simulation step
    //-----------------------------------------------------------------------------
    void Reset() override;

private:
    //-----------------------------------------------------------------------------
    //! Checks if sender is within proximity of receiver ("is visible").
    //! Physical model: isotropic radiator
    //! @param[in] signalStrength   signal strength of sender [W]
    //! @param[in] distance         distance between sender and receiver
    //! @param[in] sensitivity      sensitivity of receiver [W/m2]
    //! @return bool is sender within proximity
    //-----------------------------------------------------------------------------
    bool CanHearSignal(double signalStrength, double distance, double sensitivity);

    std::vector<RadioSignal> signalVector;
};
