/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2019-2021 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <limits>

#include "common/globalDefinitions.h"
#include <optional>
#include <set>
#include "boost/graph/adjacency_list.hpp"

//! Double values with difference lower than this should be considered equal
constexpr double EQUALITY_BOUND = 1e-3;

//! Type of element in RoadNetwork
enum class RoadNetworkElementType
{
    Road,
    Junction,
    None
};

//! Element in RoadNetwork (as used as successor/predecessor of a road in OpenDRIVE)
struct RoadNetworkElement
{
    RoadNetworkElementType type;
    std::string id;

    RoadNetworkElement() = default;
    RoadNetworkElement(RoadNetworkElementType type, std::string id) :
        type(type),
        id(id)
    {}
};

//! Single connection of a junction
struct JunctionConnection
{
    std::string connectingRoadId;
    std::string outgoingRoadId;
    bool outgoingStreamDirection;
};

//! Priority defintion of two crossing connections on a junction
//! (i.e. defines which vehicle has right of way)
struct JunctionConnectorPriority
{
    JunctionConnectorPriority(std::string high, std::string low) :
        high(high),
        low(low)
    {}
    std::string high; //!id of connecting road with higher priority
    std::string low; //!id of connecting road with lower priority
};

//!Rank of one junction connection w.r.t. another
enum class IntersectingConnectionRank
{
    Undefined,
    Higher,
    Lower
};

//! Information regarding a connecting road intersecting another
struct IntersectingConnection
{
    std::string id;
    IntersectingConnectionRank rank;
    bool operator== (const IntersectingConnection& other) const
    {
        return (id == other.id) && (rank == other.rank);
    }
};

//! Type of a lane
enum class LaneType
{
    Undefined = 0,
    Shoulder,
    Border,
    Driving,
    Stop,
    None,
    Restricted,
    Parking,
    Median,
    Biking,
    Sidewalk,
    Curb,
    Exit,
    Entry,
    OnRamp,
    OffRamp,
    ConnectingRamp,
    Tram,
    RoadWorks,
    Bidirectional
};

//! interval on a road over multiple lanes
struct LaneSection
{
    double startS;
    double endS;
    std::vector<int> laneIds;
};

using LaneSections = std::vector<LaneSection>;

namespace RelativeWorldView {
//! Lane as viewed relative to a position / agent
struct Lane
{
    int relativeId;
    bool inDrivingDirection;
    LaneType type;
    std::optional<int> predecessor;
    std::optional<int> successor;

    bool operator==(const Lane& other) const
    {
        return relativeId == other.relativeId
                && inDrivingDirection == other.inDrivingDirection
                && type == other.type
                && predecessor == other.predecessor
                && successor == other.successor;
    }
};

//! interval on a road over multiple lanes relative to a position / agent
struct LanesInterval
{
    double startS;
    double endS;
    std::vector<Lane> lanes;
};

//! Relative view of a portion of the road network
//! as viewed from a specific position / agent
using Lanes = std::vector<LanesInterval>;

//! Position of a road relative to an agent
struct Road
{
    double startS; //!< relative distance to start of road
    double endS; //!< relative distance to end of road
    std::string roadId; //!< id of the road
    bool junction; //!< whether the road is part of a junction
    bool inOdDirection; //!< whether the agent is driving in reference line direction (i.e. increasing s coordinate)
};

using Roads = std::vector<Road>;
} //end of namespace RelativeWorldView


//! Position of a point on a specific lane
struct RoadPosition
{
    RoadPosition() = default;
    RoadPosition(double s,
                 double t,
                 double hdg):
        s(s),
        t(t),
        hdg(hdg) {}

    double s {0};
    double t {0};
    double hdg {0};

    bool operator==(const RoadPosition& other) const
    {
        return std::abs(s - other.s) < EQUALITY_BOUND
                && std::abs(t - other.t) < EQUALITY_BOUND
                && std::abs(hdg - other.hdg) < EQUALITY_BOUND;
    }
};

//! Position of a point in the road network
struct GlobalRoadPosition
{
    GlobalRoadPosition() = default;
    GlobalRoadPosition(std::string roadId, int laneId, double s, double t, double hdg) :
        roadId{roadId},
        laneId{laneId},
        roadPosition(s, t, hdg)
    {}

    bool operator==(const GlobalRoadPosition& other) const
    {
        return roadId == other.roadId
                && laneId == other.laneId
                && roadPosition == other.roadPosition;
    }

    std::string roadId {};
    int laneId {-999};
    RoadPosition roadPosition {};
};

//! A single point in the world may have multiple RoadPositions (on intersecting roads), represented by this map
//! The key is the id of the road and the value is the position on this road
using GlobalRoadPositions = std::map<std::string, GlobalRoadPosition>;

//! This struct describes how much space an agent has to next lane boundary on both sides
struct Remainder
{
    Remainder() = default;
    Remainder(double left, double right) : left{left}, right{right}
    {}

    double left {0.0};
    double right {0.0};
};

//! Interval on a specific road
struct RoadInterval
{
    std::vector<int> lanes;
    GlobalRoadPosition sMin{"", 0, std::numeric_limits<double>::infinity(), 0, 0};
    GlobalRoadPosition sMax{"", 0, -std::numeric_limits<double>::infinity(), 0, 0};
    GlobalRoadPosition tMin{"", 999, 0, std::numeric_limits<double>::infinity(), 0};
    GlobalRoadPosition tMax{"", -999, 0, -std::numeric_limits<double>::infinity(), 0};
};

using RoadIntervals = std::map<std::string, RoadInterval>;

//! Specific point of the bounding box of an object (or the reference point)
enum class ObjectPointPredefined
{
    Reference,
    Center,
    FrontCenter,
    RearCenter,
    FrontLeft,
    FrontRight,
    RearLeft,
    RearRight
};

//! Point relative to another agents route
enum class ObjectPointRelative
{
    Rearmost,
    Frontmost,
    Leftmost,
    Rightmost
};

//! Point of an object for costum use (e.g. sensor position)
struct ObjectPointCustom
{
    double longitudinal;
    double lateral;

    bool operator== (const ObjectPointCustom& other) const
    {
        return std::abs(longitudinal - other.longitudinal) < EQUALITY_BOUND
                && std::abs(lateral - other.lateral) < EQUALITY_BOUND;
    }

    //Needed for map sorting
    bool operator< (const ObjectPointCustom& other) const
    {
        return longitudinal < other.longitudinal
                || ((longitudinal == other.longitudinal) && (lateral < other.lateral));
    }
};

//! Point of an object used for object related queries that are specific for a point instead of an area
using ObjectPoint = std::variant<ObjectPointPredefined, ObjectPointRelative, ObjectPointCustom>;

//! This represents one node on the road network graph of the world or in the routing graph of an agent.
struct RouteElement
{
    std::string roadId;
    bool inOdDirection{false};

    RouteElement() = default;

    RouteElement (std::string roadId, bool inOdDirection) :
        roadId(roadId),
        inOdDirection(inOdDirection)
    {}

    bool operator==(const RouteElement& other) const
    {
        return roadId == other.roadId
                && inOdDirection == other.inOdDirection;
    }

    bool operator<(const RouteElement& other) const
    {
        return inOdDirection < other.inOdDirection
                || (inOdDirection == other.inOdDirection && roadId < other.roadId);
    }

    typedef boost::vertex_property_tag kind;
};

using RouteElementProperty = boost::property<RouteElement, RouteElement>;

//! Directed graph representing the road network.
//! For each road there is one vertex for each possible driving direction.
//! An edge between two vertices means, that an agent driving on the first road in this direction can after the road ends
//! continue its way on the second road in the given direction.
using RoadGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, RouteElementProperty>;
using RoadGraphVertex = boost::graph_traits<RoadGraph>::vertex_descriptor;
using RoadGraphEdge = boost::graph_traits<RoadGraph>::edge_descriptor;
using RoadGraphVertexMapping = std::map<RouteElement, RoadGraphVertex>;

template <typename T>
using RouteQueryResult = std::map<RoadGraphVertex, T>;

//! @brief Describes the obstruction of an opposing object within the driving lanes
///
class Obstruction
{
public:
    bool valid {false};   ///!< @brief True, if obstruction could be calculated
    std::map<ObjectPoint, double> lateralDistances{}; ///!< @brief obstructions for various points (defined during call of the query)

    Obstruction(std::map<ObjectPoint, double> lateralDistances) :
        valid{true},
        lateralDistances{lateralDistances}
    {
    }

    Obstruction() = default;

    /// \return Invalid obstruction object
    static Obstruction Invalid()
    {
        return {};
    }
};

enum class MeasurementPoint
{
    Front,
    Reference,
    Rear
};

namespace CommonTrafficSign {
enum Type
{
    Undefined = 0,
    GiveWay = 205,
    Stop = 206,
    DoNotEnter = 267,
    EnvironmentalZoneBegin = 2701, // 270.1
    EnvironmentalZoneEnd = 2702, // 270.2
    MaximumSpeedLimit = 274,
    SpeedLimitZoneBegin = 2741, //274.1
    SpeedLimitZoneEnd = 2742, // 274.2
    MinimumSpeedLimit = 275,
    OvertakingBanBegin = 276,
    OvertakingBanTrucksBegin = 277,
    EndOfMaximumSpeedLimit = 278,
    EndOfMinimumSpeedLimit = 279,
    OvertakingBanEnd = 280,
    OvertakingBanTrucksEnd = 281,
    EndOffAllSpeedLimitsAndOvertakingRestrictions = 282,
    PedestrianCrossing = 293,
    RightOfWayNextIntersection = 301,
    RightOfWayBegin = 306,
    RightOfWayEnd = 307,
    TownBegin = 310,
    TownEnd = 311,
    TrafficCalmedDistrictBegin = 3251, // 325.1
    TrafficCalmedDistrictEnd = 3252, // 325.2
    HighWayBegin = 3301, // 330.1
    HighWayEnd = 3302, // 330.2
    HighWayExit = 333,
    HighwayExitPole = 450, // 450-(50/51/52),
    AnnounceHighwayExit = 448,
    AnnounceRightLaneEnd = 5311, // 531-(10/11/12/13)
    AnnounceLeftLaneEnd = 5312, // 531-(20/21/22/23)
    DistanceIndication = 1004 // 1004-(30/31/32/33)
};

enum Unit
{
    None = 0,
    Kilogram,
    MeterPerSecond,
    Meter,
    Percentage,
    Second
};

struct Entity
{
    Type type {Type::Undefined};
    Unit unit {Unit::None};
    double distanceToStartOfRoad {0};
    double relativeDistance {0};
    double value {0};
    std::string text {""};
    std::vector<Entity> supplementarySigns{};
};
}

namespace LaneMarking
{
enum class Type
{
    None,
    Solid,
    Broken,
    Solid_Solid,
    Solid_Broken,
    Broken_Solid,
    Broken_Broken,
    Grass,
    Botts_Dots,
    Curb
};

enum class Color
{
    White,
    Yellow,
    Red,
    Blue,
    Green,
    Other
};

struct Entity
{
    Type type{Type::None};
    Color color{Color::White};
    double relativeStartDistance{0.0};
    double width{0.0};
};
}

namespace CommonTrafficLight
{
//! Type of a traffic light
enum class Type
{
    Undefined,
    ThreeLights,                //! Standard red, yellow, green without arrows or symbols
    ThreeLightsLeft,            //! red, yellow, green with arrows pointing left
    ThreeLightsRight,           //! red, yellow, green with arrows pointing right
    ThreeLightsStraight,        //! red, yellow, green with arrows pointing upwards
    ThreeLightsLeftStraight,    //! red, yellow, green with arrows pointing left and upwards
    ThreeLightsRightStraight,   //! red, yellow, green with arrows pointing right and upwards
    TwoLights,                  //! red, green, yellow green, red yellow, without arrows or symbols
    TwoLightsPedestrian,        //! red, green with pedestrian symbol
    TwoLightsBicycle,           //! red, green with bicycle symbol
    TwoLightsPedestrianBicycle, //! red, green with pedestrian and bicycle symbol
    OneLight,                   //! red, yellow, or green with no symbol
    OneLightPedestrian,         //! red, yellow, or green with pedestrian symbol
    OneLightBicycle,            //! red, yellow, or green with bicycle symbol
    OneLightPedestrianBicycle   //! red, yellow, or green with pedestrian and bicycle symbol
};

//! State of a traffic light
enum class State
{
    Off,
    Green,
    Yellow,
    Red,
    RedYellow,
    YellowFlashing,
    Unknown
};



//! Represents a single traffic light as seen from an agent
struct Entity
{
    Type type{Type::ThreeLights};
    State state{State::Red};
    double relativeDistance{0.0};
};
}
