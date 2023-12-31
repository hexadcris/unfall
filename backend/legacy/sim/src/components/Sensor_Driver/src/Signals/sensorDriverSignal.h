/********************************************************************************
 * Copyright (c) 2018-2019 AMFD GmbH
 *               2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SensorDriverSignal.h
//! @brief This file contains all functions for class
//! SensorDriverSignal
//!
//! This class contains all functionality of the signal.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "include/signalInterface.h"
#include "sensor_driverDefinitions.h"

class SensorDriverSignal: public SignalInterface
{
public:
    static constexpr char COMPONENTNAME[] {"SensorDriverHumanSignal"};

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    SensorDriverSignal(OwnVehicleInformation ownVehicleInformation,
                       TrafficRuleInformation trafficRuleInformation,
                       GeometryInformation geometryInformation,
                       SurroundingObjects surroundingObjects) :
        ownVehicleInformation(ownVehicleInformation),
        trafficRuleInformation(trafficRuleInformation),
        geometryInformation(geometryInformation),
        surroundingObjects(surroundingObjects)
    {}

    SensorDriverSignal(const SensorDriverSignal&) = default;
    SensorDriverSignal(SensorDriverSignal&&) = default;
    SensorDriverSignal& operator=(const SensorDriverSignal&) = default;
    SensorDriverSignal& operator=(SensorDriverSignal&&) = default;

    virtual ~SensorDriverSignal()
    {}

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return                       Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        return COMPONENTNAME;
    }

    //! Returns the information about the own vehicle
    virtual OwnVehicleInformation GetOwnVehicleInformation() const
    {
        return ownVehicleInformation;
    }

    //! Returns the traffic rule information
    virtual TrafficRuleInformation GetTrafficRuleInformation() const
    {
        return trafficRuleInformation;
    }

    //! Returns the lane geometry information
    virtual GeometryInformation GetGeometryInformation() const
    {
        return geometryInformation;
    }

    //! Returns the information about the surrouding objects
    virtual SurroundingObjects GetSurroundingObjects() const
    {
        return surroundingObjects;
    }

private:
    //! \brief Struct for all sensor data concerning the own vehicle
    OwnVehicleInformation ownVehicleInformation;
    //! \brief Struct for all sensor data concerning mesosopic information
    TrafficRuleInformation trafficRuleInformation;
    //! \brief Struct for all sensor data concerning mesosopic information
    GeometryInformation geometryInformation;
    //! \brief Struct for all sensor data concerning surrounding objects
    SurroundingObjects surroundingObjects;
};
