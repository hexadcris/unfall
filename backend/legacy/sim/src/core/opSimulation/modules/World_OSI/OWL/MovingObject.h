/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef OPENPASS_MOVINGOBJECT_H
#define OPENPASS_MOVINGOBJECT_H

#include "DataTypes.h"

namespace OWL::Implementation {

    class MovingObject : public Interfaces::MovingObject {
    public:
        MovingObject(osi3::MovingObject *osiMovingObject);

        ~MovingObject() override = default;

        Id GetId() const override;

        //Base
        Primitive::Dimension GetDimension() const override;

        Primitive::AbsPosition GetReferencePointPosition() const override;

        const RoadIntervals &GetTouchedRoads() const override;

        double GetDistanceReferencePointToLeadingEdge() const override;

        Primitive::AbsOrientation GetAbsOrientation() const override;

        Primitive::LaneOrientation GetLaneOrientation() const override;

        Primitive::AbsVelocity GetAbsVelocity() const override;

        double GetAbsVelocityDouble() const override;

        Primitive::AbsAcceleration GetAbsAcceleration() const override;

        double GetAbsAccelerationDouble() const override;

        Primitive::AbsOrientationRate GetAbsOrientationRate() const override;

        Primitive::AbsOrientationAcceleration GetAbsOrientationAcceleration() const override;

        void SetBoundingBoxCenterToRear(double distanceX, double distanceY, double distanceZ) override;

        void SetBoundingBoxCenterToFront(double distanceX, double distanceY, double distanceZ) override;

        void SetReferencePointPosition(const Primitive::AbsPosition &newPosition) override;

        void SetX(const double newX) override;

        void SetY(const double newY) override;

        void SetZ(const double newZ) override;

        void SetTouchedRoads(const RoadIntervals &touchedRoads) override;

        void SetDimension(const Primitive::Dimension &newDimension) override;

        void SetLength(const double newLength) override;

        void SetWidth(const double newWidth) override;

        void SetHeight(const double newHeight) override;

        void SetAbsOrientation(const Primitive::AbsOrientation &newOrientation) override;

        void SetYaw(const double newYaw) override;

        void SetPitch(const double newPitch) override;

        void SetRoll(const double newRoll) override;

        void SetAbsVelocity(const Primitive::AbsVelocity &newVelocity) override;

        void SetAbsVelocity(const double newVelocity) override;

        void SetAbsAcceleration(const Primitive::AbsAcceleration &newAcceleration) override;

        void SetAbsAcceleration(const double newAcceleration) override;

        void SetAbsOrientationRate(const Primitive::AbsOrientationRate &newOrientationRate) override;

        void SetAbsOrientationAcceleration(const Primitive::AbsOrientationAcceleration &newOrientationAcceleration) override;

        void AddLaneAssignment(const Interfaces::Lane &lane) override;

        const Interfaces::Lanes &GetLaneAssignments() const override;

        void ClearLaneAssignments() override;

        void SetIndicatorState(IndicatorState indicatorState) override;

        IndicatorState GetIndicatorState() const override;

        void SetBrakeLightState(bool brakeLightState) override;

        bool GetBrakeLightState() const override;

        void SetHeadLight(bool headLight) override;

        bool GetHeadLight() const override;

        void SetHighBeamLight(bool highbeamLight) override;

        bool GetHighBeamLight() const override;

        void SetType(AgentVehicleType) override;

        void AddWheel(const WheelData &wheelData) override;

        std::optional<const WheelData> GetWheelData(unsigned int axleIndex, unsigned int rowIndex) override;

        Angle GetSteeringWheelAngle() override;

        void SetSteeringWheelAngle(const Angle newValue) override;

        void SetFrontAxleSteeringYaw(const double wheelYaw) override;

        void SetWheelsRotationRateAndOrientation(const double velocity, const double wheelRadiusFront, const double wheelRadiusRear, const double cycleTime) override;

        void SetSourceReference(const ExternalReference &externalReference) override;

        void CopyToGroundTruth(osi3::GroundTruth &target) const override;

    private:
        osi3::MovingObject *osiObject;
        const RoadIntervals *touchedRoads;
        Interfaces::Lanes assignedLanes;

        mutable Lazy<double> frontDistance;
        mutable Lazy<double> rearDistance;

        const Implementation::InvalidLane invalidLane;
        const Implementation::InvalidSection invalidSection;
        const Implementation::InvalidRoad invalidRoad;
    };

    class DefaultMovingObjectFactory {
    public :

        void AssignDefaultValues(osi3::MovingObject *_osiMovingObject);

    private:
        osi3::Dimension3d d_dimension;
        osi3::Orientation3d d_orientation;
        osi3::Vector3d d_3d;
        osi3::Identifier d_identifier;
        osi3::MovingObject_Type d_type;
        google::protobuf::uint32 d_uint32;
        std::string model_reference;
        double d_double;

        void AssignDefaultTypes();

        void AssignDefaultBase(osi3::BaseMoving *base);

        void AssignDefaultVehicleAttributes(osi3::MovingObject_VehicleAttributes *vehicleAttributes);

        void AssignDefaultVehicleClassification(osi3::MovingObject_VehicleClassification *vehicleClassifcation);

        void
        AssignDefaultMovingObjectClassification(osi3::MovingObject_MovingObjectClassification *objectClassifcation);


    };

}// namespace OWL

#endif //OPENPASS_MOVINGOBJECT_H
