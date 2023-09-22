/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "RadioImplementation.h"
#include <math.h>

void RadioImplementation::Send(double positionX, double postionY, double signalStrength, DetectedObject objectInformation)
{
    RadioSignal radioSignal{positionX, postionY, signalStrength, objectInformation};
    signalVector.push_back(radioSignal);
}

std::vector<DetectedObject> RadioImplementation::Receive(double positionX, double positionY, double sensitivity)
{
    std::vector<DetectedObject> detectedObjects{};
    for (RadioSignal radioSignal : signalVector)
    {
        double deltaX= radioSignal.positionX - positionX;
        double deltaY=radioSignal.positionY-positionY;
        double distance=sqrt(deltaX*deltaX+deltaY*deltaY);
        if (CanHearSignal(radioSignal.signalStrength,distance,sensitivity))
        {

            detectedObjects.push_back(radioSignal.objectInformation);
        }
    }
    return detectedObjects;
}

bool RadioImplementation::CanHearSignal(double signalStrength, double distance, double sensitivity)
{
    double receivedSignalStrength = signalStrength / ( 4 * M_PI * distance * distance );
    return receivedSignalStrength >= sensitivity;
}

void RadioImplementation::Reset()
{
    signalVector.clear();
}
