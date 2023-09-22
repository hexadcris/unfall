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

#include "common/globalDefinitions.h"
#include "common/boostGeometryCommon.h"
#include "common/worldDefinitions.h"
#include <unordered_map>

class WorldObjectInterface;
namespace Common
{
class Vector2d;
}

class WorldObjectInterface
{
public:
    virtual ObjectTypeOSI GetType() const = 0;

    virtual double GetPositionX() const = 0;
    virtual double GetPositionY() const = 0;

    /// \brief Retrieves width of the boundary box
    /// \return width
    virtual double GetWidth() const = 0;

    /// \brief Retrieves length of the boundary box
    /// \return length
    virtual double GetLength() const = 0;

    virtual double GetHeight() const = 0;

    /// \brief Retrieves yaw angle w.r.t. x-axis
    /// \return yaw
    virtual double GetYaw() const = 0;

    /// \brief Retrieves roll angle
    /// \return roll
    virtual double GetRoll() const = 0;

    /// \brief  Get unique id of an object
    /// \return id
    virtual int GetId() const = 0;

    virtual const polygon_t& GetBoundingBox2D() const = 0;

    /// \brief  Get roads on which the object is on
    /// \return touched road intervals
    virtual const RoadIntervals &GetTouchedRoads() const = 0;

    /// \brief  Returns the world position of the given point
    /// \param  point point to locate (must not be a of type ObjectPointRelative)
    /// \return world position
    virtual Common::Vector2d GetAbsolutePosition(const ObjectPoint& point) const = 0;

    /// \brief  Returns the road position(s) of the given point
    /// \param  point point to locate (must not be a of type ObjectPointRelative)
    /// \return road positions
    virtual const GlobalRoadPositions& GetRoadPosition(const ObjectPoint& point) const = 0;

    virtual double GetDistanceReferencePointToLeadingEdge() const = 0;

    /// \brief  Returns the objects velocity at the given point
    /// \param  point point where to get the velocity (must not be a of type ObjectPointRelative)
    /// \return velocity
    virtual Common::Vector2d GetVelocity(ObjectPoint point = ObjectPointPredefined::Reference) const = 0;

    /// \brief  Returns the objects acceleration at the given point
    /// \param  point point where to get the acceleration (must not be a of type ObjectPointRelative)
    /// \return acceleration
    virtual Common::Vector2d GetAcceleration(ObjectPoint point = ObjectPointPredefined::Reference) const = 0;

    virtual bool Locate() = 0;
    virtual void Unlocate() = 0;

    // object is not inteded to be copied or assigned
    WorldObjectInterface() = default;
    WorldObjectInterface(const WorldObjectInterface&) = delete;
    WorldObjectInterface(WorldObjectInterface&&) = delete;
    WorldObjectInterface& operator=(const WorldObjectInterface&) = delete;
    WorldObjectInterface& operator=(WorldObjectInterface&&) = delete;
    virtual ~WorldObjectInterface() = default;
};

