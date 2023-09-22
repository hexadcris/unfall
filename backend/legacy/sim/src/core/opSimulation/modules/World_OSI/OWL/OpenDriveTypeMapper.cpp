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
//! @file  OpenDriveTypeMapper.cpp
//! @brief This file provides conversions from OpenDRIVE constants to OSI
//!        constants
//-----------------------------------------------------------------------------


#include "OpenDriveTypeMapper.h"

#include "osi3/osi_lane.pb.h"
#include "OWL/DataTypes.h"

osi3::Lane_Classification_Type OpenDriveTypeMapper::OdToOsiLaneType(const RoadLaneType odLaneType)
{
    osi3::Lane_Classification_Type osiType;

    switch (odLaneType)
    {
        case RoadLaneType::Undefined:
            osiType = osi3::Lane_Classification_Type_TYPE_UNKNOWN;
            break;
        case RoadLaneType::Driving:
            osiType = osi3::Lane_Classification_Type_TYPE_DRIVING;
            break;
        default:
            osiType = osi3::Lane_Classification_Type_TYPE_OTHER;
            break;
    }
    return osiType;
}

osi3::TrafficSignValue_Unit OpenDriveTypeMapper::OdToOsiTrafficSignUnit(const RoadSignalUnit unit)
{
    switch(unit)
    {
        case RoadSignalUnit::Undefined:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_OTHER;
         case RoadSignalUnit::Meter:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER;
         case RoadSignalUnit::Kilometer:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER;
         case RoadSignalUnit::Feet:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_FEET;
         case RoadSignalUnit::LandMile:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MILE;
         case RoadSignalUnit::MilesPerHour:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MILE_PER_HOUR;
         case RoadSignalUnit::KilometersPerHour:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR;
         case RoadSignalUnit::MetricTons:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METRIC_TON;
         case RoadSignalUnit::Percent:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_PERCENTAGE;
         default:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_UNKNOWN;
    }
}


osi3::LaneBoundary::Classification::Type OpenDriveTypeMapper::OdToOsiLaneMarkingType(RoadLaneRoadMarkType type, OWL::LaneMarkingSide side)
{
    switch (type)
    {
    case  RoadLaneRoadMarkType::None:
        return osi3::LaneBoundary_Classification_Type_TYPE_NO_LINE;
    case  RoadLaneRoadMarkType::Solid:
        return osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE;
    case  RoadLaneRoadMarkType::Broken:
        return osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE;
    case  RoadLaneRoadMarkType::Solid_Solid:
        return osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE;
    case  RoadLaneRoadMarkType::Solid_Broken:
        if (side == OWL::LaneMarkingSide::Right)
        {
            return osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE;
        }
        else
        {
            return osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE;
        }
    case  RoadLaneRoadMarkType::Broken_Solid:
        if (side == OWL::LaneMarkingSide::Right)
        {
            return osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE;
        }
        else
        {
            return osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE;
        }
    case  RoadLaneRoadMarkType::Broken_Broken:
        return osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE;
    case  RoadLaneRoadMarkType::Botts_Dots:
        return osi3::LaneBoundary_Classification_Type_TYPE_BOTTS_DOTS;
    case  RoadLaneRoadMarkType::Grass:
        return osi3::LaneBoundary_Classification_Type_TYPE_GRASS_EDGE;
    case  RoadLaneRoadMarkType::Curb:
        return osi3::LaneBoundary_Classification_Type_TYPE_CURB;
    case RoadLaneRoadMarkType::Undefined:
        return osi3::LaneBoundary_Classification_Type_TYPE_NO_LINE;
    default:
        throw std::invalid_argument("Invalid type");
    }
}

osi3::LaneBoundary::Classification::Color OpenDriveTypeMapper::OdToOsiLaneMarkingColor(RoadLaneRoadMarkColor color)
{
    switch (color)
    {
        case RoadLaneRoadMarkColor::Undefined:
            return osi3::LaneBoundary_Classification_Color_COLOR_OTHER;
        case RoadLaneRoadMarkColor::White:
            return osi3::LaneBoundary_Classification_Color_COLOR_WHITE;
        case RoadLaneRoadMarkColor::Blue:
            return osi3::LaneBoundary_Classification_Color_COLOR_BLUE;
        case RoadLaneRoadMarkColor::Green:
            return osi3::LaneBoundary_Classification_Color_COLOR_GREEN;
        case RoadLaneRoadMarkColor::Red:
            return osi3::LaneBoundary_Classification_Color_COLOR_RED;
        case RoadLaneRoadMarkColor::Yellow:
        case RoadLaneRoadMarkColor::Orange:
            return osi3::LaneBoundary_Classification_Color_COLOR_YELLOW;
        default:
            throw std::invalid_argument("Invalid color");
    }
}

LaneType OpenDriveTypeMapper::OdToOwlLaneType(const RoadLaneType laneType)
{
    switch (laneType)
    {
        case RoadLaneType::Shoulder:
            return LaneType::Shoulder;
        case RoadLaneType::Border:
            return LaneType::Border;
        case RoadLaneType::Driving:
            return LaneType::Driving;
        case RoadLaneType::Stop:
            return LaneType::Stop;
        case RoadLaneType::None:
            return LaneType::None;
        case RoadLaneType::Restricted:
            return LaneType::Restricted;
        case RoadLaneType::Parking:
            return LaneType::Parking;
        case RoadLaneType::Median:
            return LaneType::Median;
        case RoadLaneType::Biking:
            return LaneType::Biking;
        case RoadLaneType::Sidewalk:
            return LaneType::Sidewalk;
        case RoadLaneType::Curb:
            return LaneType::Curb;
        case RoadLaneType::Exit:
            return LaneType::Exit;
        case RoadLaneType::Entry:
            return LaneType::Entry;
        case RoadLaneType::OnRamp:
            return LaneType::OnRamp;
        case RoadLaneType::OffRamp:
            return LaneType::OffRamp;
        case RoadLaneType::ConnectingRamp:
            return LaneType::ConnectingRamp;
        case RoadLaneType::Tram:
            return LaneType::Tram;
        case RoadLaneType::RoadWorks:
            return LaneType::RoadWorks;
        case RoadLaneType::Bidirectional:
            return LaneType::Bidirectional;
        default:
            return LaneType::Undefined;
    }
}

LaneMarking::Color OpenDriveTypeMapper::OsiToOdLaneMarkingColor(const osi3::LaneBoundary_Classification_Color color)
{
    switch (color)
    {
    case osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_WHITE:
        return LaneMarking::Color::White;
    case osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_YELLOW:
        return LaneMarking::Color::Yellow;
    case osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_GREEN:
        return LaneMarking::Color::Green;
    case osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_RED:
        return LaneMarking::Color::Red;
    case osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_BLUE:
        return LaneMarking::Color::Blue;
    case osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_OTHER:
        return LaneMarking::Color::Other;
    default:
        throw std::invalid_argument("Type of lane marking color not supported.");
    }
}

LaneMarking::Type OpenDriveTypeMapper::OsiToOdLaneMarkingType(const osi3::LaneBoundary_Classification_Type type)
{
    switch(type)
    {
    case osi3::LaneBoundary_Classification_Type_TYPE_NO_LINE:
        return LaneMarking::Type::None;
    case osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE:
        return LaneMarking::Type::Solid;
    case osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE:
        return LaneMarking::Type::Broken;
    case osi3::LaneBoundary_Classification_Type_TYPE_BOTTS_DOTS:
        return LaneMarking::Type::Botts_Dots;
    case osi3::LaneBoundary_Classification_Type_TYPE_GRASS_EDGE:
        return LaneMarking::Type::Grass;
    case osi3::LaneBoundary_Classification_Type_TYPE_CURB:
        return LaneMarking::Type::Curb;
    default:
        throw std::invalid_argument("Type of lane marking not supported.");
    }
}

const OpenDriveTypeMapper::SignalsMapping *OpenDriveTypeMapper::GetSignalsMapping(const std::string &country)
{
    if (country == "" || country == "DE" || country == "OpenDRIVE")
    {
        return &OpenDriveTypeMapper::signalsMappingDE;
    }
    else if (country == "US")
    {
        return &OpenDriveTypeMapper::signalsMappingUS;
    }
    else if (country == "CN")
    {
        return &OpenDriveTypeMapper::signalsMappingCN;
    }
    else
    {
        throw std::runtime_error("Unknown country \"" + country + "\"" + ". Supported countries are listed in the documentation. ");
    }
}
