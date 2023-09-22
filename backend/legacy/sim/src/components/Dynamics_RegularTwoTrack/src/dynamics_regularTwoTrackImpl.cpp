/********************************************************************************
 * Copyright (c) 2018-2019 AMFD GmbH
 *               2019-2020 ITK Engineering GmbH
 *               2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
//-----------------------------------------------------------------------------
//! @file  dynamics_regularTwoTrackImplementation.cpp
//! @brief This file contains the implementation header file
//-----------------------------------------------------------------------------

/**
 * @defgroup module_tt Two-track vehicle model
 * The dynamics of the vehicle is modelled in 2 dimensions. In the sub items the principle function is described in order of execution.
 */
/** @addtogroup module_tt
 * Abbreviations:
 * - COG = center-of-gravity
 * - CS = coordinate system
*/

/**
 * @ingroup module_tt
 * @defgroup init_tt Initialization
*/
/**
 * @ingroup module_tt
 * @defgroup sim_step_00_tt Entry
*/
/**
 * @ingroup module_tt
 * @defgroup sim_step_10_tt Process
*/
/**
 * @ingroup module_tt
 * @defgroup sim_step_20_tt Output
*/

#include <memory>
#include <qglobal.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "dynamics_regularTwoTrackImpl.h"
#include "common/longitudinalSignal.h"
#include "common/accelerationSignal.h"
#include "common/steeringSignal.h"
#include "common/parametersVehicleSignal.h"
#include "common/globalDefinitions.h"
#include "common/commonTools.h"
#include "components/common/vehicleProperties.h"

DynamicsRegularTwoTrackImplementation::~DynamicsRegularTwoTrackImplementation()
{
    #ifdef QT_DEBUG
        logFile.close();
    #endif
}

void DynamicsRegularTwoTrackImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if (localLinkId == 0)
    {
        const std::shared_ptr<ComponentStateSignalInterface const> stateSignal = std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
        if(stateSignal->componentState == ComponentState::Acting)
        {
            const std::shared_ptr<LongitudinalSignal const> signal = std::dynamic_pointer_cast<LongitudinalSignal const>(data);
            if (!signal)
            {
                const std::string msg = COMPONENTNAME + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }
            throttlePedal.SetDefaultValue(signal->accPedalPos);
            brakePedal.SetDefaultValue(signal->brakePedalPos);
        }
    }
    else if (localLinkId == 1)
    {
        const std::shared_ptr<ComponentStateSignalInterface const> stateSignal = std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
        if(stateSignal->componentState == ComponentState::Acting)
        {
            const std::shared_ptr<SteeringSignal const> signal = std::dynamic_pointer_cast<SteeringSignal const>(data);
            if (!signal)
            {
                const std::string msg = COMPONENTNAME + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }
            angleTireFront.SetDefaultValue(signal->steeringWheelAngle);
        }
    }
    else if (localLinkId == 100)
    {
        // from ParametersAgent
        const std::shared_ptr<ParametersVehicleSignal const> signal = std::dynamic_pointer_cast<ParametersVehicleSignal const>(data);
        if (!signal)
        {
            const std::string msg = COMPONENTNAME + " invalid signaltype";
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

void DynamicsRegularTwoTrackImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if(localLinkId == 0)
    {
        try {
            data = std::make_shared<DynamicsSignal const>(dynamicsSignal);
        }
        catch(const std::bad_alloc&)
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

void DynamicsRegularTwoTrackImplementation::Trigger(int time)
{
    Q_UNUSED(time);

    if (timeStep <= std::numeric_limits<double>::epsilon()) {
        Init();
    }

    /** @addtogroup sim_step_00_tt
     * Read and update previous vehicle's state:
     *  - global position (cartesian coordinates)
     *  - direction of vehicle's longitudinal axes (angle in polar coordinates)
     *  - vehicle's longitudinal and lateral velocity in vehicle's CS
     *  - vehicle's rotational velociy
     *  - vehicle's longitudinal and lateral acceleration in vehicle's CS
     *  - vehicle's rotational acceleration
    */
    ReadPreviousState();

    #ifdef QT_DEBUG
        logFile << (std::to_string(time) + ";"
                    + std::to_string(GetAgent()->GetId()) + ";"
                    + std::to_string(positionCar.x) + ";"
                    + std::to_string(positionCar.y) + ";"
                    + std::to_string(velocityCar.x) + ";"
                    + std::to_string(velocityCar.y) + ";"
                    + std::to_string(GetAgent()->GetYaw()) + ";")
                   << std::endl;
    #endif

    vehicle->SetVelocity(velocityCar, yawVelocity);

    /** @addtogroup sim_step_10_tt
     * Apply acceleration/deceleration intentions:
     *  - calculate tire torques due to engine
     *  - calculate tire torques due to braking by
     *      - driver (pedal state)
     *      - assistant systems (selective single wheel braking)
    */
    vehicle->DriveTrain(std::clamp(throttlePedal.GetValue(), 0.0, 1.0),
                        std::clamp(brakePedal.GetValue(), 0.0, 1.0),
                        brakeSuperpose);

    /** @addtogroup sim_step_10_tt
     * Apply tire forces at the tire/road interface:
     *  - calculate longitudinal tire slips and forces
     *  - calculate lateral tire slips and forces
     *  - calculate friction forces
    */
    vehicle->ForceLocal(timeStep, angleTireFront.GetValue(), forceWheelVertical);

    /** @addtogroup sim_step_10_tt
     * Combine local tire forces to a global force at the vehicle's body:
     *  - total longitudinal force
     *  - total lateral force
     *  - air drag
     *  - total rotational momentum
    */
    vehicle->ForceGlobal();

    /** @addtogroup sim_step_20_tt
     * Perform a translational Euler step:
     *  - calculate next vehicle's position from prevoius velocity values
     *  - calculate new velocity from previous acceleration values
     *  - calculate new acceleration from global forces
    */
    NextStateTranslation();

    /** @addtogroup sim_step_20_tt
     * Perform a rotational Euler step:
     *  - calculate vehicle's orientation from previous rotational velocity
     *  - calculate vehicle's rotational velocity from previous rotational acceleration
     *  - calculate vehicle's rotational acceleration from the total rotational momentum
    */
    NextStateRotation();

    /** @addtogroup sim_step_20_tt
     * Write actual vehicle's state:
     *  - global position (cartesian coordinates)
     *  - direction of vehicle's longitudinal axes (angle in polar coordinates)
     *  - vehicle's longitudinal and lateral velocity in vehicle's CS
     *  - vehicle's rotational velociy
     *  - vehicle's longitudinal and lateral acceleration in vehicle's CS
     *  - vehicle's rotational acceleration
     *  - inertia forces on vehicle's COG
    */
    NextStateSet();
}

double DynamicsRegularTwoTrackImplementation::GetWheelbase() const
{
    return GetAgent()->GetVehicleModelParameters().frontAxle.positionX - GetAgent()->GetVehicleModelParameters().rearAxle.positionX;
}

double DynamicsRegularTwoTrackImplementation::GetWeight() const
{
    const auto weight = helper::map::query(GetAgent()->GetVehicleModelParameters().properties, vehicle::properties::Mass);
    THROWIFFALSE(weight.has_value(), "Mass was not defined in VehicleCatalog");
    return weight.value();
}

void DynamicsRegularTwoTrackImplementation::Init()
{
    std::map<std::string, double> parameterMapDoubleExternal = GetParameters()->GetParametersDouble();
    radiusTire.SetValue(parameterMapDoubleExternal.find("radiusTire")->second);
    muTireMax.SetValue(parameterMapDoubleExternal.find("muTireMax")->second);
    muTireSlide.SetValue(parameterMapDoubleExternal.find("muTireSlide")->second);
    slipTireMax.SetValue(parameterMapDoubleExternal.find("slipTireMax")->second);
    powerEngineMax.SetValue(parameterMapDoubleExternal.find("powerEngineMax")->second);

    torqueBrakeMin.SetValue(-std::fabs(parameterMapDoubleExternal.find("torqueBrakeMin")->second));

    timeStep = (double)GetCycleTime() / 1000.0;

    vehicle = std::make_unique<VehicleSimpleTT>();

    /** @addtogroup init_tt
     * Define vehicle's body and engine characteristics:
     *  - total mass
     *  - power
     *  - maximum brake torque
    */
    vehicle->InitSetEngine(GetWeight(), powerEngineMax.GetValue(), torqueBrakeMin.GetValue());

    /** @addtogroup init_tt
     * Define vehicle's geometry:
     *  - set the height of the COG
     *  - set the longitudinal position of the COG
     *  - set the wheelbase
     *  - set the track width
    */
    vehicle->InitSetGeometry(GetWheelbase(), 0.0,
                             GetAgent()->GetVehicleModelParameters().rearAxle.trackWidth, 0.0);

    /** @addtogroup init_tt
     * Define vehicle's tire properties:
     *  - set initial velocity
     *  - set peak tire force
     *  - set force at full slide
     *  - set the slip corresponding to peak tire force
     *  - set the radius of the tire
     *  - set the road/tire friction coefficient
    */

    const auto frictionCoeff = helper::map::query(GetAgent()->GetVehicleModelParameters().properties, vehicle::properties::FrictionCoefficient);
    THROWIFFALSE(frictionCoeff.has_value(), "FrictionCoefficient was not defined in VehicleCatalog");

    vehicle->InitSetTire(GetAgent()->GetVelocity().Projection(GetAgent()->GetYaw()),
                         muTireMax.GetValue(), muTireSlide.GetValue(),
                         slipTireMax.GetValue(), radiusTire.GetValue(), frictionCoeff.value());

    forceWheelVertical = {
        vehicle->forceTireVerticalStatic[0],
        vehicle->forceTireVerticalStatic[1],
        vehicle->forceTireVerticalStatic[2],
        vehicle->forceTireVerticalStatic[3]};

    #ifdef QT_DEBUG
        logFile.open("dataLog_TT_" + std::to_string(GetAgent()->GetId()) + ".csv", std::ios::trunc);
        logFile << ("time;agent;position_X;position_Y;velocity_X;velocity_Y;yawAngle\n");
    #endif
}

void DynamicsRegularTwoTrackImplementation::ReadPreviousState()
{
    // actual state
    double midRearAxleX = GetAgent()->GetPositionX(); // reference point (rear axle) in global CS
    double midRearAxleY = GetAgent()->GetPositionY(); // reference point (rear axle) in global CS

    yawAngle = GetAgent()->GetYaw(); // global CS
    const double wheelbase = GetWheelbase();
    positionCar.x = midRearAxleX + std::cos(yawAngle) * wheelbase / 2.0; // geometrical center of vehicle in global CS
    positionCar.y = midRearAxleY + std::sin(yawAngle) * wheelbase / 2.0; // geometrical center of vehicle in global CS

    velocityCar.x = GetAgent()->GetVelocity().Projection(GetAgent()->GetYaw()); // car's CS
    velocityCar.y = GetAgent()->GetVelocity().Projection(GetAgent()->GetYaw() + M_PI_2); // car's CS
    yawVelocity = GetAgent()->GetYawRate();

    yawAcceleration = GetAgent()->GetYawAcceleration();
    accelerationCar.x = GetAgent()->GetAcceleration().Projection(GetAgent()->GetYaw()); // car's CS

}

void DynamicsRegularTwoTrackImplementation::NextStateTranslation()
{

    // update position (constant velocity step)
    velocityCar.Rotate(yawAngle); // global CS
    positionCar =  positionCar + velocityCar*timeStep;
    velocityCar.Rotate(- yawAngle); // vehicle CS

    // update velocity
    Common::Vector2d velocityCarNew = velocityCar + accelerationCar*timeStep;

    // update acceleration
    const double weight = GetWeight();
    if (weight >= 1.0) {
        accelerationCar = vehicle->forceTotalXY * (1 / weight);
    }

    // correct velocity and acceleration for zero-crossing
    if (velocityCarNew.x*velocityCar.x<0.0)
    {
        velocityCar.x = 0.0;
        accelerationCar.x = 0.0;
    }
    else
    {
        velocityCar.x = velocityCarNew.x;
    }
    if (velocityCarNew.y*velocityCar.y<0.0)
    {
        velocityCar.y = 0.0;
        accelerationCar.y = 0.0;
    }
    else
    {
        velocityCar.y = velocityCarNew.y;
    }

}

void DynamicsRegularTwoTrackImplementation::NextStateRotation()
{

    // preserve directions of velocity and acceleration
    velocityCar.Rotate(yawAngle);
    accelerationCar.Rotate(yawAngle);

    // update yaw angle
    yawAngle = yawAngle + timeStep * yawVelocity;

    // update yaw velocity
    double yawVelocityNew = yawVelocity + yawAcceleration * timeStep;

    // update acceleration
    double momentInertiaYaw = CommonHelper::CalculateMomentInertiaYaw(GetWeight(),
                                                                      GetAgent()->GetLength(),
                                                                      GetAgent()->GetWidth());
    if (momentInertiaYaw >= 1.0) {
        yawAcceleration = vehicle->momentTotalZ / momentInertiaYaw;
    }

    // correct velocity and acceleration for zero-crossing
    if (yawVelocityNew*yawVelocity<0.0)
    {
        yawVelocity = 0.0;
        yawAcceleration = 0.0;
    }
    else
    {
        yawVelocity = yawVelocityNew;
    }

    // reassign directions of velocity and acceleration
    velocityCar.Rotate(-yawAngle);
    accelerationCar.Rotate(-yawAngle);
}

void DynamicsRegularTwoTrackImplementation::NextStateSet()
{
    const double wheelbase = GetWheelbase();
    double midRearAxleX = positionCar.x - std::cos(yawAngle) * wheelbase / 2.0; // reference point (rear axle) in global CS
    double midRearAxleY = positionCar.y - std::sin(yawAngle) * wheelbase / 2.0; // reference point (rear axle) in global CS

    // update position (constant acceleration step)
    dynamicsSignal.acceleration = accelerationCar.x;
    dynamicsSignal.velocity = velocityCar.x;
    dynamicsSignal.positionX = midRearAxleX;
    dynamicsSignal.positionY = midRearAxleY;
    dynamicsSignal.travelDistance = velocityCar.x * GetCycleTime() * 0.001;
    dynamicsSignal.steeringWheelAngle = angleTireFront.GetValue();
    dynamicsSignal.yawRate = yawVelocity;
    dynamicsSignal.yawAcceleration = yawAcceleration;
    dynamicsSignal.yaw = yawAngle;
}
