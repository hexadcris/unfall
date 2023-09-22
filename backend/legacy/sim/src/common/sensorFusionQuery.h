/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2018-2020 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SensorFusionQuery.h
//! @brief This file provides methods to retrieve structured basic information
//!        from sensorData provided by SensorFusionImplementation::Output().
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <vector>

#include "osi3/osi_sensordata.pb.h"

namespace SensorFusionHelperFunctions
{
    static std::vector<osi3::DetectedMovingObject> RetrieveMovingObjectsBySensorId(const std::vector<int>& sensorIds, const osi3::SensorData &sensorData)
    {
        std::vector<osi3::DetectedMovingObject> result;
        auto detectedMovingObjects = sensorData.moving_object();
        for (const auto& object : detectedMovingObjects)
        {
            for (auto sensorId : object.header().sensor_id())
            {
                if(std::count(sensorIds.cbegin(), sensorIds.cend(), sensorId.value()) > 0)
                {
                    result.push_back(object);
                    break;
                }
            }
        }
        return result;
    }

    static std::vector<osi3::DetectedStationaryObject> RetrieveStationaryObjectsBySensorId(const std::vector<int>& sensorIds, const osi3::SensorData &sensorData)
    {
        std::vector<osi3::DetectedStationaryObject> result;
        auto detectedStationaryObjects = sensorData.stationary_object();
        for (const auto& object : detectedStationaryObjects)
        {
            for (auto sensorId : object.header().sensor_id())
            {
                if(std::count(sensorIds.cbegin(), sensorIds.cend(), sensorId.value()) > 0)
                {
                    result.push_back(object);
                    break;
                }
            }
        }
        return result;
    }

    //! Converts a position from the sensor coordinate system to the vehicle coordinate system
    //!
    //! \param position         position to convert
    //! \param mountingPosition sensor mounting position
    //! \return converted position
    static point_t ConvertPositionToVehicleCoordinates(const point_t& position,  const osi3::MountingPosition& mountingPosition)
    {
        point_t convertedPosition;
        point_t tempPoint;
        bt::rotate_transformer<bg::radian, double, 2, 2> sensorRotation(-mountingPosition.orientation().yaw());
        bg::transform(position, tempPoint, sensorRotation);
        bt::translate_transformer<double, 2, 2> sensorTranslate(mountingPosition.position().x(), mountingPosition.position().y());
        bg::transform(tempPoint, convertedPosition, sensorTranslate);
        return convertedPosition;
    }

    //! Converts a relative vector from the sensor coordinate system to the vehicle coordinate system
    //!
    //! \param detectedValue                vector to convert
    //! \param differenceToSensorCoordinate difference between the absolute value between sensor position and reference position (in global coordinates)
    //! \param vehicleYaw                   yaw of the vehicle in global coordinates
    //! \param mountingPosition             sensor mounting position
    //! \return converted vector
    static point_t ConvertVectorToVehicleCoordinates(const point_t& detectedValue, const point_t& differenceToSensorCoordinate, double vehicleYaw, const osi3::MountingPosition& mountingPosition)
    {
        point_t differenceInVehicleCoordinates;
        bt::rotate_transformer<bg::radian, double, 2, 2> vehicleRotation(vehicleYaw);
        bg::transform(differenceToSensorCoordinate, differenceInVehicleCoordinates, vehicleRotation);
        point_t valueInVehicleCoordinates;
        bt::rotate_transformer<bg::radian, double, 2, 2> sensorRotation(-mountingPosition.orientation().yaw());
        bg::transform(detectedValue, valueInVehicleCoordinates, sensorRotation);
        return {valueInVehicleCoordinates.x() + differenceInVehicleCoordinates.x(),
                valueInVehicleCoordinates.y() + differenceInVehicleCoordinates.y()};
    }

    //! Converts the values in a DetectedMovingObject from the sensor coordinate system to the vehicle coordinate system
    //!
    //! \param object           object to convert
    //! \param mountingPosition sensor mounting position
    //! \return converted object
    static osi3::DetectedMovingObject ConvertToVehicleCoordinates(const osi3::DetectedMovingObject& object, const osi3::MountingPosition& mountingPosition, const AgentInterface& agent)
    {
        osi3::DetectedMovingObject convertedObject = object;
        point_t position{object.base().position().x(), object.base().position().y()};
        point_t convertedPosition = ConvertPositionToVehicleCoordinates(position, mountingPosition);
        convertedObject.mutable_base()->mutable_position()->set_x(convertedPosition.x());
        convertedObject.mutable_base()->mutable_position()->set_y(convertedPosition.y());
        convertedObject.mutable_base()->mutable_orientation()->set_yaw(object.base().orientation().yaw() + mountingPosition.orientation().yaw());

        ObjectPointCustom pointMountingPosition{mountingPosition.position().x(), mountingPosition.position().y()};
        const auto differenceVelocity = agent.GetVelocity(pointMountingPosition) - agent.GetVelocity(ObjectPointPredefined::Reference);
        point_t velocity{object.base().velocity().x(), object.base().velocity().y()};
        point_t convertedVelocity = ConvertVectorToVehicleCoordinates(velocity, {differenceVelocity.x, differenceVelocity.y}, agent.GetYaw(), mountingPosition);
        convertedObject.mutable_base()->mutable_velocity()->set_x(convertedVelocity.x());
        convertedObject.mutable_base()->mutable_velocity()->set_y(convertedVelocity.y());

        const auto differenceAcceleration = agent.GetAcceleration(pointMountingPosition) - agent.GetAcceleration(ObjectPointPredefined::Reference);
        point_t acceleration{object.base().acceleration().x(), object.base().acceleration().y()};
        point_t convertedAcceleration = ConvertVectorToVehicleCoordinates(acceleration, {differenceAcceleration.x, differenceAcceleration.y}, agent.GetYaw(), mountingPosition);
        convertedObject.mutable_base()->mutable_acceleration()->set_x(convertedAcceleration.x());
        convertedObject.mutable_base()->mutable_acceleration()->set_y(convertedAcceleration.y());

        return convertedObject;
    }

    //! Converts the values in a DetectedStationaryObject from the sensor coordinate system to the vehicle coordinate system
    //!
    //! \param object           object to convert
    //! \param mountingPosition sensor mounting position
    //! \return converted object
    static osi3::DetectedStationaryObject ConvertToVehicleCoordinates(const osi3::DetectedStationaryObject& object, const osi3::MountingPosition& mountingPosition)
    {
        osi3::DetectedStationaryObject convertedObject = object;
        point_t position{object.base().position().x(), object.base().position().y()};
        point_t convertedPosition = ConvertPositionToVehicleCoordinates(position, mountingPosition);
        convertedObject.mutable_base()->mutable_position()->set_x(convertedPosition.x());
        convertedObject.mutable_base()->mutable_position()->set_y(convertedPosition.y());
        convertedObject.mutable_base()->mutable_orientation()->set_yaw(object.base().orientation().yaw() + mountingPosition.orientation().yaw());

        return convertedObject;
    }
};
