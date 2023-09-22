/********************************************************************************
 * Copyright (c) 2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "gmock/gmock.h"
#include "OWL/DataTypes.h"
#include "common/globalDefinitions.h"

namespace osi3 {
class GroundTruth;
}

namespace OWL::Fakes {
class StationaryObject : public OWL::Interfaces::StationaryObject
{
public:
    void SetLinkedObjectForTesting(void* linkedObject)
    {
        this->linkedObject = linkedObject;
    }

    MOCK_CONST_METHOD0(GetId, OWL::Id());
    MOCK_CONST_METHOD0(GetDimension, OWL::Primitive::Dimension());

    MOCK_CONST_METHOD0(GetReferencePointPosition, OWL::Primitive::AbsPosition());
    MOCK_CONST_METHOD0(GetAbsOrientation, OWL::Primitive::AbsOrientation());
    MOCK_CONST_METHOD0(GetAbsVelocityDouble, double());
    MOCK_CONST_METHOD0(GetAbsAccelerationDouble, double());

    MOCK_METHOD1(AddLaneAssignment, void(const OWL::Interfaces::Lane& lane));
    MOCK_CONST_METHOD0(GetLaneAssignments, const OWL::Interfaces::Lanes & ());
    MOCK_METHOD0(ClearLaneAssignments, void());

    MOCK_METHOD1(SetAbsOrientation, void(const OWL::Primitive::AbsOrientation& newOrientation));
    MOCK_METHOD1(SetDimension, void(const OWL::Primitive::Dimension& newDimension));
    MOCK_CONST_METHOD0(GetTouchedRoads, const RoadIntervals &());
    MOCK_METHOD1(SetTouchedRoads, void(const RoadIntervals &touchedRoads));
    MOCK_METHOD1(SetReferencePointPosition, void(const OWL::Primitive::AbsPosition& newPosition));

    MOCK_CONST_METHOD1(CopyToGroundTruth, void(osi3::GroundTruth&));
};


class MovingObject : public OWL::Interfaces::MovingObject
{
public:
    void SetLinkedObjectForTesting(void* linkedObject)
    {
        this->linkedObject = linkedObject;
    }

    MOCK_CONST_METHOD0(GetId, OWL::Id());
    MOCK_CONST_METHOD0(GetDimension, OWL::Primitive::Dimension());

    MOCK_CONST_METHOD0(GetLane, const OWL::Interfaces::Lane & ());
    MOCK_CONST_METHOD0(GetSection, const OWL::Interfaces::Section & ());
    MOCK_CONST_METHOD0(GetRoad, const OWL::Interfaces::Road & ());

    MOCK_CONST_METHOD0(GetReferencePointPosition, OWL::Primitive::AbsPosition());
    MOCK_CONST_METHOD0(GetTouchedRoads, const RoadIntervals &());

    MOCK_CONST_METHOD0(GetAbsOrientation, OWL::Primitive::AbsOrientation());
    MOCK_CONST_METHOD0(GetLaneOrientation, OWL::Primitive::LaneOrientation());

    MOCK_CONST_METHOD0(GetAbsVelocity, OWL::Primitive::AbsVelocity());
    MOCK_CONST_METHOD0(GetAbsVelocityDouble, double());

    MOCK_CONST_METHOD0(GetAbsAcceleration, OWL::Primitive::AbsAcceleration());
    MOCK_CONST_METHOD0(GetAbsAccelerationDouble, double());

    MOCK_CONST_METHOD0(GetAbsOrientationRate, OWL::Primitive::AbsOrientationRate());
    MOCK_CONST_METHOD0(GetAbsOrientationAcceleration, OWL::Primitive::AbsOrientationAcceleration());

    MOCK_METHOD1(SetDimension, void(const OWL::Primitive::Dimension& newDimension));
    MOCK_METHOD1(SetLength, void(const double newLength));
    MOCK_METHOD1(SetWidth, void(const double newWidth));
    MOCK_METHOD1(SetHeight, void(const double newHeight));
    MOCK_METHOD3(SetBoundingBoxCenterToRear, void(const double distanceX, const double distanceY, const double distanceZ));
    MOCK_METHOD3(SetBoundingBoxCenterToFront, void(const double distanceX, const double distanceY, const double distanceZ));

    MOCK_METHOD1(SetReferencePointPosition, void(const OWL::Primitive::AbsPosition& newPosition));
    MOCK_METHOD1(SetX, void(const double newX));
    MOCK_METHOD1(SetY, void(const double newY));
    MOCK_METHOD1(SetZ, void(const double newZ));

    MOCK_METHOD1(SetTouchedRoads, void(const RoadIntervals &touchedRoads));

    MOCK_METHOD1(SetAbsOrientation, void(const OWL::Primitive::AbsOrientation& newOrientation));
    MOCK_METHOD1(SetYaw, void(const double newYaw));
    MOCK_METHOD1(SetPitch, void(const double newPitch));
    MOCK_METHOD1(SetRoll, void(const double newRoll));

    MOCK_METHOD1(SetAbsVelocity, void(const OWL::Primitive::AbsVelocity& newVelocity));
    MOCK_METHOD1(SetAbsVelocity, void(const double newVelocity));

    MOCK_METHOD1(SetAbsAcceleration, void(const OWL::Primitive::AbsAcceleration& newAcceleration));
    MOCK_METHOD1(SetAbsAcceleration, void(const double newAcceleration));

    MOCK_METHOD1(SetAbsOrientationRate, void(const OWL::Primitive::AbsOrientationRate& newOrientationRate));
    MOCK_METHOD1(SetAbsOrientationAcceleration, void(const OWL::Primitive::AbsOrientationAcceleration& newOrientationAcceleration));

    MOCK_METHOD1(AddLaneAssignment, void(const OWL::Interfaces::Lane& lane));
    MOCK_CONST_METHOD0(GetLaneAssignments, const OWL::Interfaces::Lanes & ());
    MOCK_METHOD0(ClearLaneAssignments, void());

    MOCK_CONST_METHOD0(GetDistanceReferencePointToLeadingEdge, double());

    MOCK_METHOD1(SetIndicatorState, void(IndicatorState indicatorState));
    MOCK_CONST_METHOD0(GetIndicatorState, IndicatorState());
    MOCK_METHOD1(SetBrakeLightState, void(bool brakeLightState));
    MOCK_CONST_METHOD0(GetBrakeLightState, bool());
    MOCK_METHOD1(SetHeadLight, void(bool headLight));
    MOCK_CONST_METHOD0(GetHeadLight, bool());
    MOCK_METHOD1(SetHighBeamLight, void(bool highbeamLight));
    MOCK_CONST_METHOD0(GetHighBeamLight, bool());

    MOCK_METHOD1(SetType, void(AgentVehicleType));
    MOCK_METHOD1(SetSourceReference, void(const OWL::ExternalReference& externalReference));
    MOCK_METHOD1(AddWheel, void(const WheelData &wheelData));

    MOCK_METHOD0(GetSteeringWheelAngle, float());
    MOCK_METHOD1(SetSteeringWheelAngle, void(const Angle));

    MOCK_METHOD1(SetFrontAxleSteeringYaw, void(const double));
    MOCK_METHOD4(SetWheelsRotationRateAndOrientation, void(const double, const double, const double, const double));
    MOCK_METHOD2(GetWheelData, std::optional<const OWL::WheelData>(unsigned int, unsigned int));

    MOCK_CONST_METHOD1(CopyToGroundTruth, void(osi3::GroundTruth&));
};
}
