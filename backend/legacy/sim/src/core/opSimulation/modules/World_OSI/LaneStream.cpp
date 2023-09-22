/********************************************************************************
 * Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "LaneStream.h"

LaneStream::LaneStream(std::vector<LaneStreamElement> elements) :
    elements(elements)
{
}


StreamPosition LaneStream::GetStreamPosition(const GlobalRoadPosition &roadPosition) const
{
    StreamPosition streamPosition {-1, 0};

    for (const auto& element : elements)
    {
        if (element.lane->GetRoad().GetId() == roadPosition.roadId && element.lane->GetOdId() == roadPosition.laneId
        && element.lane->GetDistance(OWL::MeasurementPoint::RoadStart) <= roadPosition.roadPosition.s
        && element.lane->GetDistance(OWL::MeasurementPoint::RoadEnd) >= roadPosition.roadPosition.s)
        {
            streamPosition.s = element.GetStreamPosition(roadPosition.roadPosition.s - element.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            streamPosition.t = roadPosition.roadPosition.t * (element.inStreamDirection ? 1 : -1);
            streamPosition.hdg = CommonHelper::SetAngleToValidRange(roadPosition.roadPosition.hdg + (element.inStreamDirection ? 0 : M_PI));
        }
    }
    return streamPosition;
}

GlobalRoadPosition LaneStream::GetRoadPosition(const StreamPosition &streamPosition) const
{
    GlobalRoadPosition globalRoadPosition;

    for (const auto& element : elements)
    {
        if (element.StartS() <= streamPosition.s && element.EndS() >= streamPosition.s)
        {
            globalRoadPosition.roadId = element.lane->GetRoad().GetId();
            globalRoadPosition.laneId = element.lane->GetOdId();
            globalRoadPosition.roadPosition.s = element.lane->GetDistance(OWL::MeasurementPoint::RoadStart) + element.GetElementPosition(streamPosition.s);
            globalRoadPosition.roadPosition.t = streamPosition.t * (element.inStreamDirection ? 1 : -1);
            globalRoadPosition.roadPosition.hdg = CommonHelper::SetAngleToValidRange(streamPosition.hdg + (element.inStreamDirection ? 0 : M_PI));
        }
    }

    return globalRoadPosition;
}

std::vector<const AgentInterface *> LaneStream::GetAgentsInRange(const StreamPosition &start, const StreamPosition &end) const
{
    AgentInterfaces foundObjects;
    for (const auto& laneStreamElement : elements)
    {
        if (laneStreamElement.EndS() < start.s) //Ignore lanes ending before startDistance
        {
            continue;
        }
        if (laneStreamElement.StartS() > end.s) //Stop if reached endDistance
        {
            break;
        }
        const auto streamDirection = laneStreamElement.inStreamDirection;

        for (const auto& [laneOverlap, object] : laneStreamElement.lane->GetWorldObjects(streamDirection))
        {
            const auto s_min = streamDirection ? laneOverlap.sMin.roadPosition.s : laneOverlap.sMax.roadPosition.s;
            const auto s_max = streamDirection ? laneOverlap.sMax.roadPosition.s : laneOverlap.sMin.roadPosition.s;

            auto streamPositionStart = laneStreamElement.GetStreamPosition(s_min - laneStreamElement.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            if (streamPositionStart > end.s)
            {
                break;
            }

            auto streamPositionEnd = laneStreamElement.GetStreamPosition(s_max - laneStreamElement.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            if (dynamic_cast<const OWL::Interfaces::MovingObject*>(object) && streamPositionEnd >= start.s)
            {
                if (std::find(foundObjects.crbegin(), foundObjects.crend(), object->GetLink<AgentInterface>()) == foundObjects.crend())
                {
                    foundObjects.push_back(object->GetLink<AgentInterface>());
                }
            }
        }
    }
    return foundObjects;
}

std::vector<const WorldObjectInterface *> LaneStream::GetObjectsInRange(const StreamPosition &start, const StreamPosition &end) const
{
    std::vector<const WorldObjectInterface*> foundObjects;
    for (const auto& laneStreamElement : elements)
    {
        if (laneStreamElement.EndS() < start.s) //Ignore lanes ending before startDistance
        {
            continue;
        }
        if (laneStreamElement.StartS() > end.s) //Stop if reached endDistance
        {
            break;
        }
        const auto streamDirection = laneStreamElement.inStreamDirection;

        for (const auto& [laneOverlap, object] : laneStreamElement.lane->GetWorldObjects(streamDirection))
        {
            const auto s_min = streamDirection ? laneOverlap.sMin.roadPosition.s : laneOverlap.sMax.roadPosition.s;
            const auto s_max = streamDirection ? laneOverlap.sMax.roadPosition.s : laneOverlap.sMin.roadPosition.s;

            auto streamPositionStart = laneStreamElement.GetStreamPosition(s_min - laneStreamElement.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            if (streamPositionStart > end.s)
            {
                break;
            }

            auto streamPositionEnd = laneStreamElement.GetStreamPosition(s_max - laneStreamElement.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            if (streamPositionEnd >= start.s)
            {
                if (std::find(foundObjects.crbegin(), foundObjects.crend(), object->GetLink<WorldObjectInterface>()) == foundObjects.crend())
                {
                    foundObjects.push_back(object->GetLink<WorldObjectInterface>());
                }
            }
        }
    }
    return foundObjects;
}

std::optional<StreamPosition> LaneStream::GetStreamPosition(const WorldObjectInterface *object, const ObjectPoint &point) const
{
    for (auto element : elements)
    {
        auto objectPosition = helper::map::query(object->GetRoadPosition(point), element.lane->GetRoad().GetId());
        if(objectPosition.has_value()
                && objectPosition.value().laneId == element.lane->GetOdId()
                && element.lane->GetSection().Covers(objectPosition.value().roadPosition.s))
        {
            const auto s = element.GetStreamPosition(objectPosition.value().roadPosition.s - element.lane->GetDistance(OWL::MeasurementPoint::RoadStart));
            const auto t = objectPosition.value().roadPosition.t * (element.inStreamDirection ? 1 : -1);
            const auto hdg = CommonHelper::SetAngleToValidRange(objectPosition.value().roadPosition.hdg + (element.inStreamDirection ? 0 : M_PI));
            return StreamPosition{s, t, hdg};
        }
    }
    return std::nullopt;
}

double LaneStream::GetLength() const
{
    return elements.back().EndS();
}

std::vector<std::pair<double, LaneType>> LaneStream::GetLaneTypes() const
{
    LaneType lastLaneType{LaneType::Undefined};
    std::vector<std::pair<double, LaneType>> laneTypes;
    for (const auto& element : elements)
    {
        if(element.lane->GetLaneType() != lastLaneType)
        {
            laneTypes.emplace_back(element.StartS(), element.lane->GetLaneType());
            lastLaneType = element.lane->GetLaneType();
        }
    }
    return laneTypes;
}
