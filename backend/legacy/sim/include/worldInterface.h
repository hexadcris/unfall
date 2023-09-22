/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
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
//! @file  worldInterface.h
//! @brief This file provides the interface of the scenery and dynamic objects
//!        to custom libraries.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <functional>
#include <list>

#include "common/boostGeometryCommon.h"
#include "common/globalDefinitions.h"
#include "common/openPassTypes.h"
#include "common/openScenarioDefinitions.h"
#include "common/vector2d.h"
#include "common/worldDefinitions.h"
#include "include/streamInterface.h"
#include "include/radioInterface.h"

class AgentInterface;
class AgentBlueprintInterface;
class ParameterInterface;
class SceneryInterface;
class SceneryDynamicsInterface;
class TrafficObjectInterface;
class WorldObjectInterface;

using LaneTypes = std::vector<LaneType>;

//-----------------------------------------------------------------------------
//! Provides access to world representation
//-----------------------------------------------------------------------------
class WorldInterface
{
public:
    WorldInterface() = default;
    WorldInterface(const WorldInterface &) = delete;
    WorldInterface(WorldInterface &&) = delete;
    WorldInterface &operator=(const WorldInterface &) = delete;
    WorldInterface &operator=(WorldInterface &&) = delete;
    virtual ~WorldInterface() = default;

    virtual bool Instantiate() {return false;}

    virtual bool isInstantiated() {return false;}

    //-----------------------------------------------------------------------------
    //! Retrieves the OSI ground truth
    //!
    //! @return                global view
    //-----------------------------------------------------------------------------
    virtual void* GetOsiGroundTruth() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves global view on complete world (from which a sensor can retrieve
    //! arbitrary information)
    //!
    //! @return                global view
    //-----------------------------------------------------------------------------
    virtual void *GetGlobalDrivingView() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves static objects (roads and junctions)
    //!
    //! @return                reference to scenery
    //-----------------------------------------------------------------------------
    virtual void *GetGlobalObjects() const = 0;

    //-----------------------------------------------------------------------------
    //! Sets time of day (hour) for the simulation run
    //!
    //! @return                time of day [0-23]
    //-----------------------------------------------------------------------------
    virtual void SetTimeOfDay(int timeOfDay) = 0;

    //-----------------------------------------------------------------------------
    //! Sets weekday for the simulation run
    //!
    //! @return                weekday
    //-----------------------------------------------------------------------------
    virtual void SetWeekday(Weekday weekday) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the OSI world data
    //!
    //! @return                Pointer OSI world data structure
    //-----------------------------------------------------------------------------
    virtual void* GetWorldData() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves time of day (hour)
    //!
    //! @return                time of day [0, 23]
    //-----------------------------------------------------------------------------
    virtual std::string GetTimeOfDay() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves weekday
    //!
    //! @return                weekday
    //-----------------------------------------------------------------------------
    virtual Weekday GetWeekday() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the visibility distance (meter)
    //!
    //! @return                visibility distance
    //-----------------------------------------------------------------------------
    virtual double GetVisibilityDistance() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves traffic rules
    //!
    //! @return                traffic rules
    //-----------------------------------------------------------------------------
    virtual const TrafficRules& GetTrafficRules() const = 0;

    //-----------------------------------------------------------------------------
    //! Sets the world parameters like weekday, library
    //! @param[in]     worldParamter  parameter to setup world
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void ExtractParameter(ParameterInterface* parameters) = 0;

    //-----------------------------------------------------------------------------
    //! Reset the world for the next run
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void Reset() = 0;

    //-----------------------------------------------------------------------------
    //! Clear the world, reset internal states
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void Clear() = 0;

    //-----------------------------------------------------------------------------
    //! Create internal global driving view
    //!
    //! @return                weekday
    //-----------------------------------------------------------------------------
    virtual bool CreateGlobalDrivingView() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves specific agent
    //! @param[in]  id         id of agent
    //!
    //! @return                Agent reference
    //-----------------------------------------------------------------------------
    virtual AgentInterface *GetAgent(int id) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves all agents
    //!
    //! @return                Mapping of ids to agents
    //-----------------------------------------------------------------------------
    virtual std::map<int, AgentInterface *> GetAgents() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves all worldObjects that currently exist
    //!
    //! @return                Vector of all worldObjects
    //-----------------------------------------------------------------------------
    virtual const std::vector<const WorldObjectInterface*>& GetWorldObjects() const = 0;

    //-----------------------------------------------------------------------------
    //! queue functions and values to update agent when SyncGlobalData is called
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void QueueAgentUpdate(std::function<void(double)> func,
                                  double val) = 0;

    //-----------------------------------------------------------------------------
    //! queue functions and values to update agent when SyncGlobalData is called
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void QueueAgentUpdate(std::function<void()> func) = 0;

    //-----------------------------------------------------------------------------
    //! Add agent to list to be removed when SyncGlobalData is called
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void QueueAgentRemove(const AgentInterface *agent) = 0;

    //-----------------------------------------------------------------------------
    //! Wirte all global date into the datesstore
    //!
    //! @return timestamp under which the data should be published
    //-----------------------------------------------------------------------------
    virtual void PublishGlobalData(int timestamp) = 0;

    //-----------------------------------------------------------------------------
    //! Update all agents / Remove agents and sync data of all agents within world
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual void SyncGlobalData(int timestamp) = 0;

    //-----------------------------------------------------------------------------
    //! Create a scenery in world.
    //!
    //! @param scenery          OpenDrive scenery to convert
    //! @param sceneryDynamics  scenery related information in the scenario
    //! @param turningRates     turning rate for junctions
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool CreateScenery(const SceneryInterface *scenery, const SceneryDynamicsInterface& sceneryDynamics, const TurningRates& turningRates) = 0;

    //-----------------------------------------------------------------------------
    //! Create an agentAdapter for an agent to communicate between the agent of the
    //! framework and the world.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    [[deprecated("Use CreateAgentAdapter instead")]] virtual AgentInterface *CreateAgentAdapterForAgent() = 0;

    //------------------------------------------------------------------------------------
    //! @brief Create an agentAdapter for an agent to communicate between the agent of the
    //! framework and the world.
    //!
    //! @param agentBlueprint  blueprint holding parameters of the agent
    //! @return          Instance of the AgentAdapter (implementing AgentInterface)
    //------------------------------------------------------------------------------------
    virtual AgentInterface &CreateAgentAdapter(const AgentBlueprintInterface &agentBlueprint) = 0;

    //-----------------------------------------------------------------------------
    //! Returns one agent which is set to be special.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const AgentInterface *GetSpecialAgent() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the last car in lane. Return nullptr if no car is found.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const AgentInterface *GetLastCarInlane(int laneNumber) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns an agentInterface which is a bicycle. Returns nullptr if no bicyle
    //! found.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual const AgentInterface *GetBicycle() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve the world postion by the distance and the lane.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual Position LaneCoord2WorldCoord(double distanceOnLane, double offset, std::string roadId,
            int laneId) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve all lane positions corresponding to the specified world position
    //!
    //! @return Position on all lanes at specified world position
    //-----------------------------------------------------------------------------
    virtual GlobalRoadPositions WorldCoord2LaneCoord(double x, double y, double heading) const = 0;

    //-----------------------------------------------------------------------------
    //! Tries to create an internal scenery from a given file.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool CreateWorldScenery(const  std::string &sceneryFilename) = 0;

    //-----------------------------------------------------------------------------
    //! Tries to create an internal scenario from a given file.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool CreateWorldScenario(const  std::string &scenarioFilename) = 0;

    //-----------------------------------------------------------------------------
    //! Locate a given relative point of an object in relation to a given RoadGraph
    //!
    //! @param[in] roadGraph        tree in road network as base of the relation
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] relativePoint    point to resolve (i.e. locate)
    //! @param[in] object           object of interest
    //!
    //! @return located position
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<GlobalRoadPosition>> ResolveRelativePoint(const RoadGraph& roadGraph, RoadGraphVertex startNode, ObjectPointRelative relativePoint, const WorldObjectInterface& object) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns all agents in specified range (also agents partially in search interval).
    //! Returns empty list otherwise.
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] startDistance    start of search (s coordinate)
    //! @param[in] backwardRange    search range against route direction from measured from startDistance
    //! @param[in] forwardRange     search range in route direction from measured from startDistance
    //!
    //! @return All agents in specified range
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<AgentInterfaces> GetAgentsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance,
                                                                       double backwardRange, double forwardRange) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns all objects in specified range (also objects partially in search interval).
    //! Returns empty list otherwise.
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] startDistance    start of search (s coordinate)
    //! @param[in] backwardRange    search range against route direction from measured from startDistance
    //! @param[in] forwardRange     search range in route direction from measured from startDistance
    //!
    //! @return All objects in specified range
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::vector<const WorldObjectInterface*>> GetObjectsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance,
                                                                       double backwardRange, double forwardRange) const = 0;

    //! Returns all agents on the specified connectingRoad of a junction and on the incoming lanes that lead to this connecting road
    //! inside a certain range. The range is measured backwards from the end of the connecting road.
    //!
    //! \param connectingRoadId OpenDrive id of the connecting road
    //! \param range            Distance of the search start to the end of connecting road
    //! 
    //! \return  All agents in specified range
    virtual AgentInterfaces GetAgentsInRangeOfJunctionConnection(std::string connectingRoadId, double range) const = 0;

    //! Returns the s coordinate distance from the front of the agent to the first point where his lane intersects another.
    //! As the agent may not yet be on the junction, it has to be specified which connecting road he will take in the junction
    //!
    //! \param intersectingConnectorId  OpenDrive id of the connecting road that intersects with the agent
    //! \param intersectingLaneId       OpenDrive id of the lane on the intersecting connecting road
    //! \param ownConnectorId           OpenDrive id of the connecting road that this agent is assumed to drive on
    //!
    //! \return distance of front of agent to the intersecting lane
    virtual double GetDistanceToConnectorEntrance(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const = 0;

    //! Returns the s coordinate distance from the rear of the agent to the furthest point where his lane intersects another.
    //! As the agent may not yet be on the junction, it has to be specified which connecting road he will take in the junction
    //!
    //! \param intersectingConnectorId  OpenDrive id of the connecting road that intersects with the agent
    //! \param intersectingLaneId       OpenDrive id of the lane on the intersecting connecting road
    //! \param ownConnectorId           OpenDrive id of the connecting road that this agent is assumed to drive on
    //!
    //! \return distance of rear of agent to the farther side of the intersecting lane
    virtual double GetDistanceToConnectorDeparture(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve whether s coordinate is valid on given lane.
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] laneId OpenDriveId of lane to search in
    //! @param[in] distance  s coordinate
    //! @return true if s is valid at given distance, false otherwise
    //-----------------------------------------------------------------------------
    virtual bool IsSValidOnLane(std::string roadId, int laneId, double distance) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve whether a road id exists in a specified openDrive direction.
    //!
    //! @param[in] roadId  OpenDriveId of the road to search in
    //! @param[in] inOdDirection  true means in openDrive direction
    //!
    //! @return true if road id exists
    //-----------------------------------------------------------------------------
    virtual bool IsDirectionalRoadExisting(const std::string &, bool inOdDirection) const = 0;

    //-----------------------------------------------------------------------------
    //! Return whether a LaneType is valid based on a range of valid LaneTypes
    //!
    //! @param[in] roadId  OpenDriveId of the road being evaluated
    //! @param[in] laneId  OpenDriveId of the lane being evaluated
    //! @param[in] distanceOnLane  distance in m along the road
    //! @param[in] validLaneTypes  container of valid laneTypes
    //!
    //! @return true if the lane has a valid LaneType
    //-----------------------------------------------------------------------------
    virtual bool IsLaneTypeValid(const std::string &roadId, const int laneId, const double distanceOnLane, const LaneTypes& validLaneTypes) = 0;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the curvature of the lane at the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position.
    //!
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @return curvature at position
    //-----------------------------------------------------------------------------
    virtual double GetLaneCurvature(std::string roadId, int laneId, double position) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the curvature of the lane at distance from the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] position s       coordinate of search start
    //! @param[in] distance s       coordinate difference from position to the point of interst
    //! @return curvature at position
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<double> > GetLaneCurvature(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the width of the lane the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position.
    //!
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @return width at position
    //-----------------------------------------------------------------------------
    virtual double GetLaneWidth(std::string roadId, int laneId, double position) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns interpolated value for the width of the lane at distance from the given position.
    //! Neighbouring joints are used as interpolation support point.
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] position s       coordinate of search start
    //! @param[in] distance s       coordinate difference from position to the point of interst
    //! @return width at position
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<double> > GetLaneWidth(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns value for the direction (i.e. heading) of the lane at the given position.
    //! The heading is constant between two joints
    //! Returns 0 if there is no lane at the given position.
    //!
    //! @param[in] roadId   OpenDriveId of the road to search in
    //! @param[in] laneId   OpenDriveId of lane to search in
    //! @param[in] position s coordinate of search start
    //! @return direction at position
    //-----------------------------------------------------------------------------
    virtual double GetLaneDirection(std::string roadId, int laneId, double position) const = 0 ;

    //-----------------------------------------------------------------------------
    //! Returns value for the curvature (i.e. heading) of the lane at distance from the given position.
    //! The heading is constant between two joints
    //! Returns 0 if there is no lane at the given position or the lane stream ends before the distance
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneId           OpenDriveId of lane to search in
    //! @param[in] position s       coordinate of search start
    //! @param[in] distance s       coordinate difference from position to the point of interst
    //! @return direction at position
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<double> > GetLaneDirection(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double position, double distance) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next lane which has non of the following types:
    //! Driving, Exit, OnRamp, OffRamp or Stop
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneNumber       OpenDriveId of lane to search in
    //! @param[in] distance         s coordinate
    //! @param[in] maxSearchLength  maximum search length
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<double> GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                                                             double maximumSearchLength) const = 0;

    //-----------------------------------------------------------------------------
    //! Returns remaining distance to end of lane stream (along given route) or until next lane which has non of the specified types:
    //!
    //! @param[in] roadGraph        tree in road network to search in
    //! @param[in] startNode        root of tree (identifies start road)
    //! @param[in] laneNumber       OpenDriveId of lane to search in
    //! @param[in] distance         s coordinate
    //! @param[in] maxSearchLength  maximum search length
    //! @param[in] laneTypes        filter for lane types
    //! @return remaining distance
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<double> GetDistanceToEndOfLane(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double initialSearchDistance,
                                                                             double maximumSearchLength, const LaneTypes& laneTypes) const = 0;

    //-----------------------------------------------------------------------------
    //! \brief GetDistanceBetweenObjects gets the distance between two
    //!        positions on the specified Route. Returns
    //!        std::numeric_limits<double>::max() if Route does not include both
    //!        positions
    //!
    //! \param route the Route on which to calculate the distance between two
    //!        positions
    //! \param ownPosition the position of the first object
    //! \param target the position of the second object
    //! \return the distance between the positions on the Route
    //-----------------------------------------------------------------------------
    virtual RouteQueryResult<std::optional<double>> GetDistanceBetweenObjects(const RoadGraph& roadGraph,
                                                                              RoadGraphVertex startNode,
                                                                              double ownPosition,
                                                                              const GlobalRoadPositions &target) const = 0;

    //-----------------------------------------------------------------------------
    //! \brief This method returns all LaneSections of a road
    //!
    //! \param roadId   Id of the road
    //!
    //! \return LaneSections
    //-----------------------------------------------------------------------------
    virtual LaneSections GetLaneSections(const std::string& roadId) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieve whether a new agent intersects with an existing agent
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual bool IntersectsWithAgent(double x, double y, double rotation, double length, double width, double center) = 0;

    virtual Position RoadCoord2WorldCoord(RoadPosition roadCoord, std::string roadID = "") const = 0;

    /*!
     * \brief Retrieve the length of a road
     *
     * \param[in]    roadId   Road ID for which to retrieve the length
     *
     * \return Length of the specified road in [m]
     */
    virtual double GetRoadLength(const std::string& roadId) const = 0;

    //! Calculates the obstruction of an agent with another object i.e. how far to left or the right the object is from my position
    //! For more information see the [markdown documentation](\ref dev_framework_modules_world_getobstruction)
    //!
    //! \param roadGraph        road network as viewed from agent
    //! \param startNode        position on roadGraph of agent
    //! \param ownPosition      position of the agent
    //! \param points           points of other object for which to calculate the obstruction
    //! \param touchedRoads     position of the other object
    //! \return obstruction with the other object
    virtual RouteQueryResult<Obstruction> GetObstruction(const RoadGraph &roadGraph, RoadGraphVertex startNode, const GlobalRoadPosition &ownPosition,
                                                         const std::map<ObjectPoint, Common::Vector2d> &points, const RoadIntervals &touchedRoads) const = 0;

    //! Returns all traffic signs valid for a lane inside the range
    //!
    //! \param roadGraph        road network as viewed from agent
    //! \param startNode        position on roadGraph of agent
    //! \param laneId           OpenDrive Id of the lane
    //! \param startDistance    s coordinate
    //! \param searchRange      range of search (can also be negative)
    //! \return traffic signs in range
    virtual RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetTrafficSignsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                            double startDistance, double searchRange) const = 0;

    //! Returns all road markings valid for a lane inside the range
    //!
    //! \param roadGraph        road network as viewed from agent
    //! \param startNode        position on roadGraph of agent
    //! \param laneId           OpenDrive Id of the lane
    //! \param startDistance    s coordinate
    //! \param searchRange      range of search
    //! \return road markings in range (can also be negative)
    virtual RouteQueryResult<std::vector<CommonTrafficSign::Entity>> GetRoadMarkingsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                            double startDistance, double searchRange) const = 0;

    //! Returns all traffic lights valid for a lane inside the range
    //!
    //! \param route            route to search along
    //! \param roadId           OpenDrive Id of the road
    //! \param laneId           OpenDrive Id of the lane
    //! \param startDistance    s coordinate
    //! \param searchRange      range of search (can also be negative)
    //! \return traffic lights in range
    virtual RouteQueryResult<std::vector<CommonTrafficLight::Entity>> GetTrafficLightsInRange(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId,
                                                                                             double startDistance, double searchRange) const = 0;

    //! Retrieves all lane markings on the given position on the given side of the lane inside the range
    //!
    //! \param roadGraph        road network as viewed from agent
    //! \param startNode        position on roadGraph of agent
    //! \param laneId           OpenDrive Id of the lane
    //! \param startDistance    s coordinate
    //! \param range            search range
    //! \param side             side of the lane
    virtual RouteQueryResult<std::vector<LaneMarking::Entity>> GetLaneMarkings(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double startDistance, double range, Side side) const = 0;

    //! Returns the relative distances (start and end) and the connecting road id of all junctions  on the route in range
    //!
    //! \param roadGraph        road network as viewed from agent
    //! \param startNode        position on roadGraph of agent
    //! \param startDistance    start s coordinate on the road
    //! \param range            range of search
    //! \return information about all junctions  in range
    [[deprecated]] virtual RouteQueryResult<RelativeWorldView::Roads> GetRelativeJunctions(const RoadGraph &roadGraph, RoadGraphVertex startNode, double startDistance, double range) const = 0;

    //! Returns the relative distances (start and end) and the road id of all roads on the route in range
    //!
    //! \param roadGraph        road network as viewed from agent
    //! \param startNode        position on roadGraph of agent
    //! \param startDistance    start s coordinate on the road
    //! \param range            range of search
    //! \return information about all roads in range
    virtual RouteQueryResult<RelativeWorldView::Roads> GetRelativeRoads(const RoadGraph& roadGraph, RoadGraphVertex startNode, double startDistance, double range) const = 0;

    //! Returns information about all lanes on the route in range. These info are the relative distances (start and end),
    //! the laneId relative to the ego lane, the successors and predecessors if existing and the information wether the intended
    //! driving direction of the lane is the same as the direction of the route. If the ego lane prematurely ends, then
    //! the further lane ids are relative to the middle of the road.
    //!
    //! \param roadGraph        road network as viewed from agent
    //! \param startNode        position on roadGraph of agent
    //! \param laneId           OpenDrive Id of the lane
    //! \param distance         start s coordinate on the road
    //! \param range            range of search
    //! \param includeOncoming  indicating whether oncoming lanes should be included
    //! \return information about all lanes in range
    virtual RouteQueryResult<RelativeWorldView::Lanes> GetRelativeLanes(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, double range, bool includeOncoming = true) const = 0;

    //! Returns the relative lane id of the located position of a point relative to the given position
    //!
    //! \param roadGraph        road network as viewed from agent
    //! \param startNode        position on roadGraph of agent
    //! \param laneId           OpenDrive Id of the lane
    //! \param distance         own s coordinate
    //! \param targetPosition   position of queried point
    //! \return lane id relative to own position
    virtual RouteQueryResult<std::optional<int>> GetRelativeLaneId(const RoadGraph& roadGraph, RoadGraphVertex startNode, int laneId, double distance, GlobalRoadPositions targetPosition) const = 0;

    //! Returns all possible connections on the junction, that an agent has when coming from the specified road
    //!
    //! \param junctionId       OpenDrive Id of the junction
    //! \param incomingRoadId   OpenDrive Id of the incoming road
    //! \return all connections on junctions with specified incoming road
    virtual std::vector<JunctionConnection> GetConnectionsOnJunction(std::string junctionId, std::string incomingRoadId) const = 0;

    //! Returns all priorities between the connectors of a junction
    //!
    //! \param junctionId   OpenDrive id of the junction
    virtual std::vector<JunctionConnectorPriority> GetPrioritiesOnJunction(std::string junctionId) const = 0;

    //! Returns the id of all roads on a junction that intersect with the given road and their rank compared to this road.
    //! Note: The rank is the rank of the intersecting road, i.e. "Higher" means that the intersecting road has higher priority
    //! then the given connectingRoad
    //!
    //! \param connectingRoadId     OpenDrive id of the road, for which intersections should be retrieved
    //! \return id and rank of all roads intersecting the connecting road
    virtual std::vector<IntersectingConnection> GetIntersectingConnections(std::string connectingRoadId) const = 0;

    //! Returns the Id and type (road or junction) if the next element of the road in stream direction.
    //! Returns tpye None if there is no successor.
    //!
    //! \param roadId   OpenDrive Id of road
    //! \return type and OpenDrive Id of next downstream element
    virtual RoadNetworkElement GetRoadSuccessor(std::string roadId) const = 0;

    //! Returns the Id and type (road or junction) if the next element of the road against stream direction.
    //! Returns tpye None if there is no predecessor.
    //!
    //! \param roadId   OpenDrive Id of road
    //! \return type and OpenDrive Id of next upstream element
    virtual RoadNetworkElement GetRoadPredecessor(std::string roadId) const = 0;

    //! Returns the relative road network as a tree starting the given RouteElement
    //!
    //! \param start                start element
    //! \param maxDepth             maximum depth of the tree
    //! \param inDrivingDirection   calculate tree in driving direction or against driving direction
    //! \return pair consisting of RoadGraph starting at start element and root vertex of this tree
    virtual std::pair<RoadGraph, RoadGraphVertex> GetRoadGraph(const RouteElement& start, int maxDepth, bool inDrivingDirection = true) const = 0;

    //! Returns the weight of the path for randomized route generation
    //!
    //! \param roadGraph    RoadGraph for which weights should be given
    //! \return map of weights for all edges in the graph
    virtual std::map<RoadGraphEdge, double> GetEdgeWeights (const RoadGraph& roadGraph) const = 0;

    //! Returns the RoadStream that is defined by the given route
    //!
    //! \param route    list of roads with direction
    //! \return RoadStream along route
    virtual std::unique_ptr<RoadStreamInterface> GetRoadStream(const std::vector<RouteElement>& route) const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the friction
    //!
    //! @return                friction
    //-----------------------------------------------------------------------------
    virtual double GetFriction() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns one agent which is set to be special.
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual AgentInterface* GetEgoAgent() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves agents that were removed from the world during the previous timestep and clears the list
    //!
    //! @return                List of agent references
    //-----------------------------------------------------------------------------
    virtual const std::vector<int> GetRemovedAgentsInPreviousTimestep() = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves all traffic objects
    //!
    //! @return                Traffic objects
    //-----------------------------------------------------------------------------
    virtual const std::vector<const TrafficObjectInterface*>& GetTrafficObjects() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns one agent with the specified scenarioName
    //!
    //! @return
    //-----------------------------------------------------------------------------
    virtual AgentInterface* GetAgentByName(const std::string& scenarioName) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the Radio for Car2X communication
    //!
    //! @return       radio
    //-----------------------------------------------------------------------------
    virtual RadioInterface& GetRadio() = 0;
};
