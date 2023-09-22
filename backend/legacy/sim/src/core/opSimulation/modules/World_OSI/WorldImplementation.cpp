/********************************************************************************
 * Copyright (c) 2020-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *               2020 HLRS, University of Stuttgart
 *               2016-2017 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "WorldData.h"
#include "WorldImplementation.h"
#include "common/RoutePlanning/RouteCalculation.h"
#include "EntityRepository.h"
#include "WorldEntities.h"

#include "osi3/osi_sensorview.pb.h"
#include "osi3/osi_sensorviewconfiguration.pb.h"

namespace {
    template <typename T>
    static std::vector<const T*> get_transformed(const std::vector<const OWL::Interfaces::WorldObject*>& worldObjects)
    {
        std::vector<const T*> transformedContainer;
        std::transform(worldObjects.begin(), worldObjects.end(), std::back_inserter(transformedContainer),
                       [](const OWL::Interfaces::WorldObject* object){ return object->GetLink<T>(); });
        return transformedContainer;
    }
}

WorldImplementation::WorldImplementation(const CallbackInterface* callbacks, StochasticsInterface* stochastics, DataBufferWriteInterface* dataBuffer):
    agentNetwork(this, callbacks),
    callbacks(callbacks),
    dataBuffer(dataBuffer),
    repository(dataBuffer),
    worldData(callbacks)
{}

WorldImplementation::~WorldImplementation()
{
}

AgentInterface *WorldImplementation::GetAgent(int id) const
{
    return agentNetwork.GetAgent(id);
}

const std::vector<const WorldObjectInterface*>& WorldImplementation::GetWorldObjects() const
{
    return worldObjects;
}

std::map<int, AgentInterface *> WorldImplementation::GetAgents()
{
    std::map<int, AgentInterface *> agents;
    for (auto& agent : agentNetwork.GetAgents())
    {
        agents.insert({agent.GetId(), &agent});
    }
    return agents;
}

const std::vector<int> WorldImplementation::GetRemovedAgentsInPreviousTimestep()
{
    return agentNetwork.GetRemovedAgentsInPreviousTimestep();
}

const std::vector<const TrafficObjectInterface*>& WorldImplementation::GetTrafficObjects() const
{
    return trafficObjects;
}

const TrafficRules& WorldImplementation::GetTrafficRules() const
{
    return worldParameter.trafficRules;
}

void WorldImplementation::ExtractParameter(ParameterInterface* parameters)
{
    auto intParameter = parameters->GetParametersInt();
    auto doubleParameter = parameters->GetParametersDouble();
    auto stringParameter = parameters->GetParametersString();
    auto boolParameter = parameters->GetParametersBool();

    worldParameter.timeOfDay = stringParameter.at("TimeOfDay");
    worldParameter.visibilityDistance = intParameter.at("VisibilityDistance");
    worldParameter.friction = doubleParameter.at("Friction");
    worldParameter.weather = stringParameter.at("Weather");

    auto openSpeedLimit = helper::map::query(doubleParameter, "OpenSpeedLimit");
    THROWIFFALSE(openSpeedLimit.has_value(), "Missing traffic rule OpenSpeedLimit")
    worldParameter.trafficRules.openSpeedLimit = openSpeedLimit.value();

    auto openSpeedLimitTrucks = helper::map::query(doubleParameter, "OpenSpeedLimitTrucks");
    THROWIFFALSE(openSpeedLimitTrucks.has_value(), "Missing traffic rule OpenSpeedLimitTrucks")
    worldParameter.trafficRules.openSpeedLimitTrucks = openSpeedLimitTrucks.value();

    auto openSpeedLimitBuses = helper::map::query(doubleParameter, "OpenSpeedLimitBuses");
    THROWIFFALSE(openSpeedLimitBuses.has_value(), "Missing traffic rule OpenSpeedLimitBuses")
    worldParameter.trafficRules.openSpeedLimitBuses = openSpeedLimitBuses.value();

    auto keepToOuterLanes = helper::map::query(boolParameter, "KeepToOuterLanes");
    THROWIFFALSE(keepToOuterLanes.has_value(), "Missing traffic rule KeepToOuterLanes")
    worldParameter.trafficRules.keepToOuterLanes = keepToOuterLanes.value();

    auto dontOvertakeOnOuterLanes = helper::map::query(boolParameter, "DontOvertakeOnOuterLanes");
    THROWIFFALSE(dontOvertakeOnOuterLanes.has_value(), "Missing traffic rule DontOvertakeOnOuterLanes")
    worldParameter.trafficRules.dontOvertakeOnOuterLanes = dontOvertakeOnOuterLanes.value();

    auto formRescueLane = helper::map::query(boolParameter, "FormRescueLane");
    THROWIFFALSE(formRescueLane.has_value(), "Missing traffic rule FormRescueLane")
    worldParameter.trafficRules.formRescueLane = formRescueLane.value();

    auto zipperMerge = helper::map::query(boolParameter, "ZipperMerge");
    THROWIFFALSE(zipperMerge.has_value(), "Missing traffic rule ZipperMerge")
    worldParameter.trafficRules.zipperMerge = zipperMerge.value();
}

void WorldImplementation::Reset()
{
    for (auto& [id, lane] : worldData.GetLanes())
    {
        lane->ClearMovingObjects();
    }
    worldData.Reset();
    worldParameter.Reset();
    agentNetwork.Clear();
    worldObjects.clear();
    repository.Reset();
    worldObjects.insert(worldObjects.end(), trafficObjects.begin(), trafficObjects.end());
}

void WorldImplementation::Clear()
{
    for (auto worldObject : worldObjects)
    {
        delete worldObject;
    }
}

void* WorldImplementation::GetWorldData()
{
    return &worldData;
}

void* WorldImplementation::GetOsiGroundTruth() const
{
    //    return &groundTruth;
    return nullptr;
}

void WorldImplementation::QueueAgentUpdate(std::function<void()> func)
{
    agentNetwork.QueueAgentUpdate(func);
}

void WorldImplementation::QueueAgentRemove(const AgentInterface* agent)
{
    agentNetwork.QueueAgentRemove(agent);
}

void WorldImplementation::RemoveAgent(const AgentInterface* agent)
{
    worldData.RemoveMovingObjectById(agent->GetId());
    auto it = std::find(worldObjects.begin(), worldObjects.end(), agent);
    if (it != worldObjects.end())
    {
        worldObjects.erase(it);
    }
}

void WorldImplementation::PublishGlobalData(int timestamp)
{
    agentNetwork.PublishGlobalData(
        [&](openpass::type::EntityId id, openpass::type::FlatParameterKey key, openpass::type::FlatParameterValue value)
        {
            dataBuffer->PutCyclic(id, key, value);
        });

    trafficLightNetwork.PublishGlobalData(
            [&](openpass::type::EntityId id, openpass::type::FlatParameterKey key, const openpass::databuffer::Acyclic &acyclic)
            {
                dataBuffer->PutAcyclic(id, key, acyclic);
            });
}

void WorldImplementation::SyncGlobalData(int timestamp)
{
    
    for (auto& [id, lane] : worldData.GetLanes())
    {
        lane->ClearMovingObjects();
    }
    agentNetwork.SyncGlobalData();

    trafficLightNetwork.UpdateStates(timestamp);
    worldData.ResetTemporaryMemory();
}

bool WorldImplementation::CreateScenery(const SceneryInterface* scenery, const SceneryDynamicsInterface& sceneryDynamics, const TurningRates& turningRates)
{
    this->scenery = scenery;
    sceneryConverter = std::make_unique<SceneryConverter>(scenery,
                                                          repository,
                                                          worldData,
                                                          localizer,
                                                          callbacks);

    THROWIFFALSE(sceneryConverter->ConvertRoads(), "Unable to finish conversion process.")
    localizer.Init();
    sceneryConverter->ConvertObjects();
    InitTrafficObjects();

    RoadNetworkBuilder networkBuilder(*scenery);
    auto [roadGraph, vertexMapping] = networkBuilder.Build();
    worldData.SetRoadGraph(std::move(roadGraph), std::move(vertexMapping));
    worldData.SetTurningRates(turningRates);
    worldData.SetEnvironment(sceneryDynamics.GetEnvironment());

    trafficLightNetwork = TrafficLightNetworkBuilder::Build(sceneryDynamics.GetTrafficSignalControllers(), worldData);
    auto it = trafficLightNetwork.getOsiTrafficLightIds();
    dataBuffer->PutStatic("TrafficLights", it, false);
    return true;
}

AgentInterface &WorldImplementation::CreateAgentAdapter(const AgentBlueprintInterface& agentBlueprint)
{
    const auto id = repository.Register(openpass::entity::EntityType::MovingObject,
                                        openpass::utils::GetEntityInfo(agentBlueprint));
    auto &movingObject = worldData.AddMovingObject(id);
    auto &agent = agentNetwork.CreateAgent(movingObject, this, callbacks, localizer);
    movingObject.SetLink(&agent);
    agent.InitParameter(agentBlueprint);
    worldObjects.push_back(&agent);
    return agent;
}

std::string WorldImplementation::GetTimeOfDay() const
{
    return worldParameter.timeOfDay;
}

double WorldImplementation::GetVisibilityDistance() const
{
    return worldParameter.visibilityDistance;
}

double WorldImplementation::GetFriction() const
{
    return worldParameter.friction;
}

RouteQueryResult<std::vector<CommonTrafficSign::Entity>> WorldImplementation::GetTrafficSignsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetTrafficSignsInRange(*laneMultiStream, startDistanceOnStream, searchRange);
}

RouteQueryResult<std::vector<CommonTrafficSign::Entity> > WorldImplementation::GetRoadMarkingsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetRoadMarkingsInRange(*laneMultiStream, startDistanceOnStream, searchRange);
}

RouteQueryResult<std::vector<CommonTrafficLight::Entity>> WorldImplementation::GetTrafficLightsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double searchRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetTrafficLightsInRange(*laneMultiStream, startDistanceOnStream, searchRange);
}

RouteQueryResult<std::vector<LaneMarking::Entity> > WorldImplementation::GetLaneMarkings(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double range, Side side) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetLaneMarkings(*laneMultiStream, startDistanceOnStream, range, side);
}

RouteQueryResult<RelativeWorldView::Roads> WorldImplementation::GetRelativeJunctions(const RoadGraph &roadGraph, RoadGraphVertex startNode, double startDistance, double range) const
{
    const auto roadMultiStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    double startDistanceOnStream = roadMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetRelativeJunctions(*roadMultiStream, startDistanceOnStream, range);
}

RouteQueryResult<RelativeWorldView::Roads> WorldImplementation::GetRelativeRoads(const RoadGraph& roadGraph, RoadGraphVertex startNode, double startDistance, double range) const
{
    const auto roadMultiStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    double startDistanceOnStream = roadMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    return worldDataQuery.GetRelativeRoads(*roadMultiStream, startDistanceOnStream, range);
}

std::vector<JunctionConnection> WorldImplementation::GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const
{
    return worldDataQuery.GetConnectionsOnJunction(junctionId, incomingRoadId);
}

std::vector<IntersectingConnection> WorldImplementation::GetIntersectingConnections(std::string connectingRoadId) const
{
    return worldDataQuery.GetIntersectingConnections(connectingRoadId);
}

std::vector<JunctionConnectorPriority> WorldImplementation::GetPrioritiesOnJunction(std::string junctionId) const
{
    return worldDataQuery.GetPrioritiesOnJunction(junctionId);
}

RoadNetworkElement WorldImplementation::GetRoadSuccessor(std::string roadId) const
{
    return worldDataQuery.GetRoadSuccessor(roadId);
}

RoadNetworkElement WorldImplementation::GetRoadPredecessor(std::string roadId) const
{
    return worldDataQuery.GetRoadPredecessor(roadId);
}

std::pair<RoadGraph, RoadGraphVertex> WorldImplementation::GetRoadGraph(const RouteElement& start, int maxDepth, bool inDrivingDirection) const
{
    auto startVertex = worldData.GetRoadGraphVertexMapping().at(start);
    return RouteCalculation::FilterRoadGraphByStartPosition(worldData.GetRoadGraph(), startVertex, maxDepth, inDrivingDirection);
}

std::map<RoadGraphEdge, double> WorldImplementation::GetEdgeWeights(const RoadGraph& roadGraph) const
{
    return worldDataQuery.GetEdgeWeights(roadGraph);
}

std::unique_ptr<RoadStreamInterface> WorldImplementation::GetRoadStream(const std::vector<RouteElement> &route) const
{
    return worldDataQuery.CreateRoadStream(route);
}

AgentInterface* WorldImplementation::GetEgoAgent()
{
    for (auto& agent : agentNetwork.GetAgents())
    {
        if (agent.IsEgoAgent())
        {
            return &agent;
        }
    }

    return nullptr;
}

AgentInterface* WorldImplementation::GetAgentByName(const std::string& scenarioName)
{
    for (auto& agent : agentNetwork.GetAgents())
    {
        if (agent.GetScenarioName() == (scenarioName))
        {
            return &agent;
        }
    }

    return nullptr;
}

RouteQueryResult<std::optional<GlobalRoadPosition>> WorldImplementation::ResolveRelativePoint(const RoadGraph &roadGraph, RoadGraphVertex startNode, ObjectPointRelative relativePoint, const WorldObjectInterface& object) const
{
    const auto roadMultiStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    return worldDataQuery.ResolveRelativePoint(*roadMultiStream, relativePoint, object.GetTouchedRoads());
}

RouteQueryResult<Obstruction> WorldImplementation::GetObstruction(const RoadGraph &roadGraph, RoadGraphVertex startNode, const GlobalRoadPosition &ownPosition,
                                                                  const std::map<ObjectPoint, Common::Vector2d> &points, const RoadIntervals &touchedRoads) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, ownPosition.laneId, ownPosition.roadPosition.s);
    return worldDataQuery.GetObstruction(*laneMultiStream, ownPosition.roadPosition.t, points, touchedRoads);
}

RouteQueryResult<RelativeWorldView::Lanes> WorldImplementation::GetRelativeLanes(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, double range, bool includeOncoming) const
{
    const auto roadMultiStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    double startDistanceOnStream = roadMultiStream->GetPositionByVertexAndS(startNode, distance);

    return worldDataQuery.GetRelativeLanes(*roadMultiStream, startDistanceOnStream, laneId, range, includeOncoming);
}

RouteQueryResult<std::optional<int> > WorldImplementation::GetRelativeLaneId(const RoadGraph &roadGraph, RoadGraphVertex startNode, int laneId, double distance, GlobalRoadPositions targetPosition) const
{
    const auto roadMultiStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    double startDistanceOnStream = roadMultiStream->GetPositionByVertexAndS(startNode, distance);

    return worldDataQuery.GetRelativeLaneId(*roadMultiStream, startDistanceOnStream, laneId, targetPosition);
}

RouteQueryResult<AgentInterfaces > WorldImplementation::GetAgentsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double backwardRange, double forwardRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    const auto queryResult = worldDataQuery.GetObjectsOfTypeInRange<OWL::Interfaces::MovingObject>(*laneMultiStream, startDistanceOnStream - backwardRange, startDistanceOnStream + forwardRange);
    RouteQueryResult<AgentInterfaces> result;
    for (const auto& [node, objects]: queryResult)
    {
        result[node] = get_transformed<AgentInterface>(objects);
    }
    return result;
}

RouteQueryResult<std::vector<const WorldObjectInterface*>> WorldImplementation::GetObjectsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double backwardRange, double forwardRange) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, startDistance);
    double startDistanceOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, startDistance);
    const auto queryResult = worldDataQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(*laneMultiStream, startDistanceOnStream - backwardRange, startDistanceOnStream + forwardRange);
    RouteQueryResult<std::vector<const WorldObjectInterface*>> result;
    for (const auto& [node, objects]: queryResult)
    {
        result[node] = get_transformed<WorldObjectInterface>(objects);
    }
    return result;
}

std::vector<const AgentInterface *> WorldImplementation::GetAgentsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const
{
    auto movingObjects = worldDataQuery.GetMovingObjectsInRangeOfJunctionConnection(connectingRoadId, range);
    return get_transformed<AgentInterface>(movingObjects);
}

double WorldImplementation::GetDistanceToConnectorEntrance(/*const ObjectPosition position,*/ std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return worldDataQuery.GetDistanceUntilObjectEntersConnector(/*position,*/ intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

double WorldImplementation::GetDistanceToConnectorDeparture(/*const ObjectPosition position,*/ std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return worldDataQuery.GetDistanceUntilObjectLeavesConnector(/*position,*/ intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

Position WorldImplementation::LaneCoord2WorldCoord(double distanceOnLane, double offset, std::string roadId,
        int laneId) const
{
    OWL::CLane& lane = worldDataQuery.GetLaneByOdId(roadId, laneId, distanceOnLane);
    return worldDataQuery.GetPositionByDistanceAndLane(lane, distanceOnLane, offset);
}

GlobalRoadPositions WorldImplementation::WorldCoord2LaneCoord(double x, double y, double heading) const
{
    return localizer.Locate({x,y}, heading);
}

bool WorldImplementation::IsSValidOnLane(std::string roadId, int laneId,
        double distance) //when necessary optional parameter with reference to get point
{
    return worldDataQuery.IsSValidOnLane(roadId, laneId, distance);
}

bool WorldImplementation::IsDirectionalRoadExisting(const std::string &roadId, bool inOdDirection) const
{
    return worldData.GetRoadGraphVertexMapping().find(RouteElement {roadId, inOdDirection}) != worldData.GetRoadGraphVertexMapping().end();
}

bool WorldImplementation::IsLaneTypeValid(const std::string &roadId, const int laneId, const double distanceOnLane, const LaneTypes& validLaneTypes)
{
    const auto& laneType = worldDataQuery.GetLaneByOdId(roadId, laneId, distanceOnLane).GetLaneType();

    if (std::find(validLaneTypes.begin(), validLaneTypes.end(), laneType) == validLaneTypes.end())
    {
        return false;
    }

    return true;
}

double WorldImplementation::GetLaneCurvature(std::string roadId, int laneId, double position) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    return lane.GetCurvature(position);
}

RouteQueryResult<std::optional<double> > WorldImplementation::GetLaneCurvature(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, position);
    double positionOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, position);
    return worldDataQuery.GetLaneCurvature(*laneMultiStream, positionOnStream + distance);
}

double WorldImplementation::GetLaneWidth(std::string roadId, int laneId, double position) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    return lane.GetWidth(position);
}

RouteQueryResult<std::optional<double>> WorldImplementation::GetLaneWidth(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, position);
    double positionOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, position);
    return worldDataQuery.GetLaneWidth(*laneMultiStream, positionOnStream + distance);
}

double WorldImplementation::GetLaneDirection(std::string roadId, int laneId, double position) const
{
    auto& lane =  worldDataQuery.GetLaneByOdId(roadId, laneId, position);
    if (!lane.Exists())
    {
        return 0.0;
    }
    return lane.GetDirection(position);
}

RouteQueryResult<std::optional<double> > WorldImplementation::GetLaneDirection(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const
{
    const auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, position);
    double positionOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, position);
    return worldDataQuery.GetLaneDirection(*laneMultiStream, positionOnStream + distance);
}

RouteQueryResult<double> WorldImplementation::GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance, double maximumSearchLength) const
{
    return GetDistanceToEndOfLane(roadGraph, startNode, laneId, initialSearchDistance, maximumSearchLength,
                                  {LaneType::Driving, LaneType::Exit, LaneType::OnRamp, LaneType::OffRamp, LaneType::Stop});
}

RouteQueryResult<double> WorldImplementation::GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance, double maximumSearchLength, const LaneTypes& laneTypes) const
{
    auto laneMultiStream = worldDataQuery.CreateLaneMultiStream(roadGraph, startNode, laneId, initialSearchDistance);
    auto initialPositionOnStream = laneMultiStream->GetPositionByVertexAndS(startNode, initialSearchDistance);
    return worldDataQuery.GetDistanceToEndOfLane(*laneMultiStream, initialPositionOnStream, maximumSearchLength, laneTypes);
}

LaneSections WorldImplementation::GetLaneSections(const std::string& roadId) const
{
    LaneSections result;

    const auto& road = worldDataQuery.GetRoadByOdId(roadId);
    for (const auto &section : road->GetSections())
    {
        LaneSection laneSection;
        laneSection.startS = section->GetSOffset();
        laneSection.endS = laneSection.startS + section->GetLength();



        for (const auto& lane : section->GetLanes())
        {
            laneSection.laneIds.push_back(lane->GetOdId());
        }

        result.push_back(laneSection);
    }

    return result;
}

bool WorldImplementation::IntersectsWithAgent(double x, double y, double rotation, double length, double width,
        double center)
{

    polygon_t polyNewAgent = World::Localization::GetBoundingBox(x, y, length, width, rotation, center);

    for (std::pair<int, AgentInterface*> agent : GetAgents())
    {
        polygon_t polyAgent = agent.second->GetBoundingBox2D();


        bool intersects = bg::intersects(polyNewAgent, polyAgent);
        if (intersects)
        {
            return true;
        }
    }

    return false;
}

Position WorldImplementation::RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID) const
{
    const auto& [lane, tOnLane] = worldDataQuery.GetLaneByOffset(roadID, roadCoord.t, roadCoord.s);
    return worldDataQuery.GetPositionByDistanceAndLane(lane, roadCoord.s, tOnLane);
}

double WorldImplementation::GetRoadLength(const std::string& roadId) const
{
    return worldDataQuery.GetRoadByOdId(roadId)->GetLength();
}

void WorldImplementation::InitTrafficObjects()
{
    assert(trafficObjects.size() == 0);

    for (auto& baseTrafficObject : worldData.GetStationaryObjects())
    {
        TrafficObjectInterface* trafficObject = baseTrafficObject.second->GetLink<TrafficObjectInterface>();
        trafficObjects.push_back(trafficObject);
        worldObjects.push_back(trafficObject);
    }
}

RouteQueryResult<std::optional<double>> WorldImplementation::GetDistanceBetweenObjects(const RoadGraph& roadGraph, RoadGraphVertex startNode,
                                                                                      double ownPosition, const GlobalRoadPositions &target) const
{
    const auto roadStream = worldDataQuery.CreateRoadMultiStream(roadGraph, startNode);
    const auto ownStreamPosition = roadStream->GetPositionByVertexAndS(startNode, ownPosition);
    return worldDataQuery.GetDistanceBetweenObjects(*roadStream, ownStreamPosition, target);
}

RadioInterface &WorldImplementation::GetRadio()
{
    return radio;
}
