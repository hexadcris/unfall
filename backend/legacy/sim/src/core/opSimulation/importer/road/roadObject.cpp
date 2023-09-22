/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "roadObject.h"
#include <exception>

RoadObjectType RoadObject::GetType() const
{
    return object.type;
}

double RoadObject::GetLength() const
{
    return object.length;
}

double RoadObject::GetWidth() const
{
    return object.width;
}

bool RoadObject::IsContinuous() const
{
    return object.continuous;
}

double RoadObject::GetHdg() const
{
    return object.hdg;
}

double RoadObject::GetHeight() const
{
    return object.height;
}

double RoadObject::GetPitch() const
{
    return object.pitch;
}

double RoadObject::GetRoll() const
{
    return object.roll;
}

std::string RoadObject::GetId() const
{
    return object.id;
}

double RoadObject::GetS() const
{
    return object.s;
}

double RoadObject::GetT() const
{
    return object.t;
}

double RoadObject::GetZOffset() const
{
    return object.zOffset;
}

bool RoadObject::IsValidForLane(int laneId) const
{
    return object.validity.all ||
      ( std::find( object.validity.lanes.begin(), object.validity.lanes.end(), laneId) != object.validity.lanes.end() );
}

std::string RoadObject::GetName() const
{
    return object.name;
}
