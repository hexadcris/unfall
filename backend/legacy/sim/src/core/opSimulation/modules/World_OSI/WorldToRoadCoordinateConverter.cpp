/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/opMath.h"
#include "common/commonTools.h"
#include "WorldToRoadCoordinateConverter.h"

namespace World {
namespace Localization {

bool WorldToRoadCoordinateConverter::IsConvertible(const Common::Vector2d& point) const
{
    return CommonHelper::IntersectionCalculation::IsWithin(element.laneGeometryElement.joints.current.points.left,
                                                           element.laneGeometryElement.joints.next.points.left,
                                                           element.laneGeometryElement.joints.current.points.right,
                                                           element.laneGeometryElement.joints.next.points.right,
                                                           point);
}

RoadPosition WorldToRoadCoordinateConverter::GetRoadCoordinate(const Common::Vector2d& point, double hdg) const
{
    const auto intersectionPoint = GetIntersectionPoint(point);
    const auto s = CalcS(intersectionPoint);
    const auto t = CalcT(point, intersectionPoint);
    const auto yaw = CalcYaw(hdg);
    return {s, t, yaw};
}

double WorldToRoadCoordinateConverter::GetS(const Common::Vector2d& point) const
{
    const auto intersectionPoint = GetIntersectionPoint(point);
    return CalcS(intersectionPoint);
}

Common::Vector2d WorldToRoadCoordinateConverter::GetIntersectionPoint(const Common::Vector2d& point) const
{
    Common::Vector2d intersectionPoint;
    if (element.tAxisCenter)
    {
        intersectionPoint = CommonHelper::CalculateIntersection(element.laneGeometryElement.joints.current.points.reference, element.referenceVector,
                                                                element.tAxisCenter.value(), point - element.tAxisCenter.value())
                            .value_or(element.laneGeometryElement.joints.current.points.reference);
    }
    else
    {
        intersectionPoint = ProjectOntoReferenceAxis(point);
    }
    return intersectionPoint;
}

double WorldToRoadCoordinateConverter::CalcS(const Common::Vector2d& intersectionPoint) const
{
    Common::Vector2d sVector = intersectionPoint - element.laneGeometryElement.joints.current.points.reference;
    return element.laneGeometryElement.joints.current.sOffset + sVector.Length() * element.referenceScale;

}

double WorldToRoadCoordinateConverter::CalcT(const Common::Vector2d& point, const Common::Vector2d& intersectionPoint) const
{
    Common::Vector2d tVector = point - intersectionPoint;
    return IsLeftOfReferenceAxis(tVector) ? tVector.Length() : - tVector.Length();

}

double WorldToRoadCoordinateConverter::CalcYaw(double hdg) const
{
    // Updated for direction awareness - might be an issue, when cars try to turn around
    return CommonHelper::SetAngleToValidRange(hdg - element.laneGeometryElement.joints.current.sHdg);
}

Common::Vector2d WorldToRoadCoordinateConverter::ProjectOntoReferenceAxis(const Common::Vector2d& point) const
{
    const double lamdba = element.referenceVector.Dot(point - element.laneGeometryElement.joints.current.points.reference)
                    / element.referenceVector.Dot(element.referenceVector);
    return element.laneGeometryElement.joints.current.points.reference + element.referenceVector * lamdba;
}

bool WorldToRoadCoordinateConverter::IsLeftOfReferenceAxis(const Common::Vector2d& vector) const
{
    Common::Vector2d vectorToLeft{-element.referenceVector.y, element.referenceVector.x};
    return vector.Dot(vectorToLeft) >= 0;
}

} // namespace Localization
} // namespace World

