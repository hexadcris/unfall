/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  DetectedObject.h
//! @brief This class is responsible for handling the data measured by sensors
//-----------------------------------------------------------------------------
#pragma once
#include "include/worldObjectInterface.h"
#include <map>

//! Type of information detected by a sensor
enum class SensorInformationType
{
    Distance,
    PositionX,
    PositionY,
    Yaw,
    Velocity,
    Acceleration
};

class DetectedObject
{
public:
    DetectedObject(const WorldObjectInterface* worldObject) : worldObject(worldObject){}

    //-----------------------------------------------------------------------------
    //! Return the world object.
    //!
    //! @return  world object
    //-----------------------------------------------------------------------------
    const WorldObjectInterface* GetWorldObject() const;

    //-----------------------------------------------------------------------------
    //! Get a specif value of your sensor (e.g. velocity, acceleration)
    //! @param[in] informationType  information to retrieve
    //!
    //! @return  double value of required information
    //-----------------------------------------------------------------------------
    double GetSensorInformation(SensorInformationType informationType) const;

    //-----------------------------------------------------------------------------
    //! Store desired sensor value.
    //! @param[in] informationType  information to add
    //! @param[in] double           sensor value
    //-----------------------------------------------------------------------------
    void SetSensorInformation(SensorInformationType informationType, double value);

    //-----------------------------------------------------------------------------
    //! Store desired sensor value.
    //! @param[in] typeOfSensor     sensor type
    //! @param[in] idOfSensor       component ID of sensor
    //-----------------------------------------------------------------------------
    void SetSensorMetadata(std::string typeOfSensor, int Id);

    //-----------------------------------------------------------------------------
    //! Returns the type of the sensor that detected the object
    //-----------------------------------------------------------------------------
    std::string GetSensorType() const;

    //-----------------------------------------------------------------------------
    //! Returns the id of the sensor that detected the object
    //-----------------------------------------------------------------------------
    int GetSensorId() const;

    friend bool operator==(const DetectedObject &dObj1, const DetectedObject &dObj2);


private:
    const WorldObjectInterface* worldObject;
    std::map<SensorInformationType,double> sensorInformation;

    std::string sensorType = "";
    int sensorId = -999;
};
