/********************************************************************************
 * Copyright (c) 2019-2021 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "gmock/gmock.h"

#include "include/parameterInterface.h"
#include "include/sceneryDynamicsInterface.h"
#include "include/worldInterface.h"
#include "common/globalDefinitions.h"
#include "common/openScenarioDefinitions.h"

class FakeWorld : public WorldInterface
{
  public:
    MOCK_METHOD0(CreateAgentAdapterForAgent, AgentInterface*());
    MOCK_METHOD1(CreateAgentAdapter, AgentInterface&(const AgentBlueprintInterface &agentBlueprint));
    MOCK_CONST_METHOD1(GetAgent, AgentInterface*(int id));
    MOCK_METHOD1(GetAgentByName, AgentInterface*(const std::string &scenarioName));
    MOCK_METHOD0(GetEgoAgent, AgentInterface*());
    MOCK_METHOD0(CreateGlobalDrivingView, bool());
    MOCK_METHOD3(CreateScenery, bool(const SceneryInterface *scenery, const SceneryDynamicsInterface& sceneryDynamics, const TurningRates& turningRates));
    MOCK_METHOD1(CreateWorldScenario, bool(const std::string &scenarioFilename));
    MOCK_METHOD1(CreateWorldScenery, bool(const std::string &sceneryFilename));
    MOCK_METHOD0(Instantiate, bool());
    MOCK_CONST_METHOD1(GetLaneSections, LaneSections(const std::string& roadId));
    MOCK_METHOD6(IntersectsWithAgent, bool(double x, double y, double rotation, double length, double width, double center));
    MOCK_METHOD0(isInstantiated, bool());
    MOCK_METHOD3(IsSValidOnLane, bool(std::string roadId, int laneId, double distance));
    MOCK_CONST_METHOD2(IsDirectionalRoadExisting, bool(const std::string &roadId, bool inOdDirection));
    MOCK_METHOD4(IsLaneTypeValid, bool(const std::string &roadId, const int laneId, const double distanceOnLane, const LaneTypes& validLaneTypes));

    MOCK_CONST_METHOD0(GetBicycle, const AgentInterface*());
    MOCK_CONST_METHOD1(GetLastCarInlane, const AgentInterface*(int laneNumber));
    MOCK_CONST_METHOD0(GetSpecialAgent, const AgentInterface*());
    MOCK_METHOD0(GetRemovedAgentsInPreviousTimestep, const std::vector<int> ());
    MOCK_METHOD0(GetAgents, std::map<int, AgentInterface*>());
    MOCK_CONST_METHOD0(GetTrafficObjects, const std::vector<const TrafficObjectInterface*>&());
    MOCK_CONST_METHOD0(GetWorldObjects, const std::vector<const WorldObjectInterface*>&());
    MOCK_CONST_METHOD5(GetDistanceToEndOfLane, RouteQueryResult<double> (const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                                                                     double maximumSearchLength));
    MOCK_CONST_METHOD6(GetDistanceToEndOfLane, RouteQueryResult<double> (const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                                                                     double maximumSearchLength, const LaneTypes& laneTypes));
    MOCK_CONST_METHOD0(GetFriction, double());
    MOCK_CONST_METHOD3(GetLaneCurvature, double(std::string roadId, int laneId, double position));
    MOCK_CONST_METHOD5(GetLaneCurvature,  RouteQueryResult<std::optional<double>>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance));
    MOCK_CONST_METHOD3(GetLaneDirection, double(std::string roadId, int laneId, double position));
    MOCK_CONST_METHOD5(GetLaneDirection,  RouteQueryResult<std::optional<double>>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance));
    MOCK_CONST_METHOD3(GetLaneWidth, double(std::string roadId, int laneId, double position));
    MOCK_CONST_METHOD5(GetLaneWidth,  RouteQueryResult<std::optional<double>>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance));
    MOCK_CONST_METHOD0(GetVisibilityDistance, double());
    MOCK_CONST_METHOD2(GetLaneId, int(uint64_t streamId, double endDistance));
    MOCK_CONST_METHOD4(LaneCoord2WorldCoord, Position(double distanceOnLane, double offset, std::string roadId, int laneId));
    MOCK_CONST_METHOD2(RoadCoord2WorldCoord, Position(RoadPosition roadCoord, std::string roadID));
    MOCK_CONST_METHOD3(WorldCoord2LaneCoord, GlobalRoadPositions (double x, double y, double heading));
    MOCK_CONST_METHOD1(GetRoadLength, double(const std::string& roadId));
    MOCK_CONST_METHOD5(GetObstruction, RouteQueryResult<Obstruction>(const RoadGraph &roadGraph, RoadGraphVertex startNode, const GlobalRoadPosition &ownPosition,
                                                                     const std::map<ObjectPoint, Common::Vector2d> &points, const RoadIntervals &touchedRoads));
    MOCK_CONST_METHOD0(GetTimeOfDay, std::string());
    MOCK_CONST_METHOD0(GetTrafficRules, const TrafficRules& ());
    MOCK_CONST_METHOD5(GetTrafficSignsInRange, RouteQueryResult<std::vector<CommonTrafficSign::Entity>>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange));
    MOCK_CONST_METHOD5(GetRoadMarkingsInRange, RouteQueryResult<std::vector<CommonTrafficSign::Entity>>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange));
    MOCK_CONST_METHOD5(GetTrafficLightsInRange, RouteQueryResult<std::vector<CommonTrafficLight::Entity>>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange));
    MOCK_CONST_METHOD6(GetLaneMarkings, RouteQueryResult<std::vector<LaneMarking::Entity>> (const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double range, Side side));
    MOCK_CONST_METHOD6(GetAgentsInRange, RouteQueryResult<AgentInterfaces>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double backwardRange, double forwardRange));
    MOCK_CONST_METHOD6(GetObjectsInRange, RouteQueryResult<std::vector<const WorldObjectInterface*>>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double backwardRange, double forwardRange));
    MOCK_CONST_METHOD2(GetAgentsInRangeOfJunctionConnection, AgentInterfaces (std::string connectingRoadId, double range));
    MOCK_CONST_METHOD3(GetDistanceToConnectorEntrance, double (/*const ObjectPosition position,*/ std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId));
    MOCK_CONST_METHOD3(GetDistanceToConnectorDeparture, double (/*const ObjectPosition position,*/ std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId));
    MOCK_CONST_METHOD0(GetGlobalDrivingView, void*());
    MOCK_CONST_METHOD0(GetGlobalObjects, void*());
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD1(ExtractParameter, void(ParameterInterface *parameters));
    MOCK_METHOD1(RemoveAgent, void(const AgentInterface*agent));
    MOCK_METHOD1(QueueAgentRemove, void(const AgentInterface*agent));
    MOCK_METHOD1(QueueAgentUpdate, void(std::function<void()> func));
    MOCK_METHOD2(QueueAgentUpdate, void(std::function<void(double)> func, double val));
    MOCK_METHOD0(Reset, void());
    MOCK_METHOD1(SetTimeOfDay, void(int timeOfDay));
    MOCK_METHOD1(SetWeekday, void(Weekday weekday));
    MOCK_METHOD1(SyncGlobalData, void(int timestamp));
    MOCK_METHOD1(PublishGlobalData, void (int timestamp));
    MOCK_CONST_METHOD0(GetOsiGroundTruth, void*());
    MOCK_METHOD0(GetWorldData, void*());
    MOCK_CONST_METHOD0(GetWeekday, Weekday());
    MOCK_CONST_METHOD2(GetConnectionsOnJunction, std::vector<JunctionConnection> (std::string junctionId, std::string incomingRoadId));
    MOCK_CONST_METHOD1(GetIntersectingConnections, std::vector<IntersectingConnection> (std::string connectingRoadId));
    MOCK_CONST_METHOD1(GetPrioritiesOnJunction, std::vector<JunctionConnectorPriority> (std::string junctionId));
    MOCK_CONST_METHOD1(GetRoadSuccessor, RoadNetworkElement (std::string roadId));
    MOCK_CONST_METHOD1(GetRoadPredecessor, RoadNetworkElement (std::string roadId));
    MOCK_CONST_METHOD4(GetDistanceBetweenObjects, RouteQueryResult<std::optional<double>> (const RoadGraph& roadGraph, RoadGraphVertex startNode, double ownPosition, const GlobalRoadPositions &target));
    MOCK_CONST_METHOD4(GetRelativeJunctions, RouteQueryResult<RelativeWorldView::Roads>(const RoadGraph &roadGraph, RoadGraphVertex startNode, double startDistance, double range));
    MOCK_CONST_METHOD4(GetRelativeRoads, RouteQueryResult<RelativeWorldView::Roads>(const RoadGraph& roadGraph, RoadGraphVertex startNode, double startDistance, double range));
    MOCK_CONST_METHOD6(GetRelativeLanes, RouteQueryResult<RelativeWorldView::Lanes>(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, double range, bool includeOncoming));
    MOCK_CONST_METHOD5(GetRelativeLaneId, RouteQueryResult<std::optional<int>> (const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, GlobalRoadPositions targetPosition));
    MOCK_CONST_METHOD3(GetRoadGraph, std::pair<RoadGraph, RoadGraphVertex>(const RouteElement& start, int maxDepth, bool inDrivingDirection));
    MOCK_CONST_METHOD1(GetEdgeWeights, std::map<RoadGraphEdge, double>(const RoadGraph& roadGraph));
    MOCK_CONST_METHOD1(GetRoadStream, std::unique_ptr<RoadStreamInterface> (const std::vector<RouteElement>& route));
    MOCK_CONST_METHOD4(ResolveRelativePoint, RouteQueryResult<std::optional<GlobalRoadPosition>> (const RoadGraph& roadGraph, RoadGraphVertex startNode, ObjectPointRelative relativePoint, const WorldObjectInterface& object));
    MOCK_METHOD0(GetRadio, RadioInterface&());
};
