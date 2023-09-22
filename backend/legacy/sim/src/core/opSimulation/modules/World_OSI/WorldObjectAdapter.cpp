/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "common/boostGeometryCommon.h"
#include "WorldData.h"
#include "WorldObjectAdapter.h"

WorldObjectAdapter::WorldObjectAdapter(OWL::Interfaces::WorldObject& baseTrafficObject) :
    baseTrafficObject{baseTrafficObject}
{
}

const polygon_t& WorldObjectAdapter::GetBoundingBox2D() const
{
    if (boundingBoxNeedsUpdate)
    {
        boundingBox = CalculateBoundingBox();
        boundingBoxNeedsUpdate = false;
    }

    return boundingBox;
}

double WorldObjectAdapter::GetDistanceReferencePointToLeadingEdge() const
{
    return baseTrafficObject.GetDimension().length / 2.0;
}

double WorldObjectAdapter::GetHeight() const
{
    return baseTrafficObject.GetDimension().height;
}

int WorldObjectAdapter::GetId() const
{
    return baseTrafficObject.GetId();
}

double WorldObjectAdapter::GetLength() const
{
    return baseTrafficObject.GetDimension().length;
}

double WorldObjectAdapter::GetPositionX() const
{
    return baseTrafficObject.GetReferencePointPosition().x;
}

double WorldObjectAdapter::GetPositionY() const
{
    return baseTrafficObject.GetReferencePointPosition().y;
}

double WorldObjectAdapter::GetWidth() const
{
    return baseTrafficObject.GetDimension().width;
}

double WorldObjectAdapter::GetYaw() const
{
    return baseTrafficObject.GetAbsOrientation().yaw;
}

double WorldObjectAdapter::GetRoll() const
{
    return baseTrafficObject.GetAbsOrientation().roll;
}

const OWL::Interfaces::WorldObject& WorldObjectAdapter::GetBaseTrafficObject() const
{
    return baseTrafficObject;
}

const polygon_t WorldObjectAdapter::CalculateBoundingBox() const
{
    const double length = GetLength();
    const double width = GetWidth();
    const double height = GetHeight();
    const double rotation = GetYaw();
    const double roll = GetRoll();

    const double x = GetPositionX();
    const double y = GetPositionY();

    const double center = GetDistanceReferencePointToLeadingEdge();

    const double halfWidth = width / 2.0;
    const double widthLeft = halfWidth * std::cos(roll) + (roll < 0 ? height * std::sin(-roll) : 0);
    const double widthRight = halfWidth * std::cos(roll) + (roll > 0 ? height * std::sin(roll) : 0);

    point_t boxPoints[]
    {
        {center - length, -widthRight},
        {center - length,  widthLeft},
        {center,           widthLeft},
        {center,          -widthRight},
        {center - length, -widthRight}
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

namespace WorldObjectCommon {

double GetFrontDeltaS(double length, double width, double hdg, double distanceReferencePointToLeadingEdge)
{
    auto l_front = distanceReferencePointToLeadingEdge;
    auto l_rear = distanceReferencePointToLeadingEdge - length;
    auto w = width / 2;

    auto s1 = l_front * cos(hdg) - w * sin(hdg);
    auto s2 = l_front * cos(hdg) + w * sin(hdg);
    auto s3 = l_rear  * cos(hdg) - w * sin(hdg);
    auto s4 = l_rear  * cos(hdg) + w * sin(hdg);

    return std::max({s1, s2, s3, s4});
}

double GetRearDeltaS(double length, double width, double hdg, double distanceReferencePointToLeadingEdge)
{
    auto l_front = distanceReferencePointToLeadingEdge;
    auto l_rear = distanceReferencePointToLeadingEdge - length;
    auto w = width / 2;

    auto s1 = l_front * cos(hdg) - w * sin(hdg);
    auto s2 = l_front * cos(hdg) + w * sin(hdg);
    auto s3 = l_rear  * cos(hdg) - w * sin(hdg);
    auto s4 = l_rear  * cos(hdg) + w * sin(hdg);

    return std::min({s1, s2, s3, s4});
}
} // namespace WorldObjectCommon
