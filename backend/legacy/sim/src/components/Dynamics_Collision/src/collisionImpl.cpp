/********************************************************************************
 * Copyright (c) 2017-2018 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  dynamics_collisionImplementation.cpp
//! @brief This file contains the implementation header file
//-----------------------------------------------------------------------------

#include <qglobal.h>

#include "collisionImpl.h"
#include "common/hypot.h"
#include "include/worldInterface.h"
#include "components/common/vehicleProperties.h"

DynamicsCollisionImplementation::DynamicsCollisionImplementation(std::string componentName,
                                                                 bool isInit,
                                                                 int priority,
                                                                 int offsetTime,
                                                                 int responseTime,
                                                                 int cycleTime,
                                                                 StochasticsInterface *stochastics,
                                                                 WorldInterface *world,
                                                                 const ParameterInterface *parameters,
                                                                 PublisherInterface * const publisher,
                                                                 const CallbackInterface *callbacks,
                                                                 AgentInterface *agent)
    : UnrestrictedModelInterface { componentName,
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
                                   agent },
      dynamicsSignal{}
{
    dynamicsSignal.componentState = ComponentState::Disabled;
}

void DynamicsCollisionImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);
}

void DynamicsCollisionImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if(localLinkId == 0)
    {
        try
        {
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

void DynamicsCollisionImplementation::Trigger(int time)
{
    Q_UNUSED(time);

    if (GetAgent()->GetCollisionPartners().size() > numberOfCollisionPartners)
    {
        numberOfCollisionPartners = GetAgent()->GetCollisionPartners().size();
        isActive = true;
        dynamicsSignal.componentState = ComponentState::Acting;
        bool collisionWithFixedObject = false;

        //Calculate new velocity by inelastic collision
        auto weight = helper::map::query(GetAgent()->GetVehicleModelParameters().properties, vehicle::properties::Mass);
        THROWIFFALSE(weight.has_value(), "Mass was not defined in VehicleCatalog");

        double sumOfWeights = weight.value();
        double sumOfImpulsesX = GetAgent()->GetVelocity().x * weight.value();
        double sumOfImpulsesY = GetAgent()->GetVelocity().y * weight.value();

        auto collisionPartners = GetAgent()->GetCollisionPartners();
        for (const auto &partner : collisionPartners)
        {
            if (partner.first == ObjectTypeOSI::Object)
            {
                collisionWithFixedObject = true;
                break;
            }
            const AgentInterface* partnerAgent = GetWorld()->GetAgent(partner.second);
            if (partnerAgent != nullptr)
            {
                weight = helper::map::query(partnerAgent->GetVehicleModelParameters().properties, vehicle::properties::Mass);
                THROWIFFALSE(weight.has_value(), "Mass was not defined in VehicleCatalog");

                sumOfWeights += weight.value();
                sumOfImpulsesX += partnerAgent->GetVelocity().x * weight.value();
                sumOfImpulsesY += partnerAgent->GetVelocity().y * weight.value();
            }
        }

        if (collisionWithFixedObject)
        {
            velocity = 0.0;
        }
        else
        {
            double velocityX = sumOfImpulsesX / sumOfWeights;
            double velocityY = sumOfImpulsesY / sumOfWeights;
            velocity = openpass::hypot(velocityX, velocityY);
            if (velocityY > 0.0)
            {
                movingDirection = std::acos(velocityX / velocity);
            }
            else if (velocity != 0.0)
            {
                movingDirection = -std::acos(velocityX / velocity);
            }
            else
            {
                movingDirection = 0.0;
            }
        }
        dynamicsSignal.yaw = GetAgent()->GetYaw();
        dynamicsSignal.yawRate = 0;
        dynamicsSignal.yawAcceleration = 0;
    }

    if (isActive)
    {
        const double deceleration = 10.0;
        velocity -= deceleration * GetCycleTime() * 0.001;

        if (velocity < 0.0)
        {
            isActive = false;
        }

        velocity = std::max(0.0, velocity);
        // change of path coordinate since last time step [m]
        double ds = velocity * static_cast<double>(GetCycleTime()) * 0.001;
        // change of inertial x-position due to ds and yaw [m]
        double dx = ds * std::cos(movingDirection);
        // change of inertial y-position due to ds and yaw [m]
        double dy = ds * std::sin(movingDirection);
        // new inertial x-position [m]
        double x = GetAgent()->GetPositionX() + dx;
        // new inertial y-position [m]
        double y = GetAgent()->GetPositionY() + dy;

        dynamicsSignal.velocity = velocity;
        dynamicsSignal.acceleration = isActive ? -deceleration : 0.0;
        dynamicsSignal.positionX = x;
        dynamicsSignal.positionY = y;
        dynamicsSignal.travelDistance = ds;
    }
}
