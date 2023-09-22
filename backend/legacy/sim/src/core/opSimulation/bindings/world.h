/********************************************************************************
 * Copyright (c) 2017-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  World.h
//! @brief This file includes the static and dynamic objects used during a
//!        simulation run.
//-----------------------------------------------------------------------------

#pragma once

#include "include/worldInterface.h"
#include "bindings/worldBinding.h"

namespace core {

class World : public WorldInterface
{
public:
    World(WorldBinding* worldBinding) :
        worldBinding(worldBinding)
    {}

    World(const World&) = delete;
    World(World&&) = delete;
    World& operator=(const World&) = delete;
    World& operator=(World&&) = delete;

    ~World() = default;

    AgentInterface* GetAgent(int id) const override
    {
        return implementation->GetAgent(id);
    }

    AgentInterface* GetAgentByName(const std::string& scenarioName) override
    {
        return implementation->GetAgentByName(scenarioName);
    }

    const std::vector<const WorldObjectInterface*>& GetWorldObjects() const override
    {
        return implementation->GetWorldObjects();
    }

    std::map<int, AgentInterface*> GetAgents() override
    {
        return implementation->GetAgents();
    }

    const std::vector<const TrafficObjectInterface*>& GetTrafficObjects() const override
    {
        return implementation->GetTrafficObjects();
    }

    const std::vector<int> GetRemovedAgentsInPreviousTimestep() override
    {
        return implementation->GetRemovedAgentsInPreviousTimestep();
    }

    // framework internal methods to access members without restrictions
    void ExtractParameter(ParameterInterface* parameters) override
    {
        return implementation->ExtractParameter(parameters);
    }

    void Reset() override
    {
        return implementation->Reset();
    }

    void Clear() override
    {
        return implementation->Clear();
    }

    std::string GetTimeOfDay() const override
    {
        return implementation->GetTimeOfDay();
    }

    double GetVisibilityDistance() const override
    {
        return implementation->GetVisibilityDistance();
    }

    const TrafficRules& GetTrafficRules() const override
    {
        return implementation->GetTrafficRules();
    }

    double GetFriction() const override
    {
        return implementation->GetFriction();
    }

    void* GetOsiGroundTruth() const override
    {
        return implementation->GetOsiGroundTruth();
    }

    void* GetWorldData() override
    {
        return implementation->GetWorldData();
    }

    void QueueAgentUpdate(std::function<void()> func) override
    {
        return implementation->QueueAgentUpdate(func);
    }

    void QueueAgentRemove(const AgentInterface* agent) override
    {
        return implementation->QueueAgentRemove(agent);
    }

    void PublishGlobalData(int timestamp) override
    {
        return implementation->PublishGlobalData(timestamp);
    }

    void SyncGlobalData(int timestamp) override
    {
        return implementation->SyncGlobalData(timestamp);
    }

    bool CreateScenery(const SceneryInterface* scenery, const SceneryDynamicsInterface& sceneryDynamics, const TurningRates& turningRates) override
    {
        return implementation->CreateScenery(scenery, sceneryDynamics, turningRates);
    }

    AgentInterface* CreateAgentAdapterForAgent() override
    {
        return implementation->CreateAgentAdapterForAgent();
    }

    AgentInterface& CreateAgentAdapter(const AgentBlueprintInterface& agentBlueprint) override
    {
        return implementation->CreateAgentAdapter(agentBlueprint);
    }

    AgentInterface* GetEgoAgent() override
    {
        return implementation->GetEgoAgent();
    }


    RouteQueryResult<std::optional<GlobalRoadPosition>> ResolveRelativePoint(const RoadGraph& roadGraph, RoadGraphVertex startNode, ObjectPointRelative relativePoint, const WorldObjectInterface& object) const override
    {
        return implementation->ResolveRelativePoint(roadGraph, startNode, relativePoint, object);
    }

    RouteQueryResult<AgentInterfaces> GetAgentsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance,
                                                                       double backwardRange, double forwardRange) const override
    {
        return implementation->GetAgentsInRange(roadGraph, startNode, laneId, startDistance, backwardRange, forwardRange);
    }

    RouteQueryResult<std::vector<const WorldObjectInterface*>> GetObjectsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance,
                                                                       double backwardRange, double forwardRange) const override
    {
        return implementation->GetObjectsInRange(roadGraph, startNode, laneId, startDistance, backwardRange, forwardRange);
    }

    AgentInterfaces GetAgentsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const override
    {
        return implementation->GetAgentsInRangeOfJunctionConnection(connectingRoadId, range);
    }

    double GetDistanceToConnectorEntrance(/*const ObjectPosition position,*/ std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override
    {
        return implementation->GetDistanceToConnectorEntrance(/*position,*/ intersectingConnectorId, intersectingLaneId, ownConnectorId);
    }

    double GetDistanceToConnectorDeparture(/*const ObjectPosition position,*/ std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override
    {
        return implementation->GetDistanceToConnectorDeparture(/*position,*/ intersectingConnectorId, intersectingLaneId, ownConnectorId);
    }

    Position LaneCoord2WorldCoord(double distance, double offset, std::string roadId, int laneId) const override
    {
        return implementation->LaneCoord2WorldCoord(distance, offset, roadId, laneId);
    }

    GlobalRoadPositions WorldCoord2LaneCoord(double x, double y, double heading) const override
    {
        return implementation->WorldCoord2LaneCoord(x, y, heading);
    }

    bool IsSValidOnLane(std::string roadId, int laneId, double distance) override
    {
        return implementation->IsSValidOnLane(roadId, laneId, distance);
    }

    bool IsDirectionalRoadExisting(const std::string &roadId, bool inOdDirection) const override
    {
        return implementation->IsDirectionalRoadExisting(roadId, inOdDirection);
    }

    bool IsLaneTypeValid(const std::string &roadId, const int laneId, const double distanceOnLane, const LaneTypes& validLaneTypes) override
    {
        return implementation->IsLaneTypeValid(roadId, laneId, distanceOnLane, validLaneTypes);
    }

    double GetLaneCurvature(std::string roadId, int laneId, double position) const override
    {
        return implementation->GetLaneCurvature(roadId, laneId, position);
    }

    RouteQueryResult<std::optional<double> > GetLaneCurvature(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const override
    {
        return  implementation->GetLaneCurvature(roadGraph, startNode, laneId, position, distance);
    }

    double GetLaneWidth(std::string roadId, int laneId, double position) const override
    {
        return implementation->GetLaneWidth(roadId, laneId, position);
    }

    RouteQueryResult<std::optional<double> > GetLaneWidth(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const override
    {
        return  implementation->GetLaneWidth(roadGraph, startNode, laneId, position, distance);
    }

    double GetLaneDirection(std::string roadId, int laneId, double position) const override
    {
        return implementation->GetLaneDirection(roadId, laneId, position);
    }

    RouteQueryResult<std::optional<double> > GetLaneDirection(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const override
    {
        return  implementation->GetLaneDirection(roadGraph, startNode, laneId, position, distance);
    }

    RouteQueryResult<double> GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                                                     double maximumSearchLength) const override
    {
        return implementation->GetDistanceToEndOfLane(roadGraph, startNode, laneId, initialSearchDistance, maximumSearchLength);
    }

    RouteQueryResult<double> GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                                                     double maximumSearchLength, const LaneTypes& laneTypes) const override
    {
        return implementation->GetDistanceToEndOfLane(roadGraph, startNode, laneId, initialSearchDistance, maximumSearchLength, laneTypes);
    }

    LaneSections GetLaneSections(const std::string& roadId) const
    {
        return implementation->GetLaneSections(roadId);
    }

    bool IntersectsWithAgent(double x, double y, double rotation, double length, double width, double center) override
    {
        return implementation->IntersectsWithAgent(x, y, rotation, length, width, center);
    }

    Position RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID = "") const override
    {
        return implementation->RoadCoord2WorldCoord(roadCoord, roadID);
    }

    double GetRoadLength (const std::string& roadId) const override
    {
        return implementation->GetRoadLength(roadId);
    }

    //-----------------------------------------------------------------------------
    //! Instantiate the world by creating a WorldInterface out of a world library
    //!
    //! @return                true if successful
    //-----------------------------------------------------------------------------
    bool Instantiate()
    {
        if (!worldBinding)
        {
            return false;
        }
        else
            if (!implementation)
            {
                implementation = worldBinding->Instantiate();
                if (!implementation)
                {
                    return false;
                }
            }
        return true;
    }

    //-----------------------------------------------------------------------------
    //! Returns true if world was already instantiated
    //!
    //! @return                true if world was already instantiated
    //-----------------------------------------------------------------------------
    bool isInstantiated() override
    {
        return implementation;
    }

    RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetTrafficSignsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                    double startDistance, double searchRange) const override
    {
        return implementation->GetTrafficSignsInRange(roadGraph, startNode, laneId, startDistance, searchRange);
    }

    RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetRoadMarkingsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                                double startDistance, double searchRange) const override
    {
        return implementation->GetRoadMarkingsInRange(roadGraph, startNode, laneId, startDistance, searchRange);
    }

    RouteQueryResult<std::vector<CommonTrafficLight::Entity>> GetTrafficLightsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                      double startDistance, double searchRange) const override
    {
        return implementation->GetTrafficLightsInRange(roadGraph, startNode, laneId, startDistance, searchRange);
    }

    RouteQueryResult<std::vector<LaneMarking::Entity>> GetLaneMarkings(const RoadGraph& roadGraph, RoadGraphVertex startNode,
                                                                       int laneId, double startDistance, double range, Side side) const override
    {
        return implementation->GetLaneMarkings(roadGraph, startNode,laneId, startDistance, range, side);
    }

    [[deprecated]] RouteQueryResult<RelativeWorldView::Roads> GetRelativeJunctions(const RoadGraph &roadGraph, RoadGraphVertex startNode, double startDistance, double range) const override
    {
        return implementation->GetRelativeJunctions(roadGraph, startNode, startDistance, range);
    }

    RouteQueryResult<RelativeWorldView::Roads> GetRelativeRoads(const RoadGraph& roadGraph, RoadGraphVertex startNode, double startDistance, double range) const override
    {
        return implementation->GetRelativeRoads(roadGraph, startNode, startDistance, range);
    }

    RouteQueryResult<RelativeWorldView::Lanes> GetRelativeLanes(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, double range, bool includeOncoming = true) const override
    {
        return implementation->GetRelativeLanes(roadGraph, startNode, laneId, distance, range, includeOncoming);
    }

    RouteQueryResult<std::optional<int>> GetRelativeLaneId(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, GlobalRoadPositions targetPosition) const override
    {
        return implementation->GetRelativeLaneId(roadGraph, startNode, laneId, distance, targetPosition);
    }

    std::vector<JunctionConnection> GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const override
    {
        return implementation->GetConnectionsOnJunction(junctionId, incomingRoadId);
    }

    std::vector<IntersectingConnection> GetIntersectingConnections(std::string connectingRoadId) const override
    {
        return implementation->GetIntersectingConnections(connectingRoadId);
    }

    std::vector<JunctionConnectorPriority> GetPrioritiesOnJunction(std::string junctionId) const override
    {
        return implementation->GetPrioritiesOnJunction(junctionId);
    }

    RoadNetworkElement GetRoadSuccessor(std::string roadId) const override
    {
        return implementation->GetRoadSuccessor(roadId) ;
    }

    RoadNetworkElement GetRoadPredecessor(std::string roadId) const override
    {
        return implementation->GetRoadPredecessor(roadId) ;
    }

    std::pair<RoadGraph, RoadGraphVertex> GetRoadGraph (const RouteElement& start, int maxDepth, bool inDrivingDirection) const override
    {
        return implementation->GetRoadGraph(start, maxDepth, inDrivingDirection);
    }

    std::map<RoadGraphEdge, double> GetEdgeWeights (const RoadGraph& roadGraph) const override
    {
        return implementation->GetEdgeWeights(roadGraph);
    }

    std::unique_ptr<RoadStreamInterface> GetRoadStream(const std::vector<RouteElement>& route) const override
    {
        return implementation->GetRoadStream(route);
    }

    virtual RouteQueryResult<Obstruction> GetObstruction(const RoadGraph &roadGraph, RoadGraphVertex startNode, const GlobalRoadPosition &ownPosition,
                                                         const std::map<ObjectPoint, Common::Vector2d> &points, const RoadIntervals &touchedRoads) const override
    {
        return implementation->GetObstruction(roadGraph, startNode, ownPosition, points, touchedRoads);
    }

    virtual void *GetGlobalDrivingView() const override
    {
        return implementation->GetGlobalDrivingView();
    }
    virtual void *GetGlobalObjects() const override
    {
        return implementation->GetGlobalObjects();
    }
    virtual void SetTimeOfDay(int timeOfDay) override
    {
        return implementation->SetTimeOfDay(timeOfDay);
    }
    virtual void SetWeekday(Weekday weekday) override
    {
        return implementation->SetWeekday(weekday);
    }
    virtual Weekday GetWeekday() const override
    {
        return implementation->GetWeekday();
    }
    virtual bool CreateGlobalDrivingView() override
    {
        return implementation->CreateGlobalDrivingView();
    }
    virtual const AgentInterface *GetSpecialAgent() const override
    {
        return implementation->GetSpecialAgent();
    }
    virtual const AgentInterface *GetLastCarInlane(int laneNumber) const override
    {
        return implementation->GetLastCarInlane(laneNumber);
    }
    virtual const AgentInterface *GetBicycle() const override
    {
        return implementation->GetBicycle();
    }
    virtual void QueueAgentUpdate(std::function<void(double)> func,
                                  double val) override
    {
        return implementation->QueueAgentUpdate(func, val);
    }
    virtual bool CreateWorldScenery(const std::string &sceneryFilename) override
    {
        return implementation->CreateWorldScenery(sceneryFilename);
    }
    virtual bool CreateWorldScenario(const std::string &scenarioFilename) override
    {
        return implementation->CreateWorldScenario(scenarioFilename);
    }
    RouteQueryResult<std::optional<double>> GetDistanceBetweenObjects(const RoadGraph& roadGraph, RoadGraphVertex startNode, double ownPosition, const GlobalRoadPositions &target) const override
    {
        return implementation->GetDistanceBetweenObjects(roadGraph, startNode, ownPosition, target);
    }
    virtual RadioInterface& GetRadio() override
    {
        return implementation->GetRadio();
    }

private:
    WorldBinding* worldBinding = nullptr;
    WorldInterface* implementation = nullptr;
};

} // namespace core


