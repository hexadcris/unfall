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

template <typename ... QueryPack>
using Predicate = std::function<bool(QueryPack ...)>;

template <typename ... QueryPack>
using Compare = std::function<bool(std::pair<QueryPack, QueryPack>...)>;

template <typename T>
using ExecuteReturn =  std::tuple<std::vector<T>>;

template <typename T, typename Tag>
struct NamedType
{
    T value;

    NamedType (T value) :
        value(value) {}

    operator T() const
    {
        return value;
    }
};

using DistanceToEndOfLane = NamedType<double, struct DistanceType>;

//! Parameters of GetDistanceToEndOfLane
struct DistanceToEndOfLaneParameter
{
    double range;
    int relativeLane;
};

using ObjectsInRange = NamedType<std::vector<const WorldObjectInterface*>, struct ObjectsType>;

//! Parameters of GetObjectsInRange
struct ObjectsInRangeParameter
{
    double backwardRange;
    double forwardRange;
    int relativeLane;
};

enum class VelocityScope
{
    Absolute,
    Lateral,
    Longitudinal
};

//! This class represent an agent's (and especially driver's) view of his surroundings.
//! It provides various queries on the infrastructure and objects around the agent.
//! Most if these queries require, that a route for the agent is set which is the way the agent will supposedly drive through the road network.
//! There may also be alternative ways in the network, that the driver may evaluate, if his inteded route may for some reason be undesirable
class EgoAgentInterface
{

public:
    EgoAgentInterface() = default;
    EgoAgentInterface(const EgoAgentInterface &) = delete;
    EgoAgentInterface(EgoAgentInterface &&) = delete;
    EgoAgentInterface &operator=(const EgoAgentInterface &) = delete;
    EgoAgentInterface &operator=(EgoAgentInterface &&) = delete;
    virtual ~EgoAgentInterface() = default;

    //!Returns the corresponding AgentInterface
    //!
    //! \return corresponding AgentInterface
    virtual const AgentInterface* GetAgent () const = 0;

    //! This function sets the graph of the road network from the point of view
    //! of the agent, that should be considered as all possible routes of the agent.
    //! It also sets the prefered target destination in this graph.
    //!
    //! \param roadGraph    road network from the point of view of the agent, Note: Must be a tree
    //! \param current      root of the roadGraph (= current node of the agent)
    //! \param target       target in the roadGraph
    virtual void SetRoadGraph(const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target) = 0;


    //! Updates the EgoAgent for the next timestep, e.g. updates the roadGraph in the case the agent changes from one road to the next
    virtual void Update() = 0;


    //! Returns true if a route has been set and the agent is still on this route
    virtual bool HasValidRoute() const = 0;

    //! Chooses one of the possible alternatives and sets it as new target destination
    //!
    //! \param alternativeIndex     index of the alternative to set as new target
    virtual void SetNewTarget(size_t alternativeIndex) = 0;

    //! Returns the OpenDrive id of the road the agent is on (i.e. the front center).
    //! If the agent is one more than one road returns that which corresponds to the set route
    //!
    //! \return road id of the agent
    virtual const std::string& GetRoadId() const = 0;

    //! Returns the velocity of the own agent taking the route into account
    virtual double GetVelocity(VelocityScope velocityScope) const = 0;

    //! Returns the velocity of another object taking the own route into account
    //! This means that, if the other object is an agent driving in opposite direction the longitudinal velocity will be negative
    virtual double GetVelocity(VelocityScope velocityScope, const WorldObjectInterface* object) const = 0;

    //! Return the distane to the end of the driving lane (i.e. as defined by the corresponding
    //! WorldInterface function) along the set route. Returns infinity if the end is father away than the range
    //!
    //! \param range            maximum search range (calculated from MainLaneLocator)
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return remaining distance (calculated from MainLaneLocator)
    virtual double GetDistanceToEndOfLane(double range, int relativeLane = 0) const = 0;

    virtual double GetDistanceToEndOfLane(double range, int relativeLane, const LaneTypes& acceptableLaneTypes) const = 0;

    //! Returns all lane along the route relative the agent (i.e. the driving view of the agent)
    //!
    //! \param range            maximum search range (calculated from MainLaneLocator)
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return lanes relative to agent
    virtual RelativeWorldView::Lanes GetRelativeLanes(double range, int relativeLane = 0, bool includeOncoming = true) const = 0;

    //! Returns the relative lane of the ReferencePoint or MainLocatePoint of another object
    //!
    //! \param object   other object
    //! \param point    point of other object from which to get the lane
    //! \return relative lane id
    virtual std::optional<int> GetRelativeLaneId(const WorldObjectInterface* object, ObjectPoint point) const = 0;

    [[deprecated]] virtual RelativeWorldView::Roads GetRelativeJunctions(double range) const = 0;

    //! Returns the relative distances (start and end) and the road id of all roads on the route in range
    //!
    //! \param range            range of search
    //! \return information about all roads in range
    virtual RelativeWorldView::Roads GetRelativeRoads(double range) const = 0;

    //! Returns all WorldObjects around the agent inside the specified range on the specified
    //! lane along the route
    //!
    //! \param backwardRange    search range in driving direction (calculated from MainLaneLocator)
    //! \param forwardRange     search range against driving direction (calculated from MainLaneLocator)
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return objects in range
    virtual std::vector<const WorldObjectInterface*> GetObjectsInRange(double backwardRange, double forwardRange, int relativeLane = 0) const = 0;

    //! Returns all Agents around the agent inside the specified range on the specified
    //! lane along the route
    //!
    //! \param backwardRange    search range in driving direction (calculated from MainLaneLocator)
    //! \param forwardRange     search range against driving direction (calculated from MainLaneLocator)
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return agents in range
    virtual AgentInterfaces GetAgentsInRange(double backwardRange, double forwardRange, int relativeLane = 0) const = 0;

    //! Returns all TrafficSigns in front of the agent inside the specified range on the specified
    //! lane along the route
    //!
    //! \param range            search range (calculated from MainLaneLocator)
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return traffic signs in range
    virtual std::vector<CommonTrafficSign::Entity> GetTrafficSignsInRange(double range, int relativeLane = 0) const = 0;

    //! Returns all RoadMarkings in front of the agent inside the specified range on the specified
    //! lane along the route
    //!
    //! \param range            search range (calculated from MainLaneLocator)
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return road markings in range
    virtual std::vector<CommonTrafficSign::Entity> GetRoadMarkingsInRange(double range, int relativeLane = 0) const = 0;

    //! Returns all TrafficLights in front of the agent inside the specified range on the specified
    //! lane along the route
    //!
    //! \param range            search range (calculated from MainLaneLocator)
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return traffic lights in range
    virtual std::vector<CommonTrafficLight::Entity> GetTrafficLightsInRange(double range, int relativeLane = 0) const = 0;

    //! Returns all LaneMarkings in front of the agent inside the specified range on the specified
    //! lane along the route
    //!
    //! \param range            search range (calculated from MainLaneLocator)
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return lane markings in range
    virtual std::vector<LaneMarking::Entity> GetLaneMarkingsInRange(double range, Side side, int relativeLane = 0) const = 0;

    //! Returns the (longitudinal) distance to another object along the route
    //!
    //! \param otherObject  object to calculate distance to
    //! \param ownPoint     point of own agent used for calculation
    //! \param otherPoint   point of other object used for calculation
    //! \return distance to other object or nullopt if the other object is not on the route of this agent
    virtual std::optional<double> GetDistanceToObject(const WorldObjectInterface* otherObject, const ObjectPoint &ownPoint, const ObjectPoint &otherPoint) const = 0;

    virtual std::optional<double> GetNetDistance(const WorldObjectInterface* otherObject) const = 0;

    //! Returns the (lateral) obstruction with another object along the route
    //!
    //! \param otherObject  object to calculate obstruction
    //! \param points       list of points for which obstruction should be calculeted
    //! \return obstruction with other object
    virtual Obstruction GetObstruction(const WorldObjectInterface* otherObject, const std::vector<ObjectPoint> points) const = 0;


    //! Returns the yaw of the agent at the MainLaneLocater relative to the road respecting intended driving direction
    //! (i.e. driving in the intended direction equals zero relative yaw)
    virtual double GetRelativeYaw() const = 0;

    //! Returns the distance of the MainLaneLocator to the middle of the lane respecting intended driving direction
    //! (i.e. positive values are to the left w.r.t. driving direction)
    virtual double GetPositionLateral() const = 0;

    //! Returns the distance to the lane boundary on the specified side (or 0 if agent protrudes the road)
    virtual double GetLaneRemainder(Side side) const = 0;

    //! Returns the width of the specified lane at the current s coordinate
    //!
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return width of lane
    virtual double GetLaneWidth(int relativeLane = 0) const = 0;

    //! Returns the width of the specified lane at the specified distance if existing
    //!
    //! \param distance         search distance relative to MainLaneLocator
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return width of lane
    virtual std::optional<double> GetLaneWidth(double distance, int relativeLane = 0) const = 0;

    //! Returns the curvature of the specified lane at the current s coordinate
    //!
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return curvature of lane
    virtual double GetLaneCurvature(int relativeLane = 0) const = 0;

    //! Returns the curvature of the specified lane at the specified distance if existing
    //!
    //! \param distance         search distance relative to MainLaneLocator
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return curvature of lane
    virtual std::optional<double> GetLaneCurvature(double distance, int relativeLane = 0) const = 0;

    //! Returns the direction of the specified lane at the current s coordinate
    //!
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return direction of lane
    virtual double GetLaneDirection(int relativeLane = 0) const = 0;

    //! Returns the direction of the specified lane at the specified distance if existing
    //!
    //! \param distance         search distance relative to MainLaneLocator
    //! \param relativeLane     lane id relative to own lane (in driving direction)
    //! \return direction of lane
    virtual std::optional<double> GetLaneDirection(double distance, int relativeLane = 0) const = 0;

    //! Returns the position of the MainLaneLocator (w.r.t. the OpenDrive direction of the road)
    virtual const std::optional<GlobalRoadPosition>& GetMainLocatePosition() const = 0;

    //! Returns the position of the ReferencePoint if it is on the route
    virtual std::optional<GlobalRoadPosition> GetReferencePointPosition() const = 0;

    //! Return the OpenDrive lane id that is the specified amount to the left (in driving direction) of the own lane
    //! (i.e. negative values are to the right, and 0 returns the own lane)
    virtual int GetLaneIdFromRelative(int relativeLaneId) const = 0;

    //! Returns the world position that corresponds to the given distance from the reference point along the route,
    //! or std::nullopt, if the reference point is not on the route.
    //!
    //! \param sDistance    distance along s (in driving direction)
    //! \param tDistance    distance along t (left of driving direction)
    //! \param yaw          yaw relative to road at given distance
    //! \return world position at given distance
    virtual std::optional<Position> GetWorldPosition(double sDistance, double tDistance, double yaw = 0) const = 0;

    //!Returns the road positions of the given point of an object relative to the ego route
    //!
    //! \param point    point of object
    //! \param object   object of interest
    //! \return road positions of point
    virtual GlobalRoadPositions GetRoadPositions(const ObjectPoint& point, const WorldObjectInterface* object) const = 0;

    //! Returns all alternatives (i.e. leaves of the graph) that fulfill the filter function.
    //! First the appropriate WordInterface queries are called for the whole tree and then
    //! the result for each alternative is passed to the filter function.
    //!
    //! \param filter       function to filter the alternatives, all alternative where this returns true are kept
    //! \param parameters   parameters for the WorldInterface queries
    //! \return             indizes of filtered alternatives
    template<typename ... QueryPack, typename ... QueryParameters>
    std::vector<size_t> GetAlternatives(Predicate<QueryPack ...> filter, QueryParameters ... parameters) const
    {
        std::vector<size_t> indizes{};
        const auto results = executeQuery<QueryPack...>(std::forward<QueryParameters>(parameters)...);
        size_t numResults = std::get<0>(results).size();
        for (size_t i = 0; i < numResults; i++)
        {
            if(filter(std::get<std::vector<QueryPack>>(results).at(i)...))
            {
                indizes.push_back(i);
            }
        }
        return indizes;
    }

    //! Returns all alternatives (i.e. leaves of the graph) that fulfill the filter function sorted by the
    //! sort function. First the appropriate WordInterface queries are called for the whole tree and then
    //! the result for each alternative is passed to the filter and sort functions.
    //!
    //! \param filter       function to filter the alternatives, all alternative where this returns true are kept
    //! \param sort         function to compare two alternatives, must fulfill the requirements of a compare function
    //! \param parameters   parameters for the WorldInterface queries
    //! \return             indizes of filtered and sorted alternatives
    template<typename ... QueryPack, typename ... QueryParameters>
    std::vector<size_t> GetAlternatives(Predicate<QueryPack ...> filter,  Compare<QueryPack ...> sort, QueryParameters ... parameters) const
    {
        std::vector<size_t> indizes{};
        const auto results = executeQuery<QueryPack...>(std::forward<QueryParameters>(parameters)...);
        size_t numResults = std::get<0>(results).size();
        for (size_t i = 0; i < numResults; i++)
        {
            if(filter(std::get<std::vector<QueryPack>>(results).at(i)...))
            {
                indizes.push_back(i);
            }
        }
        std::sort(indizes.begin(), indizes.end() , [&](size_t i, size_t j){return sort(std::make_pair(std::get<std::vector<QueryPack>>(results).at(i), std::get<std::vector<QueryPack>>(results).at(j))...);});
        return indizes;
    }

protected:

    //! Excutes the specified (via template) WorldInterface query for all alternatives and returns the results
    //!
    //! \return     results for all alternatives
    template<typename Query, typename Parameter>
    ExecuteReturn<Query> executeQuery(Parameter) const {}

    //! Excutes the specified (via template) WorldInterface queries for all alternatives and returns the results
    //!
    //! \param param1   parameters for Query1
    //! \param params   parameters for the other queries
    //! \return     results for all alternatives
    template<typename Query1, typename Query2, typename ... QueryPack, typename Parameter1 , typename ... QueryParameters>
    std::tuple<std::vector<Query1>, std::vector<Query2>, std::vector<QueryPack>...> executeQuery(Parameter1 param1, QueryParameters ... params) const
    {
        return std::tuple_cat(executeQuery<Query1, Parameter1>(param1), executeQuery<Query2, QueryPack..., QueryParameters ...>(params...));
    }

    //! Returns the result of WorldInterface::GetDistanceToEndOfLane for all alternatives
    //!
    //! \param parameter    parameters for call of WorldInterface::GetDistanceToEndOfLane
    //! \return result for all alternatives
    virtual ExecuteReturn<DistanceToEndOfLane> executeQueryDistanceToEndOfLane(DistanceToEndOfLaneParameter parameter) const = 0;

    //! Returns the result of WorldInterface::GetObjectsInRange for all alternatives
    //!
    //! \param parameter    parameters for call of WorldInterface::GetObjectsInRange
    //! \return result for all alternatives
    virtual ExecuteReturn<ObjectsInRange> executeQueryObjectsInRange(ObjectsInRangeParameter parameter) const = 0;
};

template<>
ExecuteReturn<DistanceToEndOfLane> EgoAgentInterface::executeQuery(DistanceToEndOfLaneParameter param) const;

template<>
ExecuteReturn<ObjectsInRange> EgoAgentInterface::executeQuery(ObjectsInRangeParameter param) const;
