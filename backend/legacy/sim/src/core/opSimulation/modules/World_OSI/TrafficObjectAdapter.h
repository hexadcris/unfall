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

#include "include/trafficObjectInterface.h"
#include "WorldObjectAdapter.h"
#include "Localization.h"

class TrafficObjectAdapter : public WorldObjectAdapter, public TrafficObjectInterface
{
private:
    bool isCollidable;
    const double zOffset;
    double laneDirection;
    const World::Localization::Localizer& localizer;

    mutable World::Localization::Result locateResult;
    mutable std::vector<GlobalRoadPosition> boundaryPoints;

    OpenDriveId openDriveId;

    void InitLaneDirection(double hdg);

public:
    TrafficObjectAdapter(const openpass::type::EntityId id,
                         OWL::Interfaces::WorldData& worldData,
                         const World::Localization::Localizer& localizer,
                         OWL::Primitive::AbsPosition position,
                         OWL::Primitive::Dimension dimension,
                         const double zOffset,
                         OWL::Primitive::AbsOrientation orientation,
                         const OpenDriveId odId);

    ObjectTypeOSI GetType() const override;
    double GetZOffset() const override;
    bool GetIsCollidable() const override;
    Common::Vector2d GetVelocity(ObjectPoint point = ObjectPointPredefined::Reference) const override;
    Common::Vector2d GetAcceleration(ObjectPoint point = ObjectPointPredefined::Reference) const override;
    double GetLaneDirection() const;
    bool Locate() override;
    void Unlocate() override;

    const RoadIntervals &GetTouchedRoads() const override;

    Common::Vector2d GetAbsolutePosition(const ObjectPoint& objectPoint) const override;

    const GlobalRoadPositions& GetRoadPosition(const ObjectPoint& point) const override;

    OpenDriveId GetOpenDriveId() const override;

    // object is not inteded to be copied or assigned
    TrafficObjectAdapter(const TrafficObjectAdapter&) = delete;
    TrafficObjectAdapter(TrafficObjectAdapter&&) = delete;
    TrafficObjectAdapter& operator=(const TrafficObjectAdapter&) = delete;
    TrafficObjectAdapter& operator=(TrafficObjectAdapter&&) = delete;
    ~TrafficObjectAdapter() override;
};


