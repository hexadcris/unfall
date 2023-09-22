/********************************************************************************
 * Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "MovingObject.h"
#include "OWL/OsiDefaultValues.h"
#include "OWL/DataTypes.h"
#include "common/hypot.h"
#include <qglobal.h>


OWL::Implementation::MovingObject::MovingObject(osi3::MovingObject *osiMovingObject) :
        osiObject{osiMovingObject} {
    DefaultMovingObjectFactory dmof;
    dmof.AssignDefaultValues(osiMovingObject);
    SetIndicatorState(IndicatorState::IndicatorState_Off);
    SetBrakeLightState(false);
    SetHeadLight(false);
    SetHighBeamLight(false);
}

void OWL::Implementation::MovingObject::CopyToGroundTruth(osi3::GroundTruth &target) const {
    auto newMovingObject = target.add_moving_object();
    newMovingObject->CopyFrom(*osiObject);
}

OWL::Id OWL::Implementation::MovingObject::GetId() const {
    return osiObject->id().value();
}

OWL::Primitive::Dimension OWL::Implementation::MovingObject::GetDimension() const {
    return OWL::GetDimensionFromOsiObject(osiObject);
}

double OWL::Implementation::MovingObject::GetDistanceReferencePointToLeadingEdge() const {
    return osiObject->base().dimension().length() * 0.5 -
           osiObject->vehicle_attributes().bbcenter_to_rear().x();
}

void OWL::Implementation::MovingObject::SetDimension(const Primitive::Dimension &newDimension) {
    osi3::Dimension3d *osiDimension = osiObject->mutable_base()->mutable_dimension();

    osiDimension->set_length(newDimension.length);
    osiDimension->set_width(newDimension.width);
    osiDimension->set_height(newDimension.height);
}

void OWL::Implementation::MovingObject::SetLength(const double newLength) {
    osi3::Dimension3d *osiDimension = osiObject->mutable_base()->mutable_dimension();
    osiDimension->set_length(newLength);
}

void OWL::Implementation::MovingObject::SetWidth(const double newWidth) {
    osi3::Dimension3d *osiDimension = osiObject->mutable_base()->mutable_dimension();
    osiDimension->set_width(newWidth);
}

void OWL::Implementation::MovingObject::SetHeight(const double newHeight) {
    osi3::Dimension3d *osiDimension = osiObject->mutable_base()->mutable_dimension();
    osiDimension->set_height(newHeight);
}

void OWL::Implementation::MovingObject::SetBoundingBoxCenterToRear(const double distanceX,const double distanceY,const double distanceZ) {
    osiObject->mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->set_x(distanceX);
    osiObject->mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->set_y(distanceY);
    osiObject->mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->set_z(distanceZ);
}

void OWL::Implementation::MovingObject::SetBoundingBoxCenterToFront(const double distanceX,const double distanceY,const double distanceZ) {
    osiObject->mutable_vehicle_attributes()->mutable_bbcenter_to_front()->set_x(distanceX);
    osiObject->mutable_vehicle_attributes()->mutable_bbcenter_to_front()->set_y(distanceY);
    osiObject->mutable_vehicle_attributes()->mutable_bbcenter_to_front()->set_z(distanceZ);
}

OWL::Primitive::AbsPosition OWL::Implementation::MovingObject::GetReferencePointPosition() const {
    const osi3::Vector3d &osiPosition = osiObject->base().position();
    const double yaw = osiObject->base().orientation().yaw();
    const osi3::Vector3d &bbCenterToRear = osiObject->vehicle_attributes().bbcenter_to_rear();

    Primitive::AbsPosition position;

    position.x = osiPosition.x() + std::cos(yaw) * bbCenterToRear.x();
    position.y = osiPosition.y() + std::sin(yaw) * bbCenterToRear.x();
    position.z = osiPosition.z();

    return position;
}

void OWL::Implementation::MovingObject::SetReferencePointPosition(const Primitive::AbsPosition &newPosition) {
    osi3::Vector3d *osiPosition = osiObject->mutable_base()->mutable_position();
    const double yaw = osiObject->base().orientation().yaw();
    const osi3::Vector3d &bbCenterToRear = osiObject->vehicle_attributes().bbcenter_to_rear();

    osiPosition->set_x(newPosition.x - std::cos(yaw) * bbCenterToRear.x());
    osiPosition->set_y(newPosition.y - std::sin(yaw) * bbCenterToRear.x());
    osiPosition->set_z(newPosition.z);

    frontDistance.Invalidate();
    rearDistance.Invalidate();
}

void OWL::Implementation::MovingObject::SetX(const double newX) {
    osi3::Vector3d *osiPosition = osiObject->mutable_base()->mutable_position();
    const double yaw = osiObject->base().orientation().yaw();
    const osi3::Vector3d &bbCenterToRear = osiObject->vehicle_attributes().bbcenter_to_rear();

    osiPosition->set_x(newX - std::cos(yaw) * bbCenterToRear.x());
}

void OWL::Implementation::MovingObject::SetY(const double newY) {
    osi3::Vector3d *osiPosition = osiObject->mutable_base()->mutable_position();
    const double yaw = osiObject->base().orientation().yaw();
    const osi3::Vector3d &bbCenterToRear = osiObject->vehicle_attributes().bbcenter_to_rear();

    osiPosition->set_y(newY - std::sin(yaw) * bbCenterToRear.x());
}

void OWL::Implementation::MovingObject::SetZ(const double newZ) {
    osi3::Vector3d *osiPosition = osiObject->mutable_base()->mutable_position();
    osiPosition->set_z(newZ);
}

void OWL::Implementation::MovingObject::SetTouchedRoads(const RoadIntervals &touchedRoads) {
    this->touchedRoads = &touchedRoads;
}

OWL::Primitive::AbsOrientation OWL::Implementation::MovingObject::GetAbsOrientation() const {
    osi3::Orientation3d osiOrientation = osiObject->base().orientation();
    Primitive::AbsOrientation orientation;

    orientation.yaw = osiOrientation.yaw();
    orientation.pitch = osiOrientation.pitch();
    orientation.roll = osiOrientation.roll();

    return orientation;
}

void OWL::Implementation::MovingObject::SetAbsOrientation(const Primitive::AbsOrientation &newOrientation) {
    osi3::Orientation3d *osiOrientation = osiObject->mutable_base()->mutable_orientation();
    const auto referencePosition = GetReferencePointPosition(); //AbsPosition needs to be evaluated with "old" yaw
    osiOrientation->set_yaw(CommonHelper::SetAngleToValidRange(newOrientation.yaw));
    osiOrientation->set_pitch(CommonHelper::SetAngleToValidRange(newOrientation.pitch));
    osiOrientation->set_roll(CommonHelper::SetAngleToValidRange(newOrientation.roll));
    frontDistance.Invalidate();
    rearDistance.Invalidate();
    SetReferencePointPosition(referencePosition); //Changing yaw also changes position of the boundingBox center
}

void OWL::Implementation::MovingObject::SetYaw(const double newYaw) {
    const auto referencePosition = GetReferencePointPosition();
    osi3::Orientation3d *osiOrientation = osiObject->mutable_base()->mutable_orientation();
    osiOrientation->set_yaw(CommonHelper::SetAngleToValidRange(newYaw));
    frontDistance.Invalidate();
    rearDistance.Invalidate();
    SetReferencePointPosition(referencePosition); //Changing yaw also changes position of the boundingBox center
}

void OWL::Implementation::MovingObject::SetPitch(const double newPitch) {
    osi3::Orientation3d *osiOrientation = osiObject->mutable_base()->mutable_orientation();
    osiOrientation->set_pitch(CommonHelper::SetAngleToValidRange(newPitch));
}

void OWL::Implementation::MovingObject::SetRoll(const double newRoll) {
    osi3::Orientation3d *osiOrientation = osiObject->mutable_base()->mutable_orientation();
    osiOrientation->set_roll(CommonHelper::SetAngleToValidRange(newRoll));
}

void OWL::Implementation::MovingObject::SetIndicatorState(IndicatorState indicatorState) {
    if (indicatorState == IndicatorState::IndicatorState_Off) {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_indicator_state(
                osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_OFF);
        return;
    } else if (indicatorState == IndicatorState::IndicatorState_Left) {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_indicator_state(
                osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_LEFT);
        return;
    } else if (indicatorState == IndicatorState::IndicatorState_Right) {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_indicator_state(
                osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_RIGHT);
        return;
    } else if (indicatorState == IndicatorState::IndicatorState_Warn) {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_indicator_state(
                osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_WARNING);
        return;
    }

    throw std::invalid_argument("Indicator state is not supported");
}

IndicatorState OWL::Implementation::MovingObject::GetIndicatorState() const {
    const auto &osiState = osiObject->vehicle_classification().light_state().indicator_state();

    if (osiState == osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_OFF) {
        return IndicatorState::IndicatorState_Off;
    } else if (osiState ==
               osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_LEFT) {
        return IndicatorState::IndicatorState_Left;
    } else if (osiState ==
               osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_RIGHT) {
        return IndicatorState::IndicatorState_Right;
    } else if (osiState ==
               osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_WARNING) {
        return IndicatorState::IndicatorState_Warn;
    }

    throw std::logic_error("Indicator state is not supported");
}

void OWL::Implementation::MovingObject::SetBrakeLightState(bool brakeLightState) {
    if (brakeLightState) {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_brake_light_state(
                osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_NORMAL);
    } else {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_brake_light_state(
                osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_OFF);
    }
}

bool OWL::Implementation::MovingObject::GetBrakeLightState() const {
    const auto &osiState = osiObject->vehicle_classification().light_state().brake_light_state();

    if (osiState ==
        osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_NORMAL) {
        return true;
    } else if (osiState ==
               osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_OFF) {
        return false;
    }

    throw std::logic_error("BrakeLightState is not supported");
}

void OWL::Implementation::MovingObject::SetHeadLight(bool headLight) {
    if (headLight) {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_head_light(
                osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON);
    } else {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_head_light(
                osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    }
}

bool OWL::Implementation::MovingObject::GetHeadLight() const {
    const auto &osiState = osiObject->vehicle_classification().light_state().head_light();

    if (osiState ==
        osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON) {
        return true;
    } else if (osiState ==
               osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF) {
        return false;
    }

    throw std::logic_error("HeadLightState is not supported");
}

void OWL::Implementation::MovingObject::SetHighBeamLight(bool highbeamLight) {
    if (highbeamLight) {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_high_beam(
                osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON);
    } else {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_high_beam(
                osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    }
}

bool OWL::Implementation::MovingObject::GetHighBeamLight() const {
    const auto &osiState = osiObject->vehicle_classification().light_state().high_beam();

    if (osiState ==
        osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON) {
        return true;
    } else if (osiState ==
               osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF) {
        return false;
    }

    throw std::logic_error("HighBeamLightState is not supported");
}

void OWL::Implementation::MovingObject::SetType(AgentVehicleType type) {
    if (type == AgentVehicleType::Pedestrian) {
        osiObject->set_type(osi3::MovingObject_Type::MovingObject_Type_TYPE_PEDESTRIAN);
    } else {
        osiObject->set_type(osi3::MovingObject_Type::MovingObject_Type_TYPE_VEHICLE);

        switch (type) {
            case AgentVehicleType::Car:
                osiObject->mutable_vehicle_classification()->set_type(
                        osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_MEDIUM_CAR);
                break;

            case AgentVehicleType::Motorbike:
                osiObject->mutable_vehicle_classification()->set_type(
                        osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_MOTORBIKE);
                break;

            case AgentVehicleType::Bicycle:
                osiObject->mutable_vehicle_classification()->set_type(
                        osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_BICYCLE);
                break;

            case AgentVehicleType::Truck:
                osiObject->mutable_vehicle_classification()->set_type(
                        osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_HEAVY_TRUCK);
                break;

            default:
                throw (std::runtime_error("AgentVehicleType not supported"));
        }
    }
}

OWL::Primitive::LaneOrientation OWL::Implementation::MovingObject::GetLaneOrientation() const {
    throw std::logic_error("MovingObject::GetLaneOrientation not implemented");
}

OWL::Primitive::AbsVelocity OWL::Implementation::MovingObject::GetAbsVelocity() const {
    const osi3::Vector3d osiVelocity = osiObject->base().velocity();
    Primitive::AbsVelocity velocity;

    velocity.vx = osiVelocity.x();
    velocity.vy = osiVelocity.y();
    velocity.vz = osiVelocity.z();

    return velocity;
}

double OWL::Implementation::MovingObject::GetAbsVelocityDouble() const {
    Primitive::AbsVelocity velocity = GetAbsVelocity();
    Primitive::AbsOrientation orientation = GetAbsOrientation();
    double sign = 1.0;

    double velocityAngle = std::atan2(velocity.vy, velocity.vx);

    if (std::abs(velocity.vx) == 0.0 && std::abs(velocity.vy) == 0.0) {
        velocityAngle = 0.0;
    }

    double angleBetween = velocityAngle - orientation.yaw;

    if (std::abs(angleBetween) > M_PI_2 && std::abs(angleBetween) < 3 * M_PI_2) {
        sign = -1.0;
    }

    return openpass::hypot(velocity.vx, velocity.vy) * sign;
}

void OWL::Implementation::MovingObject::SetAbsVelocity(const Primitive::AbsVelocity &newVelocity) {
    osi3::Vector3d *osiVelocity = osiObject->mutable_base()->mutable_velocity();

    osiVelocity->set_x(newVelocity.vx);
    osiVelocity->set_y(newVelocity.vy);
    osiVelocity->set_z(newVelocity.vz);
}

void OWL::Implementation::MovingObject::SetAbsVelocity(const double newVelocity) {
    osi3::Vector3d *osiVelocity = osiObject->mutable_base()->mutable_velocity();

    double yaw = GetAbsOrientation().yaw;
    double cos_val = std::cos(yaw);
    double sin_val = std::sin(yaw);

    osiVelocity->set_x(newVelocity * cos_val);
    osiVelocity->set_y(newVelocity * sin_val);
    osiVelocity->set_z(0.0);
}

OWL::Primitive::AbsAcceleration OWL::Implementation::MovingObject::GetAbsAcceleration() const {
    const osi3::Vector3d osiAcceleration = osiObject->base().acceleration();
    Primitive::AbsAcceleration acceleration;

    acceleration.ax = osiAcceleration.x();
    acceleration.ay = osiAcceleration.y();
    acceleration.az = osiAcceleration.z();

    return acceleration;
}

double OWL::Implementation::MovingObject::GetAbsAccelerationDouble() const {
    Primitive::AbsAcceleration acceleration = GetAbsAcceleration();
    Primitive::AbsOrientation orientation = GetAbsOrientation();
    double sign = 1.0;

    double accAngle = std::atan2(acceleration.ay, acceleration.ax);

    if (std::abs(acceleration.ax) == 0.0 && std::abs(acceleration.ay) == 0.0) {
        accAngle = 0.0;
    }

    double angleBetween = accAngle - orientation.yaw;

    if ((std::abs(angleBetween) - M_PI_2) > 0) {
        sign = -1.0;
    }

    return openpass::hypot(acceleration.ax, acceleration.ay) * sign;
}

void OWL::Implementation::MovingObject::SetAbsAcceleration(const Primitive::AbsAcceleration &newAcceleration) {
    osi3::Vector3d *osiAcceleration = osiObject->mutable_base()->mutable_acceleration();

    osiAcceleration->set_x(newAcceleration.ax);
    osiAcceleration->set_y(newAcceleration.ay);
    osiAcceleration->set_z(newAcceleration.az);
}

void OWL::Implementation::MovingObject::SetAbsAcceleration(const double newAcceleration) {
    osi3::Vector3d *osiAcceleration = osiObject->mutable_base()->mutable_acceleration();

    double yaw = GetAbsOrientation().yaw;
    double cos_val = std::cos(yaw);
    double sin_val = std::sin(yaw);

    osiAcceleration->set_x(newAcceleration * cos_val);
    osiAcceleration->set_y(newAcceleration * sin_val);
    osiAcceleration->set_z(0.0);
}

OWL::Primitive::AbsOrientationRate OWL::Implementation::MovingObject::GetAbsOrientationRate() const {
    const osi3::Orientation3d osiOrientationRate = osiObject->base().orientation_rate();

    return Primitive::AbsOrientationRate
            {
                    osiOrientationRate.yaw(),
                    osiOrientationRate.pitch(),
                    osiOrientationRate.roll()
            };
}

OWL::Primitive::AbsOrientationAcceleration OWL::Implementation::MovingObject::GetAbsOrientationAcceleration() const {
    const osi3::Orientation3d osiOrientationAcceleration = osiObject->base().orientation_acceleration();

    return Primitive::AbsOrientationAcceleration
            {
            osiOrientationAcceleration.yaw(),
            osiOrientationAcceleration.pitch(),
            osiOrientationAcceleration.roll()
            };
}

void OWL::Implementation::MovingObject::SetAbsOrientationRate(const Primitive::AbsOrientationRate &newOrientationRate) {
    osi3::Orientation3d *osiOrientationRate = osiObject->mutable_base()->mutable_orientation_rate();

    osiOrientationRate->set_yaw(newOrientationRate.yawRate);
    osiOrientationRate->set_pitch(newOrientationRate.pitchRate);
    osiOrientationRate->set_roll(newOrientationRate.rollRate);
}

void OWL::Implementation::MovingObject::SetAbsOrientationAcceleration(
        const Primitive::AbsOrientationAcceleration &newOrientationAcceleration) {
    osi3::Orientation3d *osiOrientationAcceleration = osiObject->mutable_base()->mutable_orientation_acceleration();

    osiOrientationAcceleration->set_yaw(newOrientationAcceleration.yawAcceleration);
    osiOrientationAcceleration->set_pitch(newOrientationAcceleration.pitchAcceleration);
    osiOrientationAcceleration->set_roll(newOrientationAcceleration.rollAcceleration);
}

void OWL::Implementation::MovingObject::AddLaneAssignment(const Interfaces::Lane &lane) {
    osi3::Identifier *assignedId = osiObject->add_assigned_lane_id();
    assignedId->set_value(lane.GetId());
    assignedLanes.push_back(&lane);
}

void OWL::Implementation::MovingObject::SetSourceReference(const OWL::ExternalReference& externalReference) {
    //Todo as soon as its implemented in OSI
}

void OWL::Implementation::MovingObject::AddWheel(const WheelData& wheelData)
{
    osi3::MovingObject_VehicleAttributes_WheelData newWheel {};
    newWheel.set_width(wheelData.width);
    newWheel.set_rim_radius(wheelData.rim_radius);
    newWheel.set_rotation_rate(wheelData.rotation_rate);
    newWheel.set_wheel_radius(wheelData.wheelRadius);
    newWheel.set_axle(wheelData.axle);
    newWheel.set_index(wheelData.index);
    newWheel.mutable_position()->set_x(wheelData.position.x);
    newWheel.mutable_position()->set_y(wheelData.position.y);
    newWheel.mutable_position()->set_z(wheelData.position.z);
    newWheel.mutable_orientation()->set_pitch(wheelData.orientation.roll);
    newWheel.mutable_orientation()->set_roll(wheelData.orientation.pitch);
    newWheel.mutable_orientation()->set_yaw(wheelData.orientation.yaw);
    osiObject->mutable_vehicle_attributes()->mutable_wheel_data()->Add(std::move(newWheel));
    if(osiObject->mutable_vehicle_attributes()->number_wheels() == std::numeric_limits<uint32_t>::max())
        osiObject->mutable_vehicle_attributes()->set_number_wheels(0);
    uint32_t s = osiObject->mutable_vehicle_attributes()->number_wheels() + 1;
    osiObject->mutable_vehicle_attributes()->set_number_wheels(s);
}

const OWL::Interfaces::Lanes& OWL::Implementation::MovingObject::GetLaneAssignments() const {
    return assignedLanes;
}

void OWL::Implementation::MovingObject::ClearLaneAssignments() {
    osiObject->mutable_assigned_lane_id()->Clear();
    assignedLanes.clear();
}

OWL::Angle OWL::Implementation::MovingObject::GetSteeringWheelAngle() {
    return osiObject->mutable_vehicle_attributes()->steering_wheel_angle();
}

void OWL::Implementation::MovingObject::SetSteeringWheelAngle(const Angle newValue) {
    osiObject->mutable_vehicle_attributes()->set_steering_wheel_angle(newValue);
}

std::optional<const OWL::WheelData> OWL::Implementation::MovingObject::GetWheelData(unsigned int axleIndex, unsigned int rowIndex) {
    //const auto predicate = ;
    auto it = std::find_if(osiObject->vehicle_attributes().wheel_data().begin(),
                           osiObject->vehicle_attributes().wheel_data().end(), [axleIndex, rowIndex](const osi3::MovingObject_VehicleAttributes_WheelData& wheel)
                           {return wheel.axle() == axleIndex && wheel.index() == rowIndex; });
    if(it == osiObject->vehicle_attributes().wheel_data().end())
    {
        return std::nullopt;
    }
    else {
        WheelData foundWheel {};
        foundWheel.SetFromOsi(&(*it));
        return foundWheel;
    }
}

void OWL::Implementation::MovingObject::SetFrontAxleSteeringYaw(const double wheelYaw) {
    std::for_each(osiObject->mutable_vehicle_attributes()->mutable_wheel_data()->begin(),
                  osiObject->mutable_vehicle_attributes()->mutable_wheel_data()->end(),
                  [wheelYaw](osi3::MovingObject_VehicleAttributes_WheelData& wheel)
                  {
                      if(wheel.axle() == 0) // assuming steering axle is in front
                      wheel.mutable_orientation()->set_yaw( wheelYaw); });
}

void OWL::Implementation::MovingObject::SetWheelsRotationRateAndOrientation(const double velocity, const double wheelRadiusFront, const double wheelRadiusRear, const double cycleTime)
{
    double rotationRateFront = velocity / wheelRadiusFront;
    double rotationRateRear = velocity / wheelRadiusRear;
    std::for_each(osiObject->mutable_vehicle_attributes()->mutable_wheel_data()->begin(),
                  osiObject->mutable_vehicle_attributes()->mutable_wheel_data()->end(),
                  [rotationRateFront, rotationRateRear, cycleTime](osi3::MovingObject_VehicleAttributes_WheelData& wheel)
                  {
                      if(wheel.axle() == 0)
                      {
                          wheel.set_rotation_rate(rotationRateFront);
                          auto newAngle = CommonHelper::SetAngleToValidRange(wheel.mutable_orientation()->pitch() + rotationRateFront * cycleTime);
                          wheel.mutable_orientation()->set_pitch(newAngle);
                      }
                      else {
                          wheel.set_rotation_rate(rotationRateRear);
                          auto newAngle = CommonHelper::SetAngleToValidRange(wheel.mutable_orientation()->pitch() + rotationRateRear * cycleTime);
                          wheel.mutable_orientation()->set_pitch(newAngle);
                      }
                      wheel.mutable_orientation()->set_roll( 0.0);
                  });
}

void OWL::Implementation::DefaultMovingObjectFactory::AssignDefaultValues(osi3::MovingObject* _osiMovingObject)
{
    AssignDefaultTypes();
    _osiMovingObject->mutable_id()->set_value(d_identifier.value());
    AssignDefaultBase(_osiMovingObject->mutable_base());
    _osiMovingObject->set_type(d_type);
    _osiMovingObject->mutable_assigned_lane_id()->Clear();
    AssignDefaultVehicleAttributes(_osiMovingObject->mutable_vehicle_attributes());
    AssignDefaultVehicleClassification(_osiMovingObject->mutable_vehicle_classification());
    _osiMovingObject->mutable_model_reference()->assign(model_reference);
    _osiMovingObject->mutable_future_trajectory()->Clear();
    AssignDefaultMovingObjectClassification(_osiMovingObject->mutable_moving_object_classification());
    //source_reference is optional and discarded
}

void OWL::Implementation::DefaultMovingObjectFactory::AssignDefaultTypes ()
{
    d_type = osi3::MovingObject_Type_TYPE_OTHER;
    d_dimension.set_height(std::numeric_limits<double>::signaling_NaN());
    d_dimension.set_width(std::numeric_limits<double>::signaling_NaN());
    d_dimension.set_length(std::numeric_limits<double>::signaling_NaN());
    d_orientation.set_pitch(std::numeric_limits<double>::signaling_NaN());
    d_orientation.set_roll(std::numeric_limits<double>::signaling_NaN());
    d_orientation.set_yaw(std::numeric_limits<double>::signaling_NaN());
    d_3d.set_x(std::numeric_limits<double>::signaling_NaN());
    d_3d.set_y(std::numeric_limits<double>::signaling_NaN());
    d_3d.set_z(std::numeric_limits<double>::signaling_NaN());
    d_identifier.set_value(std::numeric_limits<google::protobuf::uint64>::max());
    d_uint32 = std::numeric_limits<google::protobuf::uint32>::max();
    d_double = std::numeric_limits<double>::signaling_NaN();
};

void OWL::Implementation::DefaultMovingObjectFactory::AssignDefaultBase(osi3::BaseMoving* const base)
{

    base->mutable_base_polygon()->Clear();
    base->mutable_dimension()->CopyFrom(d_dimension);
    base->mutable_orientation()->CopyFrom(d_orientation);
    base->mutable_orientation_rate()->CopyFrom(d_orientation);
    base->mutable_orientation_acceleration()->CopyFrom(d_orientation);
    base->mutable_position()->CopyFrom(d_3d);
    base->mutable_velocity()->CopyFrom(d_3d);
    base->mutable_acceleration()->CopyFrom(d_3d);
}

void OWL::Implementation::DefaultMovingObjectFactory::AssignDefaultVehicleAttributes(osi3::MovingObject_VehicleAttributes* const vehicleAttributes)
{
    vehicleAttributes->mutable_driver_id(); //not to be set if host_vehicle is set to false
    vehicleAttributes->radius_wheel(); //deprecated and moved to wheel data
    vehicleAttributes->set_number_wheels(0);
    vehicleAttributes->mutable_bbcenter_to_front()->CopyFrom(d_3d);
    vehicleAttributes->mutable_bbcenter_to_rear()->CopyFrom(d_3d);
    vehicleAttributes->set_ground_clearance(d_double);
    vehicleAttributes->mutable_wheel_data()->Clear();
    vehicleAttributes->set_steering_wheel_angle(d_double);
}

void OWL::Implementation::DefaultMovingObjectFactory::AssignDefaultVehicleClassification(osi3::MovingObject_VehicleClassification* const vehicleClassifcation)
{
    vehicleClassifcation->set_type(osi3::MovingObject_VehicleClassification_Type_TYPE_OTHER);
    vehicleClassifcation->set_has_trailer(false);
    vehicleClassifcation->trailer_id(); //Not set if trailer id is false
    vehicleClassifcation->mutable_light_state()->set_front_fog_light(
            osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_rear_fog_light(
            osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_head_light(
            osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_high_beam(
            osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_reversing_light(
            osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_brake_light_state(
            osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_license_plate_illumination_rear(
            osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_emergency_vehicle_illumination(
            osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_service_vehicle_illumination(
            osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    vehicleClassifcation->mutable_light_state()->set_brake_light_state(
            osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_OFF);
}

void OWL::Implementation::DefaultMovingObjectFactory::AssignDefaultMovingObjectClassification(osi3::MovingObject_MovingObjectClassification* const objectClassifcation)
{
    //ToDo: assign_lane_ids here. It's deprecated to set them on the root object
    objectClassifcation->mutable_assigned_lane_id()->Clear();
    objectClassifcation->mutable_assigned_lane_percentage()->Clear();
}


