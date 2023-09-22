/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
//-----------------------------------------------------------------------------

#pragma once

#include <memory>

#include "common/globalDefinitions.h"
#include "common/boostGeometryCommon.h"
#include "WorldData.h"
#include "WorldToRoadCoordinateConverter.h"


namespace World {
namespace Localization {

using RTreeElement = std::pair<CoarseBoundingBox, const LocalizationElement *>;
using bg_rTree = boost::geometry::index::rtree<RTreeElement, boost::geometry::index::quadratic<8, 4> >;

//! This struct contains all the information about an objects position in the world, that result
//! from the Localization
class Result
{
public:
    std::map<ObjectPoint, GlobalRoadPositions> points;
    RoadIntervals touchedRoads;
    bool isOnRoute {false};

    static Result Invalid()
    {
        return Result{};
    }

    Result() = default;

    Result(std::map<ObjectPoint, GlobalRoadPositions> points,
           RoadIntervals touchedRoads,
           bool isOnRoute) :
        points{points},
        touchedRoads{touchedRoads},
        isOnRoute{isOnRoute}
    {}
};

//! This struct is the result of the localization on all geometry element using an r-tree
//! It is than used to build the localization result
struct LocatedObject
{
    std::map<const OWL::Interfaces::Lane*, OWL::LaneOverlap> laneOverlaps;
    GlobalRoadPositions referencePoint;
};

//! Calculates the intersection of the object boundary with an GeometryElement and stores the min and max s coordinates
//! and min and max deltas (for t) in the LocatedObject struct. Also calculates s, t and yaw of the referencePoint and
//! mainLaneLocator, if the lay on the GeometryElement
//!
//! \param worldData        OWL WorldData
//! \param objectBoundary   bounding box of the object
//! \param referencePoint   position of the reference point (x,y)
//! \param mainLaneLocator  position of the mainLaneLocator (x,y)
//! \param hdg              heading of the object
//! \param locatedObject    Output of the function is stored here
//! \return         function that the boost r-tree calls for every LaneGeometryElement, that possibly intersects with the object
//!
std::function<void (const RTreeElement&)> LocateOnGeometryElement(const OWL::Interfaces::WorldData& worldData,
                                                             const std::vector<Common::Vector2d>& objectBoundary,
                                                             const Common::Vector2d& referencePoint,
                                                             const double& hdg,
                                                             LocatedObject& locatedObject);

//! Calculates the coordinates of the point on the GeometryElement and stores the result in the map
//!
//! \param worldData        OWL WorldData
//! \param point            point to locate
//! \param hdg              heading of the object
//! \param result    Output of the function is stored here
//! \return         function that the boost r-tree calls for every LaneGeometryElement, that possibly intersects with the object
std::function<void (const RTreeElement&)> LocateOnGeometryElement(const OWL::Interfaces::WorldData& worldData,
                                                             const Common::Vector2d& point,
                                                             const double& hdg,
                                                             GlobalRoadPositions& result);

polygon_t GetBoundingBox(double x, double y, double length, double width, double rotation, double center);

//! Assigns this object to all lanes it was located on, so that the result of Lane::GetWorldObjects
//! contains this object
void CreateLaneAssignments(OWL::Interfaces::WorldObject& object, const std::map<const OWL::Interfaces::Lane*, OWL::LaneOverlap>& laneOverlaps);

class Localizer
{
public:
    Localizer(const OWL::Interfaces::WorldData& worldData);

    void Init();

    Result Locate(const polygon_t& boundingBox, OWL::Interfaces::WorldObject& object) const;

    GlobalRoadPositions Locate(const Common::Vector2d& point, const double& hdg) const;

    void Unlocate(OWL::Interfaces::WorldObject& object) const;

private:

    //!
    //! \brief BuildResult
    //! \param locatedObject
    //! \param route
    //! \return
    //!
    Result BuildResult(const LocatedObject& locatedObject) const;

    const OWL::Interfaces::WorldData& worldData;
    std::vector<LocalizationElement> elements{};
    bg_rTree rTree;
};

}
}
