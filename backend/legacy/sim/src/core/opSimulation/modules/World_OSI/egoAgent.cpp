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

#include "egoAgent.h"

const AgentInterface* EgoAgent::GetAgent() const
{
    return agent;
}

void EgoAgent::SetRoadGraph(const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target)
{
    graphValid = true;
    this->roadGraph = roadGraph;
    this->current = current;
    alternatives.clear();
    for (auto [vertex, verticesEnd] = vertices(roadGraph); vertex != verticesEnd; ++vertex)
    {
        if (out_degree(*vertex, roadGraph) == 0) //The alternatives are the leaves of the graph
        {
            alternatives.push_back(*vertex);
        }
    }
    SetWayToTarget(target);
    Update();
}

void EgoAgent::Update()
{
    UpdatePositionInGraph();
    if (graphValid)
    {
        //buffer mainLocatePosition to prevent multiple map lookups (yields better performance)
        mainLocatePosition = agent->GetRoadPosition(ObjectPointPredefined::FrontCenter).at(GetRoadId());
    }
    else
    {
        mainLocatePosition = std::nullopt;
    }
}

void EgoAgent::UpdatePositionInGraph()
{
    if (!graphValid)
    {
        return;
    }
    auto roadIds = GetAgent()->GetRoads(ObjectPointPredefined::FrontCenter);
    while (std::find(roadIds.cbegin(), roadIds.cend(), GetRoadId()) == roadIds.end())
    {
        if (rootOfWayToTargetGraph > 0)
        {
            rootOfWayToTargetGraph--;
            auto routeElement = get(RouteElement(), wayToTarget, rootOfWayToTargetGraph);
            auto [successorBegin, successorsEnd] = adjacent_vertices(current, roadGraph);
                    auto successor = std::find_if(successorBegin, successorsEnd, [&](const auto& vertex){return get(RouteElement(), roadGraph, vertex) == routeElement;});
            if (successor == successorsEnd)
            {
                graphValid = false;
                return;
            }
            current = *successor;
            if (std::find(roadIds.cbegin(), roadIds.cend(), routeElement.roadId) == roadIds.end())
            {
                continue;
            }
        }
        else
        {
            graphValid = false;
            return;
        }
    }
}

bool EgoAgent::HasValidRoute() const
{
    return graphValid;
}

void EgoAgent::SetNewTarget(size_t alternativeIndex)
{
    SetWayToTarget(alternatives[alternativeIndex]);
}

const std::string& EgoAgent::GetRoadId() const
{
    return get(RouteElement(), roadGraph, current).roadId;
}

double EgoAgent::GetVelocity(VelocityScope velocityScope) const
{
    return GetVelocity(velocityScope, agent);
}

double EgoAgent::GetVelocity(VelocityScope velocityScope, const WorldObjectInterface *object) const
{
    if (velocityScope == VelocityScope::Absolute)
    {
        return object->GetVelocity().Length();
    }
    else if (velocityScope == VelocityScope::Lateral)
    {
        const auto referencePoint = GetReferencePointPosition();
        if (!referencePoint.has_value())
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return object->GetVelocity().Projection(agent->GetYaw() + M_PI_2);
    }
    else if (velocityScope == VelocityScope::Longitudinal)
    {
        const auto referencePoint = GetReferencePointPosition();
        if (!referencePoint.has_value())
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return object->GetVelocity().Projection(agent->GetYaw());
    }

    throw std::invalid_argument("velocity scope not within valid bounds");
}

double EgoAgent::GetDistanceToEndOfLane(double range, int relativeLane) const
{
    if (!graphValid)
    {
        return NAN;
    }
    return world->GetDistanceToEndOfLane(wayToTarget,
                                         rootOfWayToTargetGraph,
                                         GetLaneIdFromRelative(relativeLane),
                                         GetMainLocatePosition().value().roadPosition.s,
                                         range).at(0);
}

double EgoAgent::GetDistanceToEndOfLane(double range, int relativeLane, const LaneTypes& acceptableLaneTypes) const
{
    if (!graphValid)
    {
        return NAN;
    }
    return world->GetDistanceToEndOfLane(wayToTarget,
                                         rootOfWayToTargetGraph,
                                         GetLaneIdFromRelative(relativeLane),
                                         GetMainLocatePosition().value().roadPosition.s,
                                         range, acceptableLaneTypes).at(0);
}

RelativeWorldView::Lanes EgoAgent::GetRelativeLanes(double range, int relativeLane, bool includeOncoming) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetRelativeLanes(wayToTarget,
                                   rootOfWayToTargetGraph,
                                   GetLaneIdFromRelative(relativeLane),
                                   GetMainLocatePosition().value().roadPosition.s,
                                   range,
                                   includeOncoming).at(0);
}

std::optional<int> EgoAgent::GetRelativeLaneId(const WorldObjectInterface *object, ObjectPoint point) const
{
    if (!graphValid)
    {
        return {};
    }
    GlobalRoadPositions objectPosition = GetRoadPositions(point, object);
    return world->GetRelativeLaneId(wayToTarget,
                                    rootOfWayToTargetGraph,
                                    GetMainLocatePosition().value().laneId,
                                    GetMainLocatePosition().value().roadPosition.s,
                                    objectPosition).at(0);
}

std::vector<const WorldObjectInterface*> EgoAgent::GetObjectsInRange(double backwardRange, double forwardRange, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    auto objectsInRange = world->GetObjectsInRange(wayToTarget,
                                                   rootOfWayToTargetGraph,
                                                   GetLaneIdFromRelative(relativeLane),
                                                   GetMainLocatePosition().value().roadPosition.s,
                                                   backwardRange,
                                                   forwardRange).at(0);

    auto self = std::find(objectsInRange.cbegin(), objectsInRange.cend(), GetAgent());

    if (self != objectsInRange.cend())
    {
        objectsInRange.erase(self);
    }

    return objectsInRange;
}

AgentInterfaces EgoAgent::GetAgentsInRange(double backwardRange, double forwardRange, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    auto agentsInRange =  world->GetAgentsInRange(wayToTarget,
                                                  rootOfWayToTargetGraph,
                                                  GetLaneIdFromRelative(relativeLane),
                                                  GetMainLocatePosition().value().roadPosition.s,
                                                  backwardRange,
                                                  forwardRange).at(0);

    auto self = std::find(agentsInRange.cbegin(), agentsInRange.cend(), GetAgent());

    if (self != agentsInRange.cend())
    {
        agentsInRange.erase(self);
    }

    return agentsInRange;
}

std::vector<CommonTrafficSign::Entity> EgoAgent::GetTrafficSignsInRange(double range, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetTrafficSignsInRange(wayToTarget,
                                         rootOfWayToTargetGraph,
                                         GetLaneIdFromRelative(relativeLane),
                                         GetMainLocatePosition().value().roadPosition.s,
                                         range).at(0);
}

std::vector<CommonTrafficSign::Entity> EgoAgent::GetRoadMarkingsInRange(double range, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetRoadMarkingsInRange(wayToTarget,
                                         rootOfWayToTargetGraph,
                                         GetLaneIdFromRelative(relativeLane),
                                         GetMainLocatePosition().value().roadPosition.s,
                                         range).at(0);
}

std::vector<CommonTrafficLight::Entity> EgoAgent::GetTrafficLightsInRange(double range, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetTrafficLightsInRange(wayToTarget,
                                          rootOfWayToTargetGraph,
                                          GetLaneIdFromRelative(relativeLane),
                                          GetMainLocatePosition().value().roadPosition.s,
                                          range).at(0);
}

std::vector<LaneMarking::Entity> EgoAgent::GetLaneMarkingsInRange(double range, Side side, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetLaneMarkings(wayToTarget,
                                  rootOfWayToTargetGraph,
                                  GetLaneIdFromRelative(relativeLane),
                                  GetMainLocatePosition().value().roadPosition.s,
                                  range,
                                  side).at(0);
}

std::optional<double> EgoAgent::GetDistanceToObject(const WorldObjectInterface* otherObject, const ObjectPoint& ownPoint, const ObjectPoint& otherPoint) const
{
    if (!otherObject)
    {
        return {};
    }

    const auto ownPosition = GetPositionOnRoute(GetRoadPositions(ownPoint, agent));
    if (!ownPosition.has_value())
    {
        return {};
    }

    const auto& otherObjectPos = GetRoadPositions(otherPoint, otherObject);
    const auto distance = world->GetDistanceBetweenObjects(wayToTarget, rootOfWayToTargetGraph,
                                                           ownPosition.value().roadPosition.s,
                                                           otherObjectPos).at(0);

    return distance;
}

std::optional<double> EgoAgent::GetNetDistance(const WorldObjectInterface *otherObject) const
{
    const auto front = GetDistanceToObject(otherObject, ObjectPointRelative::Frontmost, ObjectPointRelative::Rearmost);
    const auto rear = GetDistanceToObject(otherObject, ObjectPointRelative::Rearmost, ObjectPointRelative::Frontmost);
    if (front.has_value() && front.value() >= 0.0)
    {
        return front;
    }
    if (rear.has_value() && rear.value() <= 0.0)
    {
        return rear;
    }
    if (front.has_value() || rear.has_value())
    {
        return 0.0;
    }
    return std::nullopt;
}

Obstruction EgoAgent::GetObstruction(const WorldObjectInterface* otherObject, const std::vector<ObjectPoint> points) const
{
    if (!HasValidRoute())
    {
        return Obstruction::Invalid();
    }

    std::map<ObjectPoint, Common::Vector2d> absolutePoints{};
    for (const auto& point : points)
    {
        if (std::holds_alternative<ObjectPointRelative>(point))
        {
            const auto roadPosition = world->ResolveRelativePoint(wayToTarget, rootOfWayToTargetGraph, std::get<ObjectPointRelative>(point), *otherObject).at(0);
            if (roadPosition.has_value())
            {
                const auto worldPosition = world->LaneCoord2WorldCoord(roadPosition.value().roadPosition.s, roadPosition.value().roadPosition.t, roadPosition.value().roadId, roadPosition.value().laneId);
                absolutePoints[point] = {worldPosition.xPos, worldPosition.yPos};
            }
        }
        else
        {
            absolutePoints[point] = otherObject->GetAbsolutePosition(point);
        }
    }
    return world->GetObstruction(wayToTarget,
                                 rootOfWayToTargetGraph,
                                 agent->GetRoadPosition(ObjectPointPredefined::FrontCenter).at(GetRoadId()),
                                 absolutePoints,
                                 otherObject->GetTouchedRoads()).at(0);
}

double EgoAgent::GetRelativeYaw() const
{
    if (!graphValid)
    {
        return NAN;
    }
    if (get(RouteElement(), roadGraph, current).inOdDirection)
    {
        return GetMainLocatePosition().value().roadPosition.hdg;
    }
    else
    {
        return std::fmod(GetMainLocatePosition().value().roadPosition.hdg + 2 * M_PI, 2 * M_PI) - M_PI;
    }
}

double EgoAgent::GetPositionLateral() const
{
    if (!graphValid)
    {
        return NAN;
    }
    return get(RouteElement(), roadGraph, current).inOdDirection ? GetMainLocatePosition().value().roadPosition.t : -GetMainLocatePosition().value().roadPosition.t;
}

double EgoAgent::GetLaneRemainder(Side side) const
{
    const auto& roadId = GetRoadId();
    return side == Side::Left ? 0.5 * GetLaneWidth(0) - agent->GetTouchedRoads().at(roadId).tMax.roadPosition.t
                              : 0.5 * GetLaneWidth(0) + agent->GetTouchedRoads().at(roadId).tMin.roadPosition.t;
}

double EgoAgent::GetLaneWidth(int relativeLane) const
{
    if (!graphValid)
    {
        return NAN;
    }
    return world->GetLaneWidth(GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().value().roadPosition.s);
}

std::optional<double> EgoAgent::GetLaneWidth(double distance, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetLaneWidth(wayToTarget, rootOfWayToTargetGraph, GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().value().roadPosition.s, distance).at(0);
}

double EgoAgent::GetLaneCurvature(int relativeLane) const
{
    if (!graphValid)
    {
        return NAN;
    }
    return world->GetLaneCurvature(GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().value().roadPosition.s);
}

std::optional<double> EgoAgent::GetLaneCurvature(double distance, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetLaneCurvature(wayToTarget, rootOfWayToTargetGraph, GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().value().roadPosition.s, distance).at(0);
}

double EgoAgent::GetLaneDirection(int relativeLane) const
{
    if (!graphValid)
    {
        return NAN;
    }
    return world->GetLaneDirection(GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().value().roadPosition.s);
}

std::optional<double> EgoAgent::GetLaneDirection(double distance, int relativeLane) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetLaneDirection(wayToTarget, rootOfWayToTargetGraph, GetLaneIdFromRelative(relativeLane), GetMainLocatePosition().value().roadPosition.s, distance).at(0);
}

const std::optional<GlobalRoadPosition>& EgoAgent::GetMainLocatePosition() const
{
    return mainLocatePosition;
}

std::optional<GlobalRoadPosition> EgoAgent::GetReferencePointPosition(const WorldObjectInterface* object) const
{
    return GetPositionOnRoute(object->GetRoadPosition(ObjectPointPredefined::Reference));
}

std::optional<GlobalRoadPosition> EgoAgent::GetReferencePointPosition() const
{
    return GetReferencePointPosition(agent);
}

std::optional<RoadGraphVertex> EgoAgent::GetReferencePointVertex() const
{
    auto referencePoint = agent->GetRoadPosition(ObjectPointPredefined::Reference);
    for (size_t steps = 0; rootOfWayToTargetGraph + steps < num_vertices(wayToTarget); ++steps)
    {
        auto routeElement = get(RouteElement(), wayToTarget, rootOfWayToTargetGraph + steps);
        auto referencePointPosition = referencePoint.find(routeElement.roadId);
        if (referencePointPosition != referencePoint.end())
        {
            return rootOfWayToTargetGraph + steps;
        }
    }
    return std::nullopt;
}


std::optional<GlobalRoadPosition> EgoAgent::GetPositionOnRoute(GlobalRoadPositions roadPositions) const
{
    auto position = roadPositions.find(GetRoadId());
    if (position != roadPositions.end())
    {
        return position->second;
    }
    size_t steps = 1;
    while(auto routeElement = GetPreviousRoad(steps))
    {
        position = roadPositions.find(routeElement.value().roadId);
        if (position != roadPositions.end())
        {
            return position->second;
        }
        steps++;
    }
    return std::nullopt;
}

int EgoAgent::GetLaneIdFromRelative(int relativeLaneId) const
{
    if (!graphValid)
    {
        return 0;
    }
    const auto& routeElement = get(RouteElement(), roadGraph, current);
    const auto mainLaneId = GetMainLocatePosition().value().laneId;
    if (routeElement.inOdDirection)
    {
        return  mainLaneId + relativeLaneId + ((mainLaneId < 0) && (relativeLaneId >= -mainLaneId) ? 1 : 0) + ((mainLaneId > 0) && (relativeLaneId <= -mainLaneId) ? -1 : 0);
    }
    else
    {
        return  mainLaneId - relativeLaneId + ((mainLaneId < 0) && (relativeLaneId <= mainLaneId) ? 1 : 0) + ((mainLaneId > 0) && (relativeLaneId >= mainLaneId) ? -1 : 0);
    }
}

std::optional<RouteElement> EgoAgent::GetPreviousRoad(size_t steps) const
{
    if (rootOfWayToTargetGraph + steps >= num_vertices(wayToTarget))
    {
        return std::nullopt;
    }
    return get(RouteElement(), wayToTarget, rootOfWayToTargetGraph + steps);
}

std::optional<Position> EgoAgent::GetWorldPosition(double sDistance, double tDistance, double yaw) const
{
    auto currentPosition = GetReferencePointPosition();
    if (!currentPosition.has_value())
    {
        return std::nullopt;
    }
    auto roadStream = world->GetRoadStream(wayToTargetRoute);
    auto laneStream = roadStream->GetLaneStream(currentPosition.value());
    auto laneStreamPosition = laneStream->GetStreamPosition(currentPosition.value());
    StreamPosition targetStreamPosition = {laneStreamPosition.s + sDistance, laneStreamPosition.t + tDistance};
    auto targetRoadPosition = laneStream->GetRoadPosition(targetStreamPosition);

    auto newPosition = world->LaneCoord2WorldCoord(targetRoadPosition.roadPosition.s, targetRoadPosition.roadPosition.t, targetRoadPosition.roadId, targetRoadPosition.laneId);
    newPosition.yawAngle = CommonHelper::SetAngleToValidRange(newPosition.yawAngle + targetRoadPosition.roadPosition.hdg + yaw);
    return newPosition;
}

GlobalRoadPositions EgoAgent::GetRoadPositions(const ObjectPoint &point, const WorldObjectInterface *object) const
{
    if (std::holds_alternative<ObjectPointRelative>(point))
    {
        auto resolvedPoint = world->ResolveRelativePoint(roadGraph, current, std::get<ObjectPointRelative>(point), *object);
        GlobalRoadPositions result;
        for(const auto& [vertex, position] : resolvedPoint)
        {
            if (position.has_value())
            {
                result[position->roadId] = position.value();
            }
        }
        return result;
    }
    else
    {
        return object->GetRoadPosition(point);
    }
}

void EgoAgent::SetWayToTarget(RoadGraphVertex targetVertex)
{
    wayToTarget = RoadGraph{};
    wayToTargetRoute = {};
    RoadGraphVertex wayPoint = targetVertex;
    auto routeElement = get(RouteElement(), roadGraph, wayPoint);
    auto vertex1 = add_vertex(routeElement, wayToTarget);
    wayToTargetRoute.push_back(routeElement);
    while (wayPoint != current)
    {
        for (auto [edge, edgesEnd] = edges(roadGraph); edge != edgesEnd; ++edge)
        {
            if (target(*edge, roadGraph) == wayPoint)
            {
                wayPoint = source(*edge, roadGraph);
                auto routeElement = get(RouteElement(), roadGraph, wayPoint);
                auto vertex2 = add_vertex(routeElement, wayToTarget);
                add_edge(vertex2, vertex1, wayToTarget);
                wayToTargetRoute.insert(wayToTargetRoute.begin(), routeElement);
                vertex1 = vertex2;
                break;
            }
        }
    }
    rootOfWayToTargetGraph = vertex1;
}

template<>
ExecuteReturn<DistanceToEndOfLane> EgoAgentInterface::executeQuery(DistanceToEndOfLaneParameter param) const
{
    return executeQueryDistanceToEndOfLane(param);
}

template<>
ExecuteReturn<ObjectsInRange> EgoAgentInterface::executeQuery(ObjectsInRangeParameter param) const
{
    return executeQueryObjectsInRange(param);
}

ExecuteReturn<DistanceToEndOfLane> EgoAgent::executeQueryDistanceToEndOfLane(DistanceToEndOfLaneParameter param) const
{
    if (!graphValid)
    {
        return {};
    }
    auto queryResult = world->GetDistanceToEndOfLane(wayToTarget,
                                                     current,
                                                     GetLaneIdFromRelative(param.relativeLane),
                                                     GetMainLocatePosition().value().roadPosition.s,
                                                     param.range);
    std::vector<DistanceToEndOfLane> results;
    std::transform(alternatives.cbegin(), alternatives.cend(), std::back_inserter(results),
                   [&](const RoadGraphVertex& alternative){return queryResult.at(alternative);});
    return {results};
}

ExecuteReturn<ObjectsInRange> EgoAgent::executeQueryObjectsInRange(ObjectsInRangeParameter param) const
{
    if (!graphValid)
    {
        return {};
    }
    auto queryResult = world->GetObjectsInRange(wayToTarget,
                                                current,
                                                GetLaneIdFromRelative(param.relativeLane),
                                                GetMainLocatePosition().value().roadPosition.s,
                                                param.backwardRange,
                                                param.forwardRange);
    std::vector<ObjectsInRange> results;
    std::transform(alternatives.cbegin(), alternatives.cend(), std::back_inserter(results),
                   [&](const RoadGraphVertex& alternative){return queryResult.at(alternative);});
    return {results};
}

RelativeWorldView::Roads EgoAgent::GetRelativeJunctions(double range) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetRelativeJunctions(wayToTarget,
                                       rootOfWayToTargetGraph,
                                       GetMainLocatePosition().value().roadPosition.s,
                                       range).at(0);
}

RelativeWorldView::Roads EgoAgent::GetRelativeRoads(double range) const
{
    if (!graphValid)
    {
        return {};
    }
    return world->GetRelativeRoads(wayToTarget,
                                   rootOfWayToTargetGraph,
                                   GetMainLocatePosition().value().roadPosition.s,
                                   range).at(0);
}
