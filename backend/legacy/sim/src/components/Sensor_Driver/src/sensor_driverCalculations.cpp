/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "sensor_driverCalculations.h"

double SensorDriverCalculations::GetLateralDistanceToObject(const std::string& roadId, const AgentInterface *otherObject)
{
    if (otherObject == nullptr)
    {
        return INFINITY;
    }

    const auto& otherReferencePoint = otherObject->GetRoadPosition(ObjectPointPredefined::Reference);
    double lateralPositionOfOtherObject = otherReferencePoint.at(roadId).roadPosition.t;
    if (otherReferencePoint.at(roadId).laneId == egoAgent.GetLaneIdFromRelative(0))
    {
        return lateralPositionOfOtherObject - egoAgent.GetPositionLateral();
    }
    if (otherReferencePoint.at(roadId).laneId == egoAgent.GetLaneIdFromRelative(1))
    {
        return lateralPositionOfOtherObject - egoAgent.GetPositionLateral() + 0.5 * egoAgent.GetLaneWidth() + 0.5 * egoAgent.GetLaneWidth(1);
    }
    if (otherReferencePoint.at(roadId).laneId == egoAgent.GetLaneIdFromRelative(-1))
    {
        return lateralPositionOfOtherObject - egoAgent.GetPositionLateral() - 0.5 * egoAgent.GetLaneWidth() - 0.5 * egoAgent.GetLaneWidth(-1);
    }
    return INFINITY;
}
