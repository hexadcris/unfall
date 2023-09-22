/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \brief ObjectDetectorBase.cpp */
//-----------------------------------------------------------------------------

#include <memory>
#include <qglobal.h>
#include <cassert>

#include "include/parameterInterface.h"
#include "include/stochasticsInterface.h"

#include "objectDetectorBase.h"

ObjectDetectorBase::ObjectDetectorBase(
    std::string componentName,
    bool isInit,
    int priority,
    int offsetTime,
    int responseTime,
    int cycleTime,
    StochasticsInterface* stochastics,
    WorldInterface* world,
    const ParameterInterface* parameters,
    PublisherInterface * const publisher,
    const CallbackInterface* callbacks,
    AgentInterface* agent) :
    SensorInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        publisher,
        callbacks,
        agent)
{
    try
    {
        ParseBasicParameter();
    }
    catch (const std::out_of_range& e)
    {
        std::string msg{"Unable to retrieve sensor parameter: "};
        msg += e.what();
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    try
    {
        if (GetPublisher() == nullptr) { throw std::runtime_error(""); }
    }
    catch (...)
    {
        const std::string msg = COMPONENTNAME + " invalid publisher module setup";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
}

void ObjectDetectorBase::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time)
{
    Q_UNUSED(time);

    std::stringstream log;
    log << COMPONENTNAME << " (component " << GetComponentName() << ", agent " << GetAgent()->GetId() <<
        ", output data for local link " << localLinkId << ": ";
    LOG(CbkLogLevel::Debug, log.str());

    if (localLinkId == 0)
    {
        // to SensorFusion
        try
        {
            data = std::make_shared<SensorDataSignal const>(sensorData);
        }
        catch (const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void ObjectDetectorBase::AddMovingObjectToSensorData(const osi3::MovingObject& object, const point_t& ownVelocity, const point_t& ownAcceleration, const point_t& sensorPosition, double ownYaw, double ownYawRate, double ownYawAcceleration)
{
    point_t objectReferencePointGlobal{object.base().position().x(), object.base().position().y()};
    point_t objectReferencePointLocal = TransformPointToLocalCoordinates(objectReferencePointGlobal, sensorPosition, ownYaw);

    osi3::DetectedMovingObject* detectedObject = sensorData.add_moving_object();
    detectedObject->mutable_header()->add_ground_truth_id()->set_value(object.id().value());
    detectedObject->mutable_header()->add_sensor_id()->set_value(id);
    detectedObject->mutable_base()->mutable_dimension()->CopyFrom(object.base().dimension());

    detectedObject->mutable_base()->mutable_position()->set_x(objectReferencePointLocal.x());
    detectedObject->mutable_base()->mutable_position()->set_y(objectReferencePointLocal.y());
    detectedObject->mutable_base()->mutable_orientation()->set_yaw(object.base().orientation().yaw() - ownYaw);
    detectedObject->mutable_base()->mutable_orientation_rate()->set_yaw(object.base().orientation_rate().yaw() - ownYawRate);
    detectedObject->mutable_base()->mutable_orientation_acceleration()->set_yaw(object.base().orientation_acceleration().yaw() - ownYawAcceleration);
    point_t objectVelocity{object.base().velocity().x(), object.base().velocity().y()};
    point_t relativeVelocity = CalculateRelativeVector(objectVelocity, ownVelocity, ownYaw);
    detectedObject->mutable_base()->mutable_velocity()->set_x(relativeVelocity.x());
    detectedObject->mutable_base()->mutable_velocity()->set_y(relativeVelocity.y());
    point_t objectAcceleration{object.base().acceleration().x(), object.base().acceleration().y()};
    point_t relativeAcceleration = CalculateRelativeVector(objectAcceleration, ownAcceleration, ownYaw);
    detectedObject->mutable_base()->mutable_acceleration()->set_x(relativeAcceleration.x());
    detectedObject->mutable_base()->mutable_acceleration()->set_y(relativeAcceleration.y());
}

void ObjectDetectorBase::AddStationaryObjectToSensorData(const osi3::StationaryObject& object, const point_t& sensorPosition, double yaw)
{
    point_t objectReferencePointGlobal{object.base().position().x(), object.base().position().y()};
    point_t objectReferencePointLocal = TransformPointToLocalCoordinates(objectReferencePointGlobal, sensorPosition, yaw);

    osi3::DetectedStationaryObject* detectedObject = sensorData.add_stationary_object();
    detectedObject->mutable_header()->add_ground_truth_id()->set_value(object.id().value());
    detectedObject->mutable_header()->add_sensor_id()->set_value(id);
    detectedObject->mutable_base()->mutable_dimension()->CopyFrom(object.base().dimension());

    detectedObject->mutable_base()->mutable_position()->set_x(objectReferencePointLocal.x());
    detectedObject->mutable_base()->mutable_position()->set_y(objectReferencePointLocal.y());
    detectedObject->mutable_base()->mutable_orientation()->set_yaw(object.base().orientation().yaw() - yaw);
}

Position ObjectDetectorBase::GetAbsolutePosition()
{
    Position absolutePosition;

    double posX = GetAgent()->GetPositionX();
    double posY = GetAgent()->GetPositionY();
    double yaw = GetAgent()->GetYaw();

    absolutePosition.xPos = posX + position.longitudinal * std::cos(yaw) - position.lateral * std::sin(yaw);
    absolutePosition.yPos = posY + position.longitudinal * std::sin(yaw) + position.lateral * std::cos(yaw);
    absolutePosition.yawAngle = yaw;

    return absolutePosition;
}

osi3::SensorData ObjectDetectorBase::ApplyLatency(int time, osi3::SensorData currentSensorData)
{
    detectedObjectsBuffer.push_back({time + latencyInMs, currentSensorData});

    osi3::SensorData detectedObjectsByLatency;

    while (!detectedObjectsBuffer.empty() && time >= detectedObjectsBuffer.front().first)
    {
        detectedObjectsByLatency = detectedObjectsBuffer.front().second;
        detectedObjectsBuffer.pop_front();
    }

    return detectedObjectsByLatency;
}

bool ObjectDetectorBase::HasDetectionError()
{
    if (failureProbability == 0.0)
    {
        return false;
    }
    return GetStochastics()->GetUniformDistributed(0, 1) < failureProbability;
}

osi3::SensorViewConfiguration ObjectDetectorBase::GenerateSensorViewConfiguration()
{
    osi3::SensorViewConfiguration viewConfiguration;

    viewConfiguration.mutable_sensor_id()->set_value(static_cast<unsigned int>(id));

    viewConfiguration.mutable_mounting_position()->mutable_orientation()->set_pitch(position.pitch);
    viewConfiguration.mutable_mounting_position()->mutable_orientation()->set_roll(position.roll);
    viewConfiguration.mutable_mounting_position()->mutable_orientation()->set_yaw(position.yaw);

    viewConfiguration.mutable_mounting_position()->mutable_position()->set_x(position.longitudinal);
    viewConfiguration.mutable_mounting_position()->mutable_position()->set_y(position.lateral);
    viewConfiguration.mutable_mounting_position()->mutable_position()->set_z(position.height);

    return viewConfiguration;
}

polygon_t ObjectDetectorBase::CalculateBoundingBox(const osi3::Dimension3d dimension, const  osi3::Vector3d position, const osi3::Orientation3d orientation)
{
    double halfLength = dimension.length() / 2.0;
    double halfWidth = dimension.width() / 2.0;
    double rotation = orientation.yaw();

    double x = position.x();
    double y = position.y();


    point_t boxPoints[]
    {
        {-halfLength, -halfWidth},
        {-halfLength,  halfWidth},
        {halfLength,  halfWidth},
        {halfLength, -halfWidth},
        {-halfLength, -halfWidth}
    };

    polygon_t box;
    polygon_t boxTemp;
    bg::append(box, boxPoints);

    bt::translate_transformer<double, 2, 2> translate(x, y);

    // rotation in mathematical negativ order (boost) -> invert to match
    bt::rotate_transformer<bg::radian, double, 2, 2> rotate(-rotation);

    bg::transform(box, boxTemp, rotate);
    bg::transform(boxTemp, box, translate);
    bg::correct(box);

    return box;
}

point_t ObjectDetectorBase::TransformPointToLocalCoordinates(point_t point, point_t sensorPositionGlobal, double yaw)
{
    point_t tempPoint;
    point_t pointLocal;
    bt::translate_transformer<double, 2, 2> sensorPositionTranslate(-sensorPositionGlobal.x(), -sensorPositionGlobal.y());
    bg::transform(point, tempPoint, sensorPositionTranslate);
    bt::rotate_transformer<bg::radian, double, 2, 2> sensorRotation(yaw);
    bg::transform(tempPoint, pointLocal, sensorRotation);

    return pointLocal;
}

polygon_t ObjectDetectorBase::TransformPolygonToGlobalCoordinates(polygon_t polygon, point_t sensorPositionGlobal, double yaw) const
{
    polygon_t tempBox;
    polygon_t polygonGlobal;
    bt::rotate_transformer<bg::radian, double, 2, 2> sensorRotation(-yaw);
    bg::transform(polygon, tempBox, sensorRotation);
    bt::translate_transformer<double, 2, 2> sensorPositionTranslate(sensorPositionGlobal.x(), sensorPositionGlobal.y());
    bg::transform(tempBox, polygonGlobal, sensorPositionTranslate);

    return polygonGlobal;
}

point_t ObjectDetectorBase::CalculateGlobalSensorPosition(point_t vehiclePosition, double yaw) const
{
    point_t sensorPositionVehicle{position.longitudinal, position.lateral};
    point_t sensorPositionGlobal;
    point_t tempPoint;
    bt::rotate_transformer<bg::radian, double, 2, 2> vehicleRotation(-yaw);
    bg::transform(sensorPositionVehicle, tempPoint, vehicleRotation);
    bt::translate_transformer<double, 2, 2> vehicleTranslate(vehiclePosition.x(), vehiclePosition.y());
    bg::transform(tempPoint, sensorPositionGlobal, vehicleTranslate);

    return sensorPositionGlobal;
}

point_t ObjectDetectorBase::CalculateRelativeVector(const point_t absolute, const point_t own, double yaw)
{
    point_t difference{absolute};
    bg::subtract_point(difference, own);
    point_t result;
    bt::rotate_transformer<bg::radian, double, 2, 2> sensorRotation(yaw);
    bg::transform(difference, result, sensorRotation);
    return result;
}

void ObjectDetectorBase::ParseBasicParameter()
{
    const auto& parameters = GetParameters();
    const auto& doubleParameters = parameters->GetParametersDouble();

    id = parameters->GetParametersInt().at("Id");
    failureProbability = doubleParameters.at("FailureProbability") < MIN_FAILURE_PROBABILITY ?
                         0.0 : doubleParameters.at("FailureProbability");
    latency = doubleParameters.at("Latency");
    latencyInMs = static_cast<int>(latency * 1000.0);

    position.longitudinal = doubleParameters.at("Longitudinal");
    position.lateral = doubleParameters.at("Lateral");
    position.height = doubleParameters.at("Height");
    position.pitch = doubleParameters.at("Pitch");
    position.yaw = doubleParameters.at("Yaw");
    position.roll = doubleParameters.at("Roll");
}

const osi3::MovingObject* ObjectDetectorBase::FindHostVehicleInSensorView(const osi3::SensorView& sensorView)
{
    const auto hostVehicleIt = std::find_if(sensorView.global_ground_truth().moving_object().cbegin(),
                                            sensorView.global_ground_truth().moving_object().cend(),
                                            [&sensorView](const osi3::MovingObject& object)
                                            {
                                                return object.id().value() == sensorView.host_vehicle_id().value();
                                            });

    if (hostVehicleIt == sensorView.global_ground_truth().moving_object().cend())
    {
        throw std::runtime_error("Host vehicle not in SensorView");
    }

    return &(*hostVehicleIt);
}

point_t ObjectDetectorBase::GetSensorPosition() const
{
    const ObjectPointCustom mountingPosition{position.longitudinal, position.lateral};
    const auto sensorPosition = GetAgent()->GetAbsolutePosition(mountingPosition);
    return {sensorPosition.x, sensorPosition.y};
}
