/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2018-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  OpenDriveTypeMapper.h
//! @brief This file provides conversions from OpenDRIVE constants to OSI
//!        constants
//-----------------------------------------------------------------------------

#pragma once

#include "common/globalDefinitions.h"

#include "osi3/osi_lane.pb.h"
#include "osi3/osi_trafficsign.pb.h"
#include "osi3/osi_trafficlight.pb.h"

#include "include/roadInterface/roadElementTypes.h"
#include "common/globalDefinitions.h"
#include "include/worldInterface.h"

namespace OWL
{
enum class LaneMarkingSide;
}


namespace OpenDriveTypeMapper
{
    //-----------------------------------------------------------------------------
    //! Convert OpenDRIVE lane type to OSI lane type
    //!
    //! @param[in] odLaneType             OpenDRIVE lane type
    //! @return                           OSI lane type
    //-----------------------------------------------------------------------------
    osi3::Lane_Classification_Type OdToOsiLaneType(const RoadLaneType odLaneType);

    //-----------------------------------------------------------------------------
    //! Convert OpenDRIVE road signal unit to OSI traffic sign unit
    //!
    //! @param[in] Unit                   OpenDRIVE road signal unit
    //! @return                           OSI traffic sign unit
    //-----------------------------------------------------------------------------
    osi3::TrafficSignValue_Unit OdToOsiTrafficSignUnit(const RoadSignalUnit unit);

    //-----------------------------------------------------------------------------
    //! Convert OpenDRIVE road mark types to OSI lane boundary types
    //!
    //! @param[in] type                 OpenDRIVE road mark type
    //! @return                         OSI lane boundary type
    //-----------------------------------------------------------------------------
    osi3::LaneBoundary::Classification::Type OdToOsiLaneMarkingType(RoadLaneRoadMarkType type, OWL::LaneMarkingSide side);

    //-----------------------------------------------------------------------------
    //! Convert OpenDRIVE road mark colors to OSI lane boundary colors
    //!
    //! @param[in] color                OpenDRIVE road mark color
    //! @return                         OSI lane boundary color
    //-----------------------------------------------------------------------------
    osi3::LaneBoundary::Classification::Color OdToOsiLaneMarkingColor(RoadLaneRoadMarkColor color);

    //-----------------------------------------------------------------------------
    //! Convert OpenDRIVE lane type to lane type defined in worldDefinitions.h
    //!
    //! @param[in] color                OSI lane type
    //! @return                         lane type
    //-----------------------------------------------------------------------------
    LaneType OdToOwlLaneType(const RoadLaneType laneType);

    //-----------------------------------------------------------------------------
    //! Convert OSI lane boundary colors to lane marking colors defined in worldDefintions.h
    //!
    //! @param[in] color                OSI road mark color
    //! @return                         lane marking color
    //-----------------------------------------------------------------------------
    LaneMarking::Color OsiToOdLaneMarkingColor(const osi3::LaneBoundary_Classification_Color color);

    //-----------------------------------------------------------------------------
    //! Convert OSI lane boundary type to lane marking type defined in worldDefintions.h
    //!
    //! @param[in] color                OSI road mark type
    //! @return                         lane marking type
    //-----------------------------------------------------------------------------
    LaneMarking::Type OsiToOdLaneMarkingType(const osi3::LaneBoundary_Classification_Type type);

    //! Defains for a specific country the mapping from OpenDrive Signals to OSI
    struct SignalsMapping
    {
        //! Conversion map from OpenDrive traffic sign to OSI for traffic signs, where the type combined with the subtype defines the value of the sign
        const std::map<std::string, std::pair<osi3::TrafficSignValue_Unit, std::map<std::string, std::pair<double, osi3::TrafficSign_MainSign_Classification_Type>>>> trafficSignsPredefinedValueAndUnit;

        //! Conversion map from OpenDrive traffic sign to OSI for traffic signs, where the subtype is the numeric value of the sign
        const std::map<std::string, std::pair<osi3::TrafficSign_MainSign_Classification_Type, osi3::TrafficSignValue_Unit>> trafficSignsSubTypeDefinesValue;

        //! Conversion map from OpenDrive traffic sign to OSI for traffic signs, that have no subtype and value
        const std::map<std::string, osi3::TrafficSign_MainSign_Classification_Type> trafficSignsTypeOnly;

        //! Conversion map from OpenDrive traffic sign to OSI for traffic signs with separately defined value
        const std::map<std::string, osi3::TrafficSign_MainSign_Classification_Type> trafficSignsWithValue;

        //! Conversion map from OpenDrive traffic sign to OSI for traffic signs with text
        const std::map<std::string, osi3::TrafficSign_MainSign_Classification_Type> trafficSignsWithText;

        //! Conversion map from OpenDrive traffic sign to OSI for road markings
        const std::map<std::string, osi3::TrafficSign_MainSign_Classification_Type> roadMarkings;
    };

    const SignalsMapping signalsMappingDE
    {
        //trafficSignsPredefinedValueAndUnit
        {
            {"274.1", {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR,
                            {
                                {"",    {30.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_ZONE_BEGIN}},
                                {"20",  {20.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_ZONE_BEGIN}}
                            }
                      }
            },
            {"274.2", {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR,
                            {
                                {"",    {30.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_ZONE_END}},
                                {"20",  {20.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_ZONE_END}}
                            }
                      }
            },
            {"450", {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER,
                            {
                                {"50", {100.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_POLE_EXIT}},
                                {"51", {200.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_POLE_EXIT}},
                                {"52", {300.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_POLE_EXIT}}
                            }
                    }
            },
            {"531", {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_NO_UNIT,
                            {
                                {"10", {1.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_RIGHT_LANE_END}},
                                {"11", {2.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_RIGHT_LANE_END}},
                                {"12", {3.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_RIGHT_LANE_END}},
                                {"13", {4.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_RIGHT_LANE_END}},
                                {"20", {1.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_LEFT_LANE_END}},
                                {"21", {2.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_LEFT_LANE_END}},
                                {"22", {3.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_LEFT_LANE_END}},
                                {"23", {4.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_LEFT_LANE_END}}
                            }
                         }
            }
        },

        //trafficSignsSubTypeDefinesValue
        {
            {"274", {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_BEGIN, osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR}},
            {"275", {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_MINIMUM_SPEED_BEGIN, osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR}},
            {"278", {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_END, osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR}},
            {"279", {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_MINIMUM_SPEED_END, osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR}}
        },

        //trafficSignsTypeOnly
        {
            {"-1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER},
            {"none", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER},
            {"205", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_GIVE_WAY},
            {"206", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_STOP},
            {"267", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_DO_NOT_ENTER},
            {"270.1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ENVIRONMENTAL_ZONE_BEGIN},
            {"270.2", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ENVIRONMENTAL_ZONE_END},
            {"276", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_BEGIN},
            {"277", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_FOR_TRUCKS_BEGIN},
            {"280", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_END},
            {"281", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_FOR_TRUCKS_END},
            {"282", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ALL_RESTRICTIONS_END},
            {"301", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_RIGHT_OF_WAY_NEXT_INTERSECTION},
            {"306", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_RIGHT_OF_WAY_BEGIN},
            {"307", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_RIGHT_OF_WAY_END},
            {"325.1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_LIVING_STREET_BEGIN},
            {"325.2", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_LIVING_STREET_END},
            {"330.1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_BEGIN},
            {"330.2", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_END},
            {"333", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_EXIT}
        },

        //trafficSignsWithValue
        {
            //Not applicable for DE
        },

        //trafficSignsWithText
        {
            {"310", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_TOWN_BEGIN},
            {"311", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_TOWN_END},
            {"448", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_ANNOUNCEMENT}
        },

        //roadMarkings
        {
            {"293", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ZEBRA_CROSSING},
            {"294", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_STOP},
            {"1.000.003", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ZEBRA_CROSSING},
        }
    };

    const SignalsMapping signalsMappingUS
    {

        //trafficSignsPredefinedValueAndUnit
        {
        },

        //trafficSignsSubTypeDefinesValue
        {
            //Not applicable for US
        },

        //trafficSignsTypeOnly
        {
            {"R1-1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_STOP},
            {"R1-2", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_GIVE_WAY},
            {"R4-1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_BEGIN},
            {"R5-1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_DO_NOT_ENTER},
            {"E5-1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_EXIT},
        },

        //trafficSignsWithValue
        {
            {"R2-1", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_BEGIN},
        },

        //trafficSignsWithText
        {
        },

        //roadMarkings
    };

    const SignalsMapping signalsMappingCN
    {

        //trafficSignsPredefinedValueAndUnit
        {
            {"EndofLane-32", {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_NO_UNIT,
                                {
                                    {"a", {2.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_RIGHT_LANE_END}},
                                    {"b", {3.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_RIGHT_LANE_END}},
                                    {"c", {2.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_LEFT_LANE_END}},
                                    {"d", {3.0, osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_LEFT_LANE_END}}
                                }
                              }
            }
        },

        //trafficSignsSubTypeDefinesValue
        {
        },

        //trafficSignsTypeOnly
        {
            {"HighwayExit-50d", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_EXIT},
        },

        //trafficSignsWithValue
        {
            {"SpeedLimit-38", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_BEGIN},
            {"EndOfSpeedLimit-39", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_END},
        },

        //trafficSignsWithText
        {
            {"AnnounceHighwayExit-50b", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_ANNOUNCEMENT},
            {"AnnounceHighwayExit-50c", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_ANNOUNCEMENT},
            {"HighwayExitPole-55", osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_POLE_EXIT},
        },

        //roadMarkings
    };

    const SignalsMapping* GetSignalsMapping (const std::string& country);


    const std::map<std::string, std::map<std::string, osi3::TrafficLight_Classification_Icon>> ThreeSignalsTrafficLightsIcons =
            {
                    {"1.000.001",
                            {
                                    {"",   osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_NONE}
                            }
                    },
                    {"1.000.011",
                            {
                                    {"10", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT},
                                    {"20", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT},
                                    {"30", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD},
                                    {"40", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT},
                                    {"50", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT}
                            }
                    },
                    {"1.000.013",
                            {
                                    {"10",   osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_BICYCLE}
                            }
                    }
            };

    const std::map<std::string, std::map<std::string, osi3::TrafficLight_Classification_Icon>> TwoSignalsTrafficLightsIcons =
            {
                    {"1.000.002",
                            {
                                    {"", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN}
                            }
                    },
                    {"1.000.007",
                            {
                                    {"", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE}
                            }
                    },
                    {"1.000.009",
                            {
                                    {"10", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_NONE},
                                    {"20", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_NONE},
                                    {"30", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_NONE}
                            }
                    },
                    {"1.000.010",
                            {
                                    {"10", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT},
                                    {"20", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT}
                            }
                    },
                    {"1.000.013",
                            {
                                    {"", osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_BICYCLE}
                            }
                    },
            };

    const std::map<std::string, std::map<std::string, std::vector<osi3::TrafficLight_Classification_Color>>> TwoSignalsTrafficLightsColors =
            {
                    {"1.000.002",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED,
                                                   osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}}
                            }
                    },
                    {"1.000.007",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED,
                                                   osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}}
                            }
                    },
                    {"1.000.009",
                            {
                                    {"10", {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED,
                                                   osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}
                                    },
                                    {"20", {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW,
                                                   osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}
                                    },
                                    {"30", {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED,
                                                   osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}
                                    }
                            }
                    },
                    {"1.000.010",
                            {
                                    {"10",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW,
                                                   osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"20",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW,
                                                     osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}}
                            }
                    },
                    {"1.000.013",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED,
                                                   osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}}
                            }
                    },
            };

    const std::map<std::string, std::map<std::string, osi3::TrafficLight_Classification_Icon>> OneSignalTrafficLightsIcons =
            {
                    {"1.000.002",
                            {
                                    {"10",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN}},
                                    {"20",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN}}
                            }
                    },
                    {"1.000.007",
                            {
                                    {"10",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE}},
                                    {"20",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE}},
                                    {"30",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE}}
                            }
                    },
                    {"1.000.008",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_NONE}},
                                    {"10",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT}},
                                    {"20",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT}},
                                    {"30",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD}},
                                    {"40",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT}},
                                    {"50",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT}},
                                    {"60",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_LEFT}},
                                    {"70",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_RIGHT}}
                            }
                    },
                    {"1.000.012",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_NONE}},
                                    {"10",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT}},
                                    {"20",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT}},
                                    {"30",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD}},
                                    {"40",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT}},
                                    {"50",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT}},
                                    {"60",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_LEFT}},
                                    {"70",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_RIGHT}}
                            }
                    },
                    {"1.000.020",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_NONE}},
                                    {"10",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT}},
                                    {"20",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT}},
                                    {"30",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD}},
                                    {"40",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT}},
                                    {"50",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT}},
                                    {"60",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_LEFT}},
                                    {"70",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_RIGHT}}
                            }
                    },
                    {"1.000.013",
                            {
                                    {"20",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_BICYCLE}},
                                    {"30",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_BICYCLE}},
                                    {"40",   {osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_BICYCLE}}
                            }
                    },
            };

    const std::map<std::string, std::map<std::string, std::vector<osi3::TrafficLight_Classification_Color>>> OneSignalTrafficLightsColors =
            {
                    {"1.000.002",
                            {
                                    {"10",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"20",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}}
                            }
                    },
                    {"1.000.007",
                            {
                                    {"10",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"20",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"30",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}}
                            }
                    },
                    {"1.000.008",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"10",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"20",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"30",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"40",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"50",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"60",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"70",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}}
                            }
                    },
                    {"1.000.012",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"10",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"20",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"30",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"40",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"50",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"60",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"70",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}}
                            }
                    },
                    {"1.000.020",
                            {
                                    {"",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"10",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"20",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"30",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"40",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"50",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"60",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"70",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}}
                            }
                    },
                    {"1.000.013",
                            {
                                    {"20",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"30",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"40",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"50",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"60",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"70",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}},
                                    {"80",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED}},
                                    {"90",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW}},
                                    {"100",   {osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN}}
                            }
                    },
            };

    //! Conversion map from OSI units to SI units
    const std::map<osi3::TrafficSignValue_Unit, std::pair<double, CommonTrafficSign::Unit>> unitConversionMap =
    {
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR, {0.27778, CommonTrafficSign::Unit::MeterPerSecond}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MILE_PER_HOUR, {0.44704, CommonTrafficSign::Unit::MeterPerSecond}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER, {1000.0, CommonTrafficSign::Unit::Meter}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER, {1.0, CommonTrafficSign::Unit::Meter}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MILE, {1609.34, CommonTrafficSign::Unit::Meter}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_FEET, {0.3048, CommonTrafficSign::Unit::Meter}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_PERCENTAGE, {1.0, CommonTrafficSign::Unit::Percentage}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METRIC_TON, {1000.0, CommonTrafficSign::Unit::Kilogram}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_SHORT_TON, {907.185, CommonTrafficSign::Unit::Kilogram}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_LONG_TON, {1016.05, CommonTrafficSign::Unit::Kilogram}},
        {osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MINUTES, {60.0, CommonTrafficSign::Unit::Second}}
    };

    //! Conversion map from OSI traffic sign type to enum defined in globalDefinitions
    const std::map<osi3::TrafficSign_MainSign_Classification_Type, CommonTrafficSign::Type> typeConversionMap =
    {
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_STOP, CommonTrafficSign::Type::Stop},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_GIVE_WAY, CommonTrafficSign::Type::GiveWay},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_DO_NOT_ENTER, CommonTrafficSign::Type::DoNotEnter},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ENVIRONMENTAL_ZONE_BEGIN, CommonTrafficSign::Type::EnvironmentalZoneBegin},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ENVIRONMENTAL_ZONE_END, CommonTrafficSign::Type::EnvironmentalZoneEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_BEGIN, CommonTrafficSign::Type::MaximumSpeedLimit},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_ZONE_BEGIN, CommonTrafficSign::Type::SpeedLimitZoneBegin},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_ZONE_END, CommonTrafficSign::Type::SpeedLimitZoneEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_MINIMUM_SPEED_BEGIN, CommonTrafficSign::Type::MinimumSpeedLimit},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_BEGIN, CommonTrafficSign::Type::OvertakingBanBegin},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_FOR_TRUCKS_BEGIN, CommonTrafficSign::Type::OvertakingBanTrucksBegin},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_END, CommonTrafficSign::Type::EndOfMaximumSpeedLimit},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_MINIMUM_SPEED_END, CommonTrafficSign::Type::EndOfMinimumSpeedLimit},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_END, CommonTrafficSign::Type::OvertakingBanEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_FOR_TRUCKS_END, CommonTrafficSign::Type::OvertakingBanTrucksEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ALL_RESTRICTIONS_END, CommonTrafficSign::Type::EndOffAllSpeedLimitsAndOvertakingRestrictions},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_RIGHT_OF_WAY_NEXT_INTERSECTION, CommonTrafficSign::Type::RightOfWayNextIntersection},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_RIGHT_OF_WAY_BEGIN, CommonTrafficSign::Type::RightOfWayBegin},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_RIGHT_OF_WAY_END, CommonTrafficSign::Type::RightOfWayEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_TOWN_BEGIN, CommonTrafficSign::Type::TownBegin},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_TOWN_END, CommonTrafficSign::Type::TownEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_ANNOUNCEMENT, CommonTrafficSign::Type::AnnounceHighwayExit},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_LIVING_STREET_BEGIN, CommonTrafficSign::Type::TrafficCalmedDistrictBegin},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_LIVING_STREET_END, CommonTrafficSign::Type::TrafficCalmedDistrictEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_BEGIN, CommonTrafficSign::Type::HighWayBegin},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_END, CommonTrafficSign::Type::HighWayEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_EXIT, CommonTrafficSign::Type::HighWayExit},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_POLE_EXIT, CommonTrafficSign::Type::HighwayExitPole},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_RIGHT_LANE_END, CommonTrafficSign::Type::AnnounceRightLaneEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ANNOUNCE_LEFT_LANE_END, CommonTrafficSign::Type::AnnounceLeftLaneEnd},
        {osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ZEBRA_CROSSING, CommonTrafficSign::Type::PedestrianCrossing}
    };

    //! Conversion map from OSI traffic sign type to enum defined in globalDefinitions
    const std::map<osi3::TrafficLight_Classification_Icon, CommonTrafficLight::Type> trafficLightTypeThreeLightsConversionMap =
    {
        {osi3::TrafficLight_Classification_Icon_ICON_NONE, CommonTrafficLight::Type::ThreeLights},
        {osi3::TrafficLight_Classification_Icon_ICON_ARROW_LEFT, CommonTrafficLight::Type::ThreeLightsLeft},
        {osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT, CommonTrafficLight::Type::ThreeLightsRight},
        {osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD, CommonTrafficLight::Type::ThreeLightsStraight},
        {osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT, CommonTrafficLight::Type::ThreeLightsLeftStraight},
        {osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT, CommonTrafficLight::Type::ThreeLightsRightStraight}
    };

    //! Conversion map from OSI traffic sign type to enum defined in globalDefinitions
    const std::map<osi3::TrafficLight_Classification_Icon, CommonTrafficLight::Type> trafficLightTypeTwoLightsConversionMap =
    {
        {osi3::TrafficLight_Classification_Icon_ICON_NONE, CommonTrafficLight::Type::TwoLights},
        {osi3::TrafficLight_Classification_Icon_ICON_PEDESTRIAN, CommonTrafficLight::Type::TwoLightsPedestrian},
        {osi3::TrafficLight_Classification_Icon_ICON_BICYCLE, CommonTrafficLight::Type::TwoLightsBicycle},
        {osi3::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE, CommonTrafficLight::Type::TwoLightsPedestrianBicycle}
    };

    const std::map<osi3::TrafficLight_Classification_Icon, CommonTrafficLight::Type> trafficLightTypeOneLightConversionMap =
    {
        {osi3::TrafficLight_Classification_Icon_ICON_NONE,                   CommonTrafficLight::Type::OneLight},
        {osi3::TrafficLight_Classification_Icon_ICON_PEDESTRIAN,             CommonTrafficLight::Type::TwoLightsPedestrian},
        {osi3::TrafficLight_Classification_Icon_ICON_BICYCLE,                CommonTrafficLight::Type::TwoLightsBicycle},
        {osi3::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE, CommonTrafficLight::Type::TwoLightsPedestrianBicycle}
    };
};
