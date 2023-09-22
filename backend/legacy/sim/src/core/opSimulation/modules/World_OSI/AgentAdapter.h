/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2016-2021 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentAdapter.h
//! @brief This file implements the wrapper for the agent so it can
//!        interact with the world.
//!        It permits a simple implementation of delegation concepts
//!        (composition vs. inheritance).
//-----------------------------------------------------------------------------

#pragma once

#include <QtGlobal>
#include <functional>

#include "include/agentInterface.h"
#include "include/callbackInterface.h"
#include "include/trafficObjectInterface.h"
#include "include/worldInterface.h"
#include "include/stochasticsInterface.h"
#include "egoAgent.h"
#include "Localization.h"
#include "WorldData.h"
#include "WorldDataQuery.h"
#include "WorldObjectAdapter.h"

constexpr double zeroBaseline = 1e-9;

/*!
 * \brief The AgentAdapter class
 * This class is a adapter for the communication between the framework and world.
 */
class AgentAdapter final : public WorldObjectAdapter, public AgentInterface
{
public:
    const std::string MODULENAME = "AGENTADAPTER";

    AgentAdapter(OWL::Interfaces::MovingObject& mo,
                 WorldInterface* world,
                 const CallbackInterface* callbacks,
                 const World::Localization::Localizer& localizer);

    ~AgentAdapter() override;

    void InitParameter(const AgentBlueprintInterface& agentBlueprint) override;

    ObjectTypeOSI GetType() const override
    {
        return ObjectTypeOSI::Vehicle;
    }

    int GetId() const override
    {
        return static_cast<int>(GetBaseTrafficObject().GetId());;
    }

    EgoAgentInterface& GetEgoAgent() override
    {
        return egoAgent;
    }

    std::string GetVehicleModelType() const override
    {
        return vehicleModelType;
    }

    std::string GetScenarioName() const override
    {
        return objectName;
    }

    std::string GetDriverProfileName() const override
    {
        return driverProfileName;
    }

    double GetSpeedGoalMin() const override
    {
        return speedGoalMin;
    }

    double GetEngineSpeed() const override
    {
        return engineSpeed;
    }

    double GetDistanceReferencePointToLeadingEdge() const override
    {
        return GetBaseTrafficObject().GetDistanceReferencePointToLeadingEdge();
    }

    int GetGear() const override
    {
        return currentGear;
    }

    double GetEffAccelPedal() const override
    {
        return accelPedal;
    }

    double GetEffBrakePedal() const override
    {
        return brakePedal;
    }

    double GetSteeringWheelAngle() const override
    {
        return steeringWheelAngle;
    }

    double GetMaxAcceleration() const override
    {
        return maxAcceleration;
    }

    double GetMaxDeceleration() const override
    {
        return maxDeceleration;
    }

    bool GetHeadLight() const override;

    bool GetHighBeamLight() const override;

    bool GetHorn() const override
    {
        return hornSwitch;
    }

    bool GetFlasher() const override
    {
        return flasherSwitch;
    }

    std::vector<std::string> GetRoads(ObjectPoint point) const override;

    LightState GetLightState() const override;

    const RoadIntervals &GetTouchedRoads() const override
    {
        return locateResult.touchedRoads;
    }

    Common::Vector2d GetAbsolutePosition(const ObjectPoint& objectPoint) const override;

    const GlobalRoadPositions& GetRoadPosition(const ObjectPoint& point) const override;

    void UpdateCollision(std::pair<ObjectTypeOSI, int> collisionPartner) override;

    std::vector<std::pair<ObjectTypeOSI, int>> GetCollisionPartners() const override
    {
        return collisionPartners;
    }

    PostCrashVelocity GetPostCrashVelocity() const override
    {
        return postCrashVelocity;
    }

    VehicleModelParameters GetVehicleModelParameters() const override
    {
        return vehicleModelParameters;
    }

    void SetPostCrashVelocity(PostCrashVelocity postCrashVelocity) override
    {
        this->postCrashVelocity = postCrashVelocity;
    }

    void SetPositionX(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            GetBaseTrafficObject().SetX(value);
        });
    }

    void SetPositionY(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            GetBaseTrafficObject().SetY(value);
        });
    }


    void SetVelocity(double velocity) override
    {
        world->QueueAgentUpdate([this, velocity]()
        {
            GetBaseTrafficObject().SetAbsVelocity(velocity);
        });
    }

    void SetVelocityVector(double vx, double vy, double vz) override
    {
        world->QueueAgentUpdate([this, vx, vy, vz]()
        {
            OWL::Primitive::AbsVelocity velocity{vx, vy, vz};
            GetBaseTrafficObject().SetAbsVelocity(velocity);
        });
    }

    void SetAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            GetBaseTrafficObject().SetAbsAcceleration(value);
        });
    }

    void SetYaw(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            UpdateYaw(value);
        });
    }

    void SetRoll(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            UpdateRoll(value);
        });
    }

    void SetYawRate(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            OWL::Primitive::AbsOrientationRate orientationRate = GetBaseTrafficObject().GetAbsOrientationRate();
            orientationRate.yawRate = value;
            GetBaseTrafficObject().SetAbsOrientationRate(orientationRate);
        });
    }

    void SetYawAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            OWL::Primitive::AbsOrientationAcceleration orientationAcceleration = GetBaseTrafficObject().GetAbsOrientationAcceleration();
            orientationAcceleration.yawAcceleration = value;
            GetBaseTrafficObject().SetAbsOrientationAcceleration(orientationAcceleration);
        });
    }

    void SetCentripetalAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]() {
            centripetalAcceleration = value;

            // Code seems to work incorrectly here, further investigation needed
            // OWL::Primitive::AbsAcceleration absAcceleration = GetBaseTrafficObject().GetAbsAcceleration();
            // OWL::Primitive::AbsOrientation absOrientation = GetBaseTrafficObject().GetAbsOrientation();

            // Common::Vector2d vec(absAcceleration.ax, absAcceleration.ay);

            // // Rotate reference frame from groundTruth to car and back
            // vec.Rotate(-absOrientation.yaw);
            // vec.y = value;
            // vec.Rotate(absOrientation.yaw);

            // absAcceleration.ax = vec.x;
            // absAcceleration.ay = vec.y;

            // GetBaseTrafficObject().SetAbsAcceleration(absAcceleration);
        });
    }

    void SetTangentialAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]() {
            tangentialAcceleration = value;

            // Code seems to work incorrectly here, further investigation needed
            // OWL::Primitive::AbsAcceleration absAcceleration = GetBaseTrafficObject().GetAbsAcceleration();
            // OWL::Primitive::AbsOrientation absOrientation = GetBaseTrafficObject().GetAbsOrientation();

            // Common::Vector2d vec(absAcceleration.ax, absAcceleration.ay);

            // // Rotate reference frame from groundTruth to car and back
            // vec.Rotate(-absOrientation.yaw);
            // vec.x = value;
            // vec.Rotate(absOrientation.yaw);

            // absAcceleration.ax = vec.x;
            // absAcceleration.ay = vec.y;

            // GetBaseTrafficObject().SetAbsAcceleration(absAcceleration);
        });
    }

    void SetWheelsRotationRateAndOrientation(double _steeringWheelAngle, double velocity, double acceleration) override
    {
        if(vehicleModelParameters.vehicleType == AgentVehicleType::Pedestrian)
            return;

        auto dTime = (velocity - previousVelocity) / acceleration;
        previousVelocity = velocity;
        auto wheelRadiusFront = vehicleModelParameters.frontAxle.wheelDiameter / 2.0;
        auto wheelRadiusRear = vehicleModelParameters.rearAxle.wheelDiameter / 2.0;
        try {
            auto steeringToWheelYawRatio = vehicleModelParameters.properties.at("SteeringRatio");
            GetBaseTrafficObject().SetWheelsRotationRateAndOrientation(velocity, wheelRadiusFront, wheelRadiusRear, dTime);
            GetBaseTrafficObject().SetFrontAxleSteeringYaw(_steeringWheelAngle * steeringToWheelYawRatio);
        } catch (const std::out_of_range& e)
        {
            Log(CbkLogLevel::Error, __FILE__, __LINE__, "Agent does not have SteeringRatio");
        }
    }

    void SetDistanceTraveled(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            distanceTraveled = value;
        });
    }

    void SetVehicleModelParameter(const VehicleModelParameters& parameter) override
    {
        world->QueueAgentUpdate([this, parameter]()
        {
            UpdateVehicleModelParameter(parameter);
        });
    }

    void SetMaxAcceleration(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            maxAcceleration = value;
        });
    }

    void SetEngineSpeed(double value) override
    {
        world->QueueAgentUpdate([this, value]()
        {
            engineSpeed = value;
        });
    }

    void SetMaxDeceleration(double maxDeceleration) override
    {
        world->QueueAgentUpdate([this, maxDeceleration]()
        {
            this->maxDeceleration = maxDeceleration;
        });
    }

    void SetGear(int gear) override
    {
        world->QueueAgentUpdate([this, gear]()
        {
            currentGear = gear;
        });
    }

    void SetEffAccelPedal(double percent) override
    {
        world->QueueAgentUpdate([this, percent]()
        {
            accelPedal = percent;
        });
    }

    void SetEffBrakePedal(double percent) override
    {
        world->QueueAgentUpdate([this, percent]()
        {
            brakePedal = percent;
        });
    }

    void SetSteeringWheelAngle(double steeringWheelAngle) override
    {
        world->QueueAgentUpdate([this, steeringWheelAngle]()
        {
            this->steeringWheelAngle = steeringWheelAngle;
            GetBaseTrafficObject().SetSteeringWheelAngle(steeringWheelAngle);
        });
    }

    void SetHeadLight(bool headLight) override
    {
        world->QueueAgentUpdate([this, headLight]()
        {
            GetBaseTrafficObject().SetHeadLight(headLight);
        });
    }

    void SetHighBeamLight(bool highBeam) override
    {
        world->QueueAgentUpdate([this, highBeam]()
        {
            GetBaseTrafficObject().SetHighBeamLight(highBeam);
        });
    }

    void SetHorn(bool horn) override
    {
        world->QueueAgentUpdate([this, horn]()
        {
            hornSwitch = horn;
        });
    }

    void SetFlasher(bool flasher) override
    {
        world->QueueAgentUpdate([this, flasher]()
        {
            flasherSwitch = flasher;
        });
    }

    double GetYawRate() const override
    {
        return GetBaseTrafficObject().GetAbsOrientationRate().yawRate;
    }

    double GetYawAcceleration() const override
    {
        return GetBaseTrafficObject().GetAbsOrientationAcceleration().yawAcceleration;
    }

    double GetCentripetalAcceleration() const override
    {
        return centripetalAcceleration;

        // Code seems to work incorrectly here, further investigation needed
        // OWL::Primitive::AbsAcceleration absAcceleration = GetBaseTrafficObject().GetAbsAcceleration();
        // OWL::Primitive::AbsOrientation absOrientation = GetBaseTrafficObject().GetAbsOrientation();

        // Common::Vector2d vec(absAcceleration.ax, absAcceleration.ay);

        // // Rotate reference frame from groundTruth to car
        // vec.Rotate(-absOrientation.yaw);

        // return vec.y;
    }

    double GetTangentialAcceleration() const override
    {
        return tangentialAcceleration;

        // Code seems to work incorrectly here, further investigation needed
        // OWL::Primitive::AbsAcceleration absAcceleration = GetBaseTrafficObject().GetAbsAcceleration();
        // OWL::Primitive::AbsOrientation absOrientation = GetBaseTrafficObject().GetAbsOrientation();

        // Common::Vector2d vec(absAcceleration.ax, absAcceleration.ay);

        // // Rotate reference frame from groundTruth to car
        // vec.Rotate(-absOrientation.yaw);

        // return vec.x;
    }

    bool Locate() override;

    void Unlocate() override;

    bool Update() override;

    void SetBrakeLight(bool brakeLightStatus) override;

    bool GetBrakeLight() const override;

    AgentCategory GetAgentCategory() const override;

    std::string GetAgentTypeName() const override;

    void SetIndicatorState(IndicatorState indicatorState) override;

    IndicatorState GetIndicatorState() const override;

    bool IsLeavingWorld() const override;

    bool IsAgentInWorld() const override;

    void SetPosition(Position pos) override;

    double  GetDistanceTraveled() const override
    {
        return distanceTraveled;
    }

    bool IsEgoAgent() const override;

    bool OnRoad(const OWL::Interfaces::Road& road) const;
    bool OnLane(const OWL::Interfaces::Lane& lane) const;

    Common::Vector2d GetVelocity(ObjectPoint point = ObjectPointPredefined::Reference) const override;

    Common::Vector2d GetAcceleration(ObjectPoint point = ObjectPointPredefined::Reference) const override;

    virtual const openpass::sensors::Parameters& GetSensorParameters() const override

    {
        return sensorParameters;
    }

    virtual void SetSensorParameters(openpass::sensors::Parameters sensorParameters) override
    {
        this->sensorParameters = sensorParameters;
    }

    virtual double GetDistanceToConnectorEntrance(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override;

    virtual double GetDistanceToConnectorDeparture(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const override;

protected:
    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char* file,
             int line,
             const std::string& message) const
    {
        if (callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:
    WorldInterface* world;
    const CallbackInterface* callbacks;
    const World::Localization::Localizer& localizer;
    EgoAgent egoAgent;

    OWL::Interfaces::MovingObject& GetBaseTrafficObject()
    {
        return *(static_cast<OWL::Interfaces::MovingObject*>(&baseTrafficObject));
    }

    OWL::Interfaces::MovingObject& GetBaseTrafficObject() const
    {
        return *(static_cast<OWL::Interfaces::MovingObject*>(&baseTrafficObject));
    }

    void UpdateVehicleModelParameter(const VehicleModelParameters& parameters)
    {
        OWL::Primitive::Dimension dimension;
        dimension.width = parameters.boundingBoxDimensions.width;
        dimension.length = parameters.boundingBoxDimensions.length;
        dimension.height = parameters.boundingBoxDimensions.height;

        GetBaseTrafficObject().SetDimension(dimension);
        GetBaseTrafficObject().SetBoundingBoxCenterToRear(parameters.rearAxle.positionX - parameters.boundingBoxCenter.x, 0.0f, parameters.rearAxle.positionZ - parameters.boundingBoxCenter.z);
        GetBaseTrafficObject().SetBoundingBoxCenterToFront(parameters.frontAxle.positionX - parameters.boundingBoxCenter.x, 0.0f, parameters.frontAxle.positionZ - parameters.boundingBoxCenter.z);
        GetBaseTrafficObject().SetType(parameters.vehicleType);

        vehicleModelParameters = parameters;

        GenerateWheels(parameters);
    }

    void GenerateWheels(const VehicleModelParameters& parameters)
    {
        if(parameters.vehicleType == AgentVehicleType::Pedestrian)
        {
            return;
        }


        std::vector<VehicleModelParameters::Axle> axles {parameters.frontAxle, parameters.rearAxle};
        for(int axle = 0; axle < 2; axle++)
        {
            OWL::WheelData newWheel;
            newWheel.axle = axle;
            newWheel.wheelRadius = axles[axle].wheelDiameter / 2.0;
            newWheel.rim_radius = std::numeric_limits<double>::signaling_NaN();
            newWheel.width = std::numeric_limits<double>::signaling_NaN();
            newWheel.rotation_rate = std::numeric_limits<double>::signaling_NaN();

            //Assuming that the center of the wheels are located at the axles
            newWheel.position.x = axle == 0 ?
                                  (parameters.frontAxle.positionX - parameters.boundingBoxCenter.x) :
                                  (parameters.rearAxle.positionX - parameters.boundingBoxCenter.x);

            newWheel.position.z = axle == 0 ?
                                  (parameters.frontAxle.positionZ - parameters.boundingBoxCenter.z) :
                                  (parameters.rearAxle.positionZ - parameters.boundingBoxCenter.z);

            if(parameters.vehicleType == AgentVehicleType::Car || parameters.vehicleType == AgentVehicleType::Truck)
            {
                newWheel.position.y = - parameters.frontAxle.trackWidth / 2.0f - parameters.boundingBoxCenter.y; //assume that the wheels are at the end of the axle
                newWheel.index = 0;
                GetBaseTrafficObject().AddWheel(newWheel);
                newWheel.position.y = parameters.frontAxle.trackWidth / 2.0f - parameters.boundingBoxCenter.y; //assume that the wheels are at the end of the axle
                newWheel.index = 1;
                GetBaseTrafficObject().AddWheel(newWheel);
            }
            else if (parameters.vehicleType == AgentVehicleType::Bicycle || parameters.vehicleType == AgentVehicleType::Motorbike)
            {
                //assuming that the positionY is in the middle of the two wheel vehicle
                newWheel.position.y = 0.0;
                newWheel.index = 0;
                GetBaseTrafficObject().AddWheel(newWheel);
            }
        }
    }

    void UpdateYaw(double yawAngle)
    {
        OWL::Primitive::AbsOrientation orientation = baseTrafficObject.GetAbsOrientation();
        orientation.yaw = yawAngle;
        GetBaseTrafficObject().SetAbsOrientation(orientation);
    }

    void UpdateRoll(double rollAngle)
    {
        OWL::Primitive::AbsOrientation orientation = baseTrafficObject.GetAbsOrientation();
        orientation.roll = rollAngle;
        GetBaseTrafficObject().SetAbsOrientation(orientation);
    }

    //-----------------------------------------------------------------------------
    //! Initialize the ego vehicle object inside the drivingView.
    //-----------------------------------------------------------------------------
    void InitEgoVehicle();

    //-----------------------------------------------------------------------------
    //! Update the ego vehicle object inside the drivingView.
    //-----------------------------------------------------------------------------
    void UpdateEgoVehicle();

    //! Returns the longitudinal position of the ObjectPoint (must not be of type ObjectPointRelative)
    double GetLongitudinal(const ObjectPoint &objectPoint) const;

    //! Returns the lateral position of the ObjectPoint (must not be of type ObjectPointRelative)
    double GetLateral(const ObjectPoint &objectPoint) const;

    struct LaneObjParameters
    {
        double distance;
        double relAngle;
        double latPosition;
        Common::Vector2d upperLeftCoord;
        Common::Vector2d upperRightCoord;
        Common::Vector2d lowerLeftCoord;
        Common::Vector2d lowerRightCoord;
    };

    bool hornSwitch = false;
    bool flasherSwitch = false;
    int currentGear = 0;
    double maxAcceleration = 0.0;
    double maxDeceleration = 0.0;
    double accelPedal = 0.;
    double brakePedal = 0.;
    double steeringWheelAngle = 0.0;
    double centripetalAcceleration = 0.0;
    double tangentialAcceleration = 0.0;
    double engineSpeed = 0.;
    double distanceTraveled = 0.0;
    double previousVelocity = 0.0;

    mutable World::Localization::Result locateResult;
    mutable std::vector<GlobalRoadPosition> boundaryPoints;

    std::vector<std::pair<ObjectTypeOSI, int>> collisionPartners;
    PostCrashVelocity postCrashVelocity {};
    bool isValid = true;

    AgentCategory agentCategory;
    std::string agentTypeName;
    std::string vehicleModelType;
    std::string driverProfileName;
    std::string objectName;
    VehicleModelParameters vehicleModelParameters;

    double speedGoalMin;

    bool completlyInWorld = false;

    openpass::sensors::Parameters sensorParameters;
};
