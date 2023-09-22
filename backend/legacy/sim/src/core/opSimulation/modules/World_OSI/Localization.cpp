/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include <exception>
#include <numeric>

#include "Localization.h"

#include <boost/function_output_iterator.hpp>

namespace World {
namespace Localization {

std::function<void (const RTreeElement&)>  LocateOnGeometryElement(const OWL::Interfaces::WorldData& worldData,
                                                              const std::vector<Common::Vector2d>& objectBoundary,
                                                              const Common::Vector2d& referencePoint,
                                                              const double& hdg,
                                                              LocatedObject& locatedObject)
{
    return  [&](auto const& value)
    {
        const LocalizationElement& localizationElement = *value.second;

        auto intersection = CommonHelper::IntersectionCalculation::GetIntersectionPoints(localizationElement.polygon, objectBoundary, false);

        if(intersection.size() < 3)
        {
            //Actual polygons do not intersect -> Skip this GeometryElement
            return;
        }

        WorldToRoadCoordinateConverter converter(localizationElement);

        const OWL::Interfaces::Lane* lane = localizationElement.lane;
        const auto laneOdId = lane->GetOdId();
        const auto roadId = lane->GetRoad().GetId();

        if (converter.IsConvertible(referencePoint))
        {
            auto [s, t, yaw] = converter.GetRoadCoordinate(referencePoint, hdg);
            locatedObject.referencePoint[roadId] = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
        }

        for (const auto& point : intersection)
        {
             auto [s, t, yaw] = converter.GetRoadCoordinate(point, hdg);
             assert(s >= 0.0);

             auto& laneOverlap = locatedObject.laneOverlaps[lane];
             if (s < laneOverlap.sMin.roadPosition.s)
             {
                 laneOverlap.sMin = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
             }
             if (s > laneOverlap.sMax.roadPosition.s)
             {
                 laneOverlap.sMax = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
             }
             if (t < laneOverlap.tMin.roadPosition.t)
             {
                 laneOverlap.tMin = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
             }
             if (t > laneOverlap.tMax.roadPosition.t)
             {
                 laneOverlap.tMax = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
             }
        }
    };
}

LocatedObject LocateOnGeometryElements(const bg_rTree& rTree, const OWL::Interfaces::WorldData& worldData, const std::vector<Common::Vector2d>& agentBoundary, CoarseBoundingBox theAgent,
                        const Common::Vector2d referencePoint, double hdg)
{
 LocatedObject locatedObject;
 rTree.query(bgi::intersects(theAgent),
                         boost::make_function_output_iterator(LocateOnGeometryElement(worldData, agentBoundary, referencePoint, hdg, locatedObject)));

 return locatedObject;
}

std::function<void (const RTreeElement&)> LocateOnGeometryElement(const OWL::Interfaces::WorldData& worldData, const Common::Vector2d& point,
                                                                  const double& hdg, GlobalRoadPositions& result)
{
    return  [&](auto const& value)
    {
        const LocalizationElement& localizationElement = *value.second;

        boost::geometry::de9im::mask mask("*****F***"); // within
        if (!boost::geometry::relate(point_t{point.x, point.y}, localizationElement.boost_polygon, mask)) //Check wether point is inside polygon
        {
            return;
        }

        WorldToRoadCoordinateConverter converter(localizationElement);

        const OWL::Interfaces::Lane* lane = localizationElement.lane;
        const auto laneOdId = lane->GetOdId();
        const auto roadId = lane->GetRoad().GetId();

        if (converter.IsConvertible(point))
        {
            auto [s, t, yaw] = converter.GetRoadCoordinate(point, hdg);
            result[roadId] = GlobalRoadPosition(roadId, laneOdId, s, t, yaw);
        }
    };
}

GlobalRoadPositions LocateOnGeometryElements(const bg_rTree& rTree, const OWL::Interfaces::WorldData& worldData,
                           const Common::Vector2d point, double hdg)
{
    GlobalRoadPositions result;
    CoarseBoundingBox box = GetSearchBox({point});
    rTree.query(bgi::intersects(box),
                            boost::make_function_output_iterator(LocateOnGeometryElement(worldData, point, hdg, result)));

    return result;
}

polygon_t GetBoundingBox(double x, double y, double length, double width, double rotation, double center)
{
    double halfWidth = width / 2;

    point_t boxPoints[]
    {
        point_t{center - length, -halfWidth},
        point_t{center - length,  halfWidth},
        point_t{center,           halfWidth},
        point_t{center,          -halfWidth},
        point_t{center - length, -halfWidth}
    };

    polygon_t box;
    polygon_t boxTemp;
    bg::append(box, boxPoints);

    bt::translate_transformer<double, 2, 2> translate(x, y);

    // rotation in mathematical negativ order (boost) -> invert to match
    bt::rotate_transformer<bg::radian, double, 2, 2> rotate(-rotation);

    bg::transform(box, boxTemp, rotate);
    bg::transform(boxTemp, box, translate);

    return box;
}

void CreateLaneAssignments(OWL::Interfaces::WorldObject& object, const std::map<const OWL::Interfaces::Lane*, OWL::LaneOverlap >& laneOverlaps)
{
    for (auto [lane, laneOverlap] : laneOverlaps)
    {
        auto movingObject = dynamic_cast<OWL::MovingObject*>(&object);
        if (movingObject)
        {
            const_cast<OWL::Interfaces::Lane*>(lane)->AddMovingObject(*movingObject, laneOverlap);
            object.AddLaneAssignment(*lane);
            continue;
        }

        auto stationaryObject = dynamic_cast<OWL::StationaryObject*>(&object);
        if (stationaryObject)
        {
            const_cast<OWL::Interfaces::Lane*>(lane)->AddStationaryObject(*stationaryObject, laneOverlap);
            object.AddLaneAssignment(*lane);
            continue;
        }
    }
}

Result Localizer::BuildResult(const LocatedObject& locatedObject) const
{
    std::set<int> touchedLaneIds;
    RoadIntervals touchedRoads;

    bool isOnRoute = !locatedObject.referencePoint.empty();

    for (const auto& [lane, laneOverlap] : locatedObject.laneOverlaps)
    {
        std::string roadId = lane->GetRoad().GetId();
        touchedRoads[roadId].lanes.push_back(lane->GetOdId());
        if (touchedRoads[roadId].sMin.roadPosition.s > laneOverlap.sMin.roadPosition.s)
        {
            touchedRoads[roadId].sMin = laneOverlap.sMin;
        }
        if (touchedRoads[roadId].sMax.roadPosition.s < laneOverlap.sMax.roadPosition.s)
        {
            touchedRoads[roadId].sMax = laneOverlap.sMax;
        }
        if (touchedRoads[roadId].tMin.laneId > laneOverlap.tMin.laneId ||
        touchedRoads[roadId].tMin.laneId == laneOverlap.tMin.laneId && touchedRoads[roadId].tMin.roadPosition.t > laneOverlap.tMin.roadPosition.t)
        {
            touchedRoads[roadId].tMin = laneOverlap.tMin;
        }
        if (touchedRoads[roadId].tMax.laneId < laneOverlap.tMax.laneId ||
        touchedRoads[roadId].tMax.laneId == laneOverlap.tMax.laneId && touchedRoads[roadId].tMax.roadPosition.t < laneOverlap.tMax.roadPosition.t)
        {
            touchedRoads[roadId].tMax = laneOverlap.tMax;
        }
    }

    Result result({{ObjectPointPredefined::Reference, locatedObject.referencePoint}},
                  touchedRoads,
                  isOnRoute);

    return result;
}

Localizer::Localizer(const OWL::Interfaces::WorldData& worldData) :
    worldData(worldData)
{
}

void Localizer::Init()
{
    for (const auto& [laneId, lane] : worldData.GetLanes())
    {
        for (const auto& laneGeometryElement : lane->GetLaneGeometryElements())
        {
            elements.emplace_back(*laneGeometryElement);
        }
    }
    for (const auto& element : elements)
    {
        rTree.insert(std::make_pair(element.search_box, &element));
    }
}

Result Localizer::Locate(const polygon_t& boundingBox, OWL::Interfaces::WorldObject& object) const
{
    const auto& referencePointPosition = object.GetReferencePointPosition();
    const auto& orientation = object.GetAbsOrientation();

    std::vector<Common::Vector2d> agentBoundary;
    for (point_t point : boundingBox.outer())
    {
        agentBoundary.emplace_back(bg::get<0>(point), bg::get<1>(point));
    }
    agentBoundary.pop_back();

    CoarseBoundingBox searchBox = GetSearchBox(agentBoundary);

    Common::Vector2d referencePoint{referencePointPosition.x, referencePointPosition.y};
    if (object.Is<OWL::Interfaces::MovingObject>())
    {
        const auto& movingObject = object.As<OWL::Interfaces::MovingObject>();
        double distanceRefToFront = movingObject->GetDistanceReferencePointToLeadingEdge();
    }

    const auto& locatedObject = LocateOnGeometryElements(rTree,
                                                         worldData,
                                                         agentBoundary,
                                                         searchBox,
                                                         referencePoint,
                                                         orientation.yaw);

    auto result = BuildResult(locatedObject);
    if (result.isOnRoute)
    {
        CreateLaneAssignments(object, locatedObject.laneOverlaps);
    }

    return result;
}

GlobalRoadPositions Localizer::Locate(const Common::Vector2d& point, const double& hdg) const
{
    return LocateOnGeometryElements(rTree,
                                    worldData,
                                    point,
                                    hdg);
}

void Localizer::Unlocate(OWL::Interfaces::WorldObject& object) const
{
    object.ClearLaneAssignments();
}

}
}
