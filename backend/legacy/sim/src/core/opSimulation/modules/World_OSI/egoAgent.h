/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "include/agentInterface.h"
#include "include/worldInterface.h"
#include "include/egoAgentInterface.h"

class EgoAgent: public EgoAgentInterface
{
public:
    EgoAgent(const AgentInterface* agent, const WorldInterface* world) :
        agent(agent),
        world(world)
    {}

    const AgentInterface* GetAgent () const override;

    void SetRoadGraph(const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target) override;

    void Update() override;

    void UpdatePositionInGraph();

    bool HasValidRoute() const override;

    void SetNewTarget (size_t alternativeIndex) override;

    const std::string& GetRoadId() const override;

    double GetVelocity(VelocityScope velocityScope) const override;

    double GetVelocity(VelocityScope velocityScope, const WorldObjectInterface* object) const override;

    double GetDistanceToEndOfLane (double range, int relativeLane = 0) const override;

    double GetDistanceToEndOfLane(double range, int relativeLane, const LaneTypes &acceptableLaneTypes) const override;

    RelativeWorldView::Lanes GetRelativeLanes(double range, int relativeLane = 0, bool includeOncoming = true) const override;

    std::optional<int> GetRelativeLaneId(const WorldObjectInterface* object, ObjectPoint point) const override;

    [[deprecated]] RelativeWorldView::Roads GetRelativeJunctions(double range) const override;

    RelativeWorldView::Roads GetRelativeRoads(double range) const override;

    std::vector<const WorldObjectInterface*> GetObjectsInRange (double backwardRange, double forwardRange, int relativeLane = 0) const override;

    AgentInterfaces GetAgentsInRange (double backwardRange, double forwardRange, int relativeLane = 0) const override;

    std::vector<CommonTrafficSign::Entity> GetTrafficSignsInRange(double range, int relativeLane = 0) const override;

    std::vector<CommonTrafficSign::Entity> GetRoadMarkingsInRange(double range, int relativeLane = 0) const override;

    std::vector<CommonTrafficLight::Entity> GetTrafficLightsInRange(double range, int relativeLane = 0) const override;

    std::vector<LaneMarking::Entity> GetLaneMarkingsInRange(double range, Side side, int relativeLane = 0) const override;

    std::optional<double> GetDistanceToObject(const WorldObjectInterface* otherObject, const ObjectPoint &ownPoint, const ObjectPoint &otherPoint) const override;

    std::optional<double> GetNetDistance(const WorldObjectInterface* otherObject) const override;

    Obstruction GetObstruction(const WorldObjectInterface* otherObject, const std::vector<ObjectPoint> points) const override;

    double GetRelativeYaw () const override;

    double GetPositionLateral() const override;

    double GetLaneRemainder(Side side) const override;

    double GetLaneWidth(int relativeLane = 0) const override;

    std::optional<double> GetLaneWidth(double distance, int relativeLane = 0) const override;

    double GetLaneCurvature(int relativeLane = 0) const override;

    std::optional<double> GetLaneCurvature(double distance, int relativeLane = 0) const override;

    double GetLaneDirection(int relativeLane = 0) const override;

    std::optional<double> GetLaneDirection(double distance, int relativeLane = 0) const override;

    const std::optional<GlobalRoadPosition>& GetMainLocatePosition() const override;

    std::optional<GlobalRoadPosition> GetReferencePointPosition() const override;

    int GetLaneIdFromRelative (int relativeLaneId) const override;

    std::optional<Position> GetWorldPosition(double sDistance, double tDistance, double yaw = 0) const override;

    GlobalRoadPositions GetRoadPositions(const ObjectPoint& point, const WorldObjectInterface* object) const override;

private:
    std::optional<RouteElement> GetPreviousRoad(size_t steps = 1) const;

    std::optional<GlobalRoadPosition> GetReferencePointPosition(const WorldObjectInterface *object) const;

    std::optional<RoadGraphVertex> GetReferencePointVertex() const;

    std::optional<GlobalRoadPosition> GetPositionOnRoute(GlobalRoadPositions roadPositions) const;

    ExecuteReturn<DistanceToEndOfLane> executeQueryDistanceToEndOfLane(DistanceToEndOfLaneParameter parameter) const override;

    ExecuteReturn<ObjectsInRange> executeQueryObjectsInRange(ObjectsInRangeParameter parameter) const override;

    void SetWayToTarget(RoadGraphVertex targetVertex);

    const AgentInterface* agent;
    const WorldInterface* world;
    bool graphValid{false};
    RoadGraph roadGraph{};
    RoadGraphVertex current{0};
    RoadGraph wayToTarget{};
    RoadGraphVertex rootOfWayToTargetGraph{0};
    std::vector<RouteElement> wayToTargetRoute{}; //! same information as wayToTarget but as vector
    std::vector<RoadGraphVertex> alternatives{};
    std::optional<GlobalRoadPosition> mainLocatePosition;
};
