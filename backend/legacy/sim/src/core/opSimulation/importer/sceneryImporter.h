/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SceneryImporter.h
//! @brief This file contains the importer of the scenery configuration.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <list>
#include <QDomDocument>
#include "scenery.h"
#include "include/roadInterface/junctionInterface.h"
#include "include/roadInterface/connectionInterface.h"

using namespace Configuration;

namespace Importer
{
const std::unordered_map<std::string, RoadLaneType> roadLaneTypeConversionMap =
{
    {"shoulder", RoadLaneType::Shoulder},
    {"border", RoadLaneType::Border},
    {"driving", RoadLaneType::Driving},
    {"stop", RoadLaneType::Stop},
    {"none", RoadLaneType::None},
    {"restricted", RoadLaneType::Restricted},
    {"parking", RoadLaneType::Parking},
    {"median", RoadLaneType::Median},
    {"biking", RoadLaneType::Biking},
    {"sidewalk", RoadLaneType::Sidewalk},
    {"curb", RoadLaneType::Curb},
    {"exit", RoadLaneType::Exit},
    {"entry", RoadLaneType::Entry},
    {"onRamp", RoadLaneType::OnRamp},
    {"offRamp", RoadLaneType::OffRamp},
    {"connectingRamp", RoadLaneType::ConnectingRamp},
    {"tram", RoadLaneType::Tram},
    {"roadWorks", RoadLaneType::RoadWorks},
    {"bidirectional", RoadLaneType::Bidirectional}
};

const std::unordered_map<std::string, RoadTypeInformation> roadTypeConversionMap = {
                                                                                    {"unknown", RoadTypeInformation::Unknown},
                                                                                    {"rural", RoadTypeInformation::Rural},
                                                                                    {"town", RoadTypeInformation::Town},
                                                                                    {"motorway", RoadTypeInformation::Motorway},
                                                                                    {"lowSpeed", RoadTypeInformation::LowSpeed},
                                                                                    {"pedestrian", RoadTypeInformation::Pedestrian},
                                                                                    {"bicycle", RoadTypeInformation::Bicycle}
                                                                                   };

class SceneryImporter
{
public:    
    SceneryImporter() = default;
    SceneryImporter(const SceneryImporter&) = delete;
    SceneryImporter(SceneryImporter&&) = delete;
    SceneryImporter& operator=(const SceneryImporter&) = delete;
    SceneryImporter& operator=(SceneryImporter&&) = delete;
    virtual ~SceneryImporter() = default;

    static void LogAndThrowIfFalse(bool expression);

    //-----------------------------------------------------------------------------
    //! Imports data structures from the scenery configuration file e.g. OpenDrive file
    //!
    //! @param[in]  filename       path to OpenDrive file
    //! @return                    true on success
    //-----------------------------------------------------------------------------
    static bool Import(const std::string &filename,
                       Scenery *scenery);

    //-----------------------------------------------------------------------------
    //! @brief Parses roads into a scenery object.
    //! @note  Use this entry point only for testing or within the class context
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - road tag:                     id
    //! Then parses geometries, elevationProfile, road links, and road lanes.
    //!
    //! @param[in]  documentRoot        DOM element containing the OpenDRIVE road
    //! @param[out] scenery             Scenery with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseRoads(QDomElement &documentRoot,
                           Scenery *scenery);

    //-----------------------------------------------------------------------------
    //! @brief Parses junctions into a scenery object.
    //! @note  Use this entry point only for testing or within the class context
    //!
    //!
    //! @param[in]  documentRoot        DOM element containing the OpenDRIVE road
    //! @param[out] scenery             Scenery with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseJunctions(QDomElement &documentRoot,
                           Scenery *scenery);

    //-----------------------------------------------------------------------------
    //! @brief Parses connections into a junction object.
    //! @note  Use this entry point only for testing or within the class context
    //!
    //!
    //! @param[in]  junctionElement      DOM element containing the OpenDRIVE junction
    //! @param[out] junction             Junction with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseJunctionConnections(QDomElement &junctionElement, JunctionInterface *junction);

    //-----------------------------------------------------------------------------
    //! @brief Parses connection links into a connection object.
    //! @note  Use this entry point only for testing or within the class context
    //!
    //!
    //! @param[in]  junctionElement      DOM element containing the OpenDRIVE junction
    //! @param[out] junction             Connection with the contents of the DOM element
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    static void ParseJunctionConnectionLinks(QDomElement &connectionElement, ConnectionInterface *connection);

    //-----------------------------------------------------------------------------
    //! @brief Parses priorities into a junction object.
    //! @note  Use this entry point only for testing or within the class context
    //!
    //!
    //! @param[in]  junctionElement      DOM element containing the OpenDRIVE junction
    //! @param[out] junction             Junction with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseJunctionPriorities(QDomElement &junctionElement, JunctionInterface *junction);

private:

    //-----------------------------------------------------------------------------
    //! @brief Parses lane elements in a lane section and stores their information
    //!         in a lane section object.
    //!
    //! OpenDRIVE attributes whose information is stored in the lane section:
    //! - lane tag:                     id and type
    //! - width subtag:                 sOffset, a, b, c, and d
    //! - link subtag:                  predecessor (and its id) and successor
    //!                                 (and its id)
    //!
    //! @param[in]  rootElement         DOM element containing the OpenDRIVE lanes
    //! @param[out] laneSection         Lane section with the contents of the DOM
    //!                                 elements
    //-----------------------------------------------------------------------------
    static void ParseLanes(QDomElement &rootElement,
                           RoadLaneSectionInterface *laneSection);


    //-----------------------------------------------------------------------------
    //! @brief Parses lane road mark elements into a road lane object.
    //!
    //! OpenDRIVE attributes whose information is stored in the road lane object.:
    //! - roadMark tag:                 sOffset, type
    //!
    //! @param[in]  leftCenterRight     left/center/right lane road mark
    //! @param[in]  roadLaneElement     DOM element containing the OpenDRIVE road lane
    //! @param[out] roadLane            Road lane with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseLaneRoadMark(std::string leftCenterRight, QDomElement &roadLaneElement, RoadLaneInterface* roadLane);

    //-----------------------------------------------------------------------------
    //! @brief Parses the road geometry into a road object.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - geometry tag:                 s, x, y, hdg and length
    //! - line subtag
    //! - arc subtag:                   curvature
    //! - spiral subtag:                curvStart and curvEnd
    //! - poly3 subtag:                 a, b, c, and d
    //!
    //! @param[in]  roadElement         DOM element containing the OpenDRIVE road
    //! @param[out] road                Road with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseGeometries(QDomElement &roadElement,
                                RoadInterface *road);

    //-----------------------------------------------------------------------------
    //! @brief Parses the elevation profile into a road object.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - elevationProfile tag
    //! - elevation subtag:             s, a, b, c, and d
    //!
    //! @param[in]  roadElement         DOM element containing the OpenDRIVE road
    //! @param[out] road                Road with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseElevationProfile(QDomElement &roadElement,
                                      RoadInterface *road);

    //-----------------------------------------------------------------------------
    //! @brief Parses the road links into a road object.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - link tag
    //! - predecessor subtag:           elementType (only "road" allowed), elementId,
    //!                                 contactPoint ("start" or "end")
    //! - successor subtag:             elementType (only "road" allowed), elementId,
    //!                                 contactPoint ("start" or "end")
    //! - neighbor subtag:              side ("left" or "right"), elementId, and
    //!                                 direction ("same" or "opposite")
    //!
    //! @param[in]  roadElement         DOM element containing the OpenDRIVE road
    //! @param[out] road                Road with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseRoadLinks(QDomElement &roadElement, RoadInterface *road);

    //-----------------------------------------------------------------------------
    //! @brief Parses the lanes specification into a road object.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - lanes tag
    //! - laneOffset subtag:            s, a, b, c, and d
    //! - laneSection subtag:           s
    //! - left subtag:                  ParseLanes called
    //! - right subtag:                 ParseLanes called
    //!
    //!
    //! @param[in]  roadElement         DOM element containing the OpenDRIVE road
    //! @param[out] road                Road with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseRoadLanes(QDomElement &roadElement, RoadInterface *road);



    //-----------------------------------------------------------------------------
    static void ConvertRadius(RoadObjectSpecification &object);


public:
    //-----------------------------------------------------------------------------
    //! @brief Parses the signals specification into a road object.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - signals tag
    //! - signal subtag:                s, t, dynamic, type, subtype, value, unit,
    //!                                 and orientation
    //! - optional: validity subtag     fromLane and toLane
    //!
    //! @param[in]  roadElement         DOM element containing the OpenDRIVE road
    //! @param[out] road                Road with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseSignals(QDomElement &roadElement, RoadInterface *road);

    //-----------------------------------------------------------------------------
    //! @brief Parses multiple OpenDRIVE road objects into a road object.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - objects tag
    //! - object subtag
    //!
    //! @param[in]  roadElement         DOM element containing the OpenDRIVE road
    //! @param[out] road                Road with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseObjects(QDomElement &roadElement, RoadInterface *road);

    //-----------------------------------------------------------------------------
    //! @brief Parses a single OpenDRIVE object into a road object.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - objects tag
    //! - object subtag
    //!
    //! @param[in]  objectElement       DOM element containing the OpenDRIVE object
    //! @param[out] road                Road with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseObject(QDomElement &objectElement, RoadInterface *road);

    //-----------------------------------------------------------------------------
    //! @brief Converts a validitiy tag into a vector with increasing lane numbers
    //!
    //! @param[in]  rootElement     root element containing a validity sub-tag
    //! @param[out] validities      parsed result
    //-----------------------------------------------------------------------------
    static void ParseElementValidity(const QDomElement &rootElement, RoadElementValidity &validity);


    //-----------------------------------------------------------------------------
    //! @brief Parses the road typen into a road object.
    //!
    //! @param[in]  roadElement         DOM element containing the OpenDRIVE road
    //! @param[out] road                Road with the contents of the DOM element
    //-----------------------------------------------------------------------------
    static void ParseRoadTypes(QDomElement &roadElement, RoadInterface *road);

    //-----------------------------------------------------------------------------
    //! @brief Parses repeat tag of objects.
    //!
    //! @param[in]  objectElement       DOM element containing the traffic object
    //! @param[out] object              Object with the contents of the DOM element
    //!
    //! @return                         List of all repeated objects
    //-----------------------------------------------------------------------------
    static std::vector<RoadObjectSpecification> ParseObjectRepeat(QDomElement &objectElement, const RoadObjectSpecification &object);

    //-----------------------------------------------------------------------------
    //! @brief Parses repeat objects into a list of road objects.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - repeat tag                    s, length, distance
    //!     optional:                   tStart, tEnd,
    //!                                 widthStart, widthEnd,
    //!                                 heightStart, heightEnd,
    //!                                 zOffsetStart, zOffsetEnd
    //!
    //! @param[in]  repeatElement       DOM element containing the OpenDRIVE repeat element
    //! @param[in]  object              DOM element of the parent object
    //! @param[out] objectRepetitions   Reference to a list of repeated road objects
    //-----------------------------------------------------------------------------
    static void ParseRepeat(QDomElement &repeatElement, RoadObjectSpecification object, std::vector<RoadObjectSpecification> &objectRepitions);

    //-----------------------------------------------------------------------------
    //! @brief Calculates and applies attributes to repeated objects.
    //!
    //! OpenDRIVE attributes whose information is stored in the road:
    //! - repeat tag                    s, length, distance
    //!     optional:                   tStart, tEnd,
    //!                                 widthStart, widthEnd,
    //!                                 heightStart, heightEnd,
    //!                                 zOffsetStart, zOffsetEnd
    //!
    //! @param[in]  objectRepeat        Repeat data
    //! @param[in]  object              Base road object
    //! @param[out] objectRepetitions   Reference to a list of repeated road objects
    //!                                 to apply the data to
    //-----------------------------------------------------------------------------
    static void ApplyRepeat(ObjectRepeat objectRepeat, RoadObjectSpecification object, std::vector<RoadObjectSpecification> &objectRepitions);

    //-----------------------------------------------------------------------------
    //! @brief Add all objects from List to Road
    //!
    //! @param[in]  parsedObjects       List of all parsed Objects within one object tag
    //! @param[in]  road                road with should contain objects
    //-----------------------------------------------------------------------------
    static void AddParsedObjectsToRoad(std::vector<RoadObjectSpecification> parsedObjects, RoadInterface *road);

    //-----------------------------------------------------------------------------
    //! @brief Parse optional parameters of an interval
    //!
    //! @param[in]  Attribute           start and end double of intervall
    //! @param[in]  repeatElement       road with should contain objects
    //! @param[out] OptionalInterval    DOM element containing the retetition of an object
    //-----------------------------------------------------------------------------
    static void ParseOptionalInterval(QDomElement &repeatElement, std::string startAttribute, std::string endAttribute, OptionalInterval &interval);

 private:
    //-----------------------------------------------------------------------------
    //! @brief Converts a potential signal unit into a known RoadSignalUnit
    //!
    //! @param[in]  element         element to be parsed
    //! @param[out] signalType      parsed result
    //-----------------------------------------------------------------------------
    static void ParseSignalUnit(std::string element, RoadSignalUnit& signalUnit);

    //-----------------------------------------------------------------------------
    //! @brief Checks the values which are not enumerations of validity (if possible)
    //!
    //! @param[in]  signal          signal to be checked
    //-----------------------------------------------------------------------------
    static void checkRoadSignalBoundaries(RoadSignalSpecification signal);

    constexpr static const double SAMPLING_RATE = 1.0; // 1m sampling rate of reference line
};

} // namespace Importer


