/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/opMath.h"
#include <exception>
#include <qglobal.h>

#include "TrafficObjectAdapter.h"

//TODO: replace GlobalObject with injected shared_pointer to locator

TrafficObjectAdapter::TrafficObjectAdapter(const openpass::type::EntityId id,
                                           OWL::Interfaces::WorldData& worldData,
                                           const World::Localization::Localizer& localizer,
                                           OWL::Primitive::AbsPosition position,
                                           OWL::Primitive::Dimension dimension,
                                           const double zOffset,
                                           OWL::Primitive::AbsOrientation orientation,
                                           const OpenDriveId odId) :
    WorldObjectAdapter{worldData.AddStationaryObject(id.value, static_cast<void*>(static_cast<WorldObjectInterface*>(this)))}, // objects passed as void * need to be casted to WorldObjectInterface*, because they are retrieved by casting to that class
    zOffset{zOffset},
    localizer{localizer},
    openDriveId{odId}
{
    baseTrafficObject.SetReferencePointPosition(position);
    baseTrafficObject.SetDimension(dimension);
    baseTrafficObject.SetAbsOrientation(orientation);
    InitLaneDirection(orientation.yaw);
    Locate();
}

ObjectTypeOSI TrafficObjectAdapter::GetType() const
{
    return ObjectTypeOSI::Object;
}

double TrafficObjectAdapter::GetZOffset() const
{
    return zOffset;
}

bool TrafficObjectAdapter::GetIsCollidable() const
{
    return (GetHeight() > 0 && GetLength() > 0 && GetWidth() > 0);
}

void TrafficObjectAdapter::InitLaneDirection(double hdg)
{
    laneDirection = GetYaw() - hdg;
}


Common::Vector2d TrafficObjectAdapter:: GetVelocity(ObjectPoint point) const
{
    Q_UNUSED(point);

    //TrafficObjects don't move
    return {0.0, 0.0};
}

Common::Vector2d TrafficObjectAdapter:: GetAcceleration(ObjectPoint point) const
{
    Q_UNUSED(point);

    //TrafficObjects don't move
    return {0.0, 0.0};
}

double TrafficObjectAdapter::GetLaneDirection() const
{
    return laneDirection;
}

bool TrafficObjectAdapter::Locate()
{
    // reset on-demand values
    boundaryPoints.clear();

    locateResult = localizer.Locate(GetBoundingBox2D(), baseTrafficObject);

    return true;
}

void TrafficObjectAdapter::Unlocate()
{
    localizer.Unlocate(baseTrafficObject);
}

const RoadIntervals &TrafficObjectAdapter::GetTouchedRoads() const
{
    return locateResult.touchedRoads;
}

Common::Vector2d TrafficObjectAdapter::GetAbsolutePosition(const ObjectPoint &objectPoint) const
{
    double longitudinal;
    double lateral;
    if (std::holds_alternative<ObjectPointCustom>(objectPoint))
    {
        longitudinal = std::get<ObjectPointCustom>(objectPoint).longitudinal;
        lateral = std::get<ObjectPointCustom>(objectPoint).lateral;
    }
    else if (std::holds_alternative<ObjectPointPredefined>(objectPoint))
    {
        switch (std::get<ObjectPointPredefined>(objectPoint))
        {
        case ObjectPointPredefined::Reference:
        case ObjectPointPredefined::Center:
            longitudinal = 0;
            lateral = 0;
            break;
        case ObjectPointPredefined::FrontCenter:
            longitudinal = 0.5 * GetLength();
            lateral = 0;
            break;
        case ObjectPointPredefined::RearCenter:
            longitudinal = -0.5 * GetLength();
            lateral = 0;
            break;
        case ObjectPointPredefined::FrontLeft:
            longitudinal = 0.5 * GetLength();
            lateral = 0.5 * GetWidth();
            break;
        case ObjectPointPredefined::FrontRight:
            longitudinal = 0.5 * GetLength();
            lateral = -0.5 * GetWidth();
            break;
        case ObjectPointPredefined::RearLeft:
            longitudinal = -0.5 * GetLength();
            lateral = 0.5 * GetWidth();
            break;
        case ObjectPointPredefined::RearRight:
            longitudinal = -0.5 * GetLength();
            lateral = -0.5 * GetWidth();
            break;
        }
    }
    else
    {
        throw std::runtime_error("Unknown type of ObjectPoint");
    }
    const auto& referencePoint = baseTrafficObject.GetReferencePointPosition();
    const auto& yaw = baseTrafficObject.GetAbsOrientation().yaw;
    double x = referencePoint.x + std::cos(yaw) * longitudinal - std::sin(yaw) * lateral;
    double y = referencePoint.y + std::sin(yaw) * longitudinal + std::cos(yaw) * lateral;
    return {x,y};
}

const GlobalRoadPositions &TrafficObjectAdapter::GetRoadPosition(const ObjectPoint& point) const
{
    auto position = locateResult.points.find(point);
    if (position != locateResult.points.cend())
    {
        return position->second;
    }
    Common::Vector2d globalPoint = GetAbsolutePosition(point);
    auto locatedPoint = localizer.Locate(globalPoint, GetYaw());
    auto [newElement, success] = locateResult.points.insert({point, locatedPoint});
    return newElement->second;
}

OpenDriveId TrafficObjectAdapter::GetOpenDriveId() const
{
    return openDriveId;
}

TrafficObjectAdapter::~TrafficObjectAdapter()
{
    Unlocate();
}
