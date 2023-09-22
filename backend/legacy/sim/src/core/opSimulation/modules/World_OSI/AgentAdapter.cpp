/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2020 HLRS, University of Stuttgart
 *               2016 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <cassert>
#include <algorithm>
#include "AgentAdapter.h"
#include "common/globalDefinitions.h"

namespace loc = World::Localization;

AgentAdapter::AgentAdapter(OWL::Interfaces::MovingObject& mo,
                           WorldInterface* world,
                           const CallbackInterface* callbacks,
                           const World::Localization::Localizer& localizer) :
    WorldObjectAdapter{mo},
    world{world},
    callbacks{callbacks},
    localizer{localizer},
    egoAgent{this, world}
{
}

AgentAdapter::~AgentAdapter()
{
}

void AgentAdapter::InitParameter(const AgentBlueprintInterface& agentBlueprint)
{
    UpdateVehicleModelParameter(agentBlueprint.GetVehicleModelParameters());

    const auto & vehicleType = this->vehicleModelParameters.vehicleType;
    if (vehicleType != AgentVehicleType::Car &&
            vehicleType != AgentVehicleType::Truck &&
            vehicleType != AgentVehicleType::Motorbike &&
            vehicleType != AgentVehicleType::Bicycle &&
            vehicleType != AgentVehicleType::Pedestrian)
    {
        LOG(CbkLogLevel::Error, "undefined traffic object type");
        throw std::runtime_error("undefined traffic object type");
    }

    this->vehicleModelType = agentBlueprint.GetVehicleModelName();
    this->driverProfileName = agentBlueprint.GetDriverProfileName();
    this->agentCategory = agentBlueprint.GetAgentCategory();
    this->agentTypeName = agentBlueprint.GetAgentProfileName();
    this->objectName = agentBlueprint.GetObjectName();
    this->speedGoalMin = agentBlueprint.GetSpeedGoalMin();

    // set default values
    GetBaseTrafficObject().SetZ(0.0);
    GetBaseTrafficObject().SetPitch(0.0);
    GetBaseTrafficObject().SetRoll(0.0);
    GetBaseTrafficObject().SetAbsOrientationRate({0,0,0});
    GetBaseTrafficObject().SetAbsOrientationAcceleration({0,0,0});

    const auto& spawnParameter = agentBlueprint.GetSpawnParameter();
    UpdateYaw(spawnParameter.yawAngle);
    GetBaseTrafficObject().SetX(spawnParameter.positionX);
    GetBaseTrafficObject().SetY(spawnParameter.positionY);
    GetBaseTrafficObject().SetAbsVelocity(spawnParameter.velocity);
    GetBaseTrafficObject().SetAbsAcceleration(spawnParameter.acceleration);
    this->currentGear = static_cast<int>(spawnParameter.gear);

    SetSensorParameters(agentBlueprint.GetSensorParameters());

    // spawn tasks are executed before any other task types within current scheduling time
    // other task types will have a consistent view of the world
    // calculate initial position
    Locate();

    auto& route = spawnParameter.route;
    egoAgent.SetRoadGraph(std::move(route.roadGraph), route.root, route.target);
}

bool AgentAdapter::Update()
{
    // currently set to constant true to correctly update BB of rotating objects (with velocity = 0)
    // and objects with an incomplete set of dynamic parameters (i. e. changing x/y with velocity = 0)
    //boundingBoxNeedsUpdate = std::abs(GetVelocity()) >= zeroBaseline;
    boundingBoxNeedsUpdate = true;
    if(!Locate())
    {
        return false;
    }
    return true;
}

void AgentAdapter::SetBrakeLight(bool brakeLightStatus)
{
    GetBaseTrafficObject().SetBrakeLightState(brakeLightStatus);
}

bool AgentAdapter::GetBrakeLight() const
{
    return GetBaseTrafficObject().GetBrakeLightState();
}

bool AgentAdapter::Locate()
{
    // reset on-demand values
    boundaryPoints.clear();

    locateResult = localizer.Locate(GetBoundingBox2D(), GetBaseTrafficObject());

    GetBaseTrafficObject().SetTouchedRoads(locateResult.touchedRoads);

    egoAgent.Update();

    return locateResult.isOnRoute;
}

void AgentAdapter::Unlocate()
{
    localizer.Unlocate(GetBaseTrafficObject());
}

AgentCategory AgentAdapter::GetAgentCategory() const
{
    return agentCategory;
}

std::string AgentAdapter::GetAgentTypeName() const
{
    return agentTypeName;
}

void AgentAdapter::SetIndicatorState(IndicatorState indicatorState)
{
    GetBaseTrafficObject().SetIndicatorState(indicatorState);
}

IndicatorState AgentAdapter::GetIndicatorState() const
{
    return GetBaseTrafficObject().GetIndicatorState();
}

bool AgentAdapter::IsAgentInWorld() const
{
    return locateResult.isOnRoute;
}

void AgentAdapter::SetPosition(Position pos)
{
    SetPositionX(pos.xPos);
    SetPositionY(pos.yPos);
    SetYaw(pos.yawAngle);
}

Common::Vector2d AgentAdapter::GetVelocity(ObjectPoint point) const
{
    double longitudinal = GetLongitudinal(point);
    double lateral = GetLateral(point);
    Common::Vector2d rotation{0.0, 0.0}; //! velocity from rotation of vehicle around reference point
    if (longitudinal != 0. || lateral != 0.)
    {
        rotation.x = -lateral * GetYawRate();
        rotation.y = longitudinal * GetYawRate();
        rotation.Rotate(-GetYaw());
    }
    return {GetBaseTrafficObject().GetAbsVelocity().vx + rotation.x, GetBaseTrafficObject().GetAbsVelocity().vy + rotation.y};
}
Common::Vector2d AgentAdapter::GetAcceleration(ObjectPoint point) const
{
    double longitudinal = GetLongitudinal(point);
    double lateral = GetLateral(point);
    Common::Vector2d rotationAcceleration{0.0, 0.0}; //! acceleration from rotation of vehicle around reference point
    if (longitudinal != 0. || lateral != 0.)
    {
        rotationAcceleration.x = -lateral * GetYawAcceleration();
        rotationAcceleration.y = longitudinal * GetYawAcceleration();
        rotationAcceleration.Rotate(-GetYaw());
    }
    return {GetBaseTrafficObject().GetAbsAcceleration().ax + rotationAcceleration.x, GetBaseTrafficObject().GetAbsAcceleration().ay + rotationAcceleration.y};
}

bool AgentAdapter::IsEgoAgent() const
{
    return agentCategory == AgentCategory::Ego;
}

void AgentAdapter::UpdateCollision(std::pair<ObjectTypeOSI, int> collisionPartner)
{
    auto findIter = std::find_if(collisionPartners.begin(), collisionPartners.end(),
                                 [collisionPartner](const std::pair<ObjectTypeOSI, int>& storedCollisionPartner)
    {
        return collisionPartner == storedCollisionPartner;
    });
    if (findIter == collisionPartners.end())
    {
        collisionPartners.push_back(collisionPartner);
    }
}

bool AgentAdapter::IsLeavingWorld() const
{
    return false;
}

bool AgentAdapter::GetHeadLight() const
{
    return GetBaseTrafficObject().GetHeadLight();
}

bool AgentAdapter::GetHighBeamLight() const
{
    return GetBaseTrafficObject().GetHighBeamLight();
}

std::vector<std::string> AgentAdapter::GetRoads(ObjectPoint point) const
{
    std::vector<std::string> roadIds;
    std::transform(GetRoadPosition(point).cbegin(), GetRoadPosition(point).cend(), std::back_inserter(roadIds), [](const auto& entry){return entry.first;});
    return roadIds;
}

double AgentAdapter::GetDistanceToConnectorEntrance(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return world->GetDistanceToConnectorEntrance(/*locateResult.position,*/ intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

double AgentAdapter::GetDistanceToConnectorDeparture(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return world->GetDistanceToConnectorDeparture(/*locateResult.position,*/ intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

LightState AgentAdapter::GetLightState() const
{

    if (GetFlasher())
    {
        return LightState::Flash;
    }
    if (GetHighBeamLight())
    {
        return LightState::HighBeam;
    }
    if (GetHeadLight())
    {
        return LightState::LowBeam;
    }

    return LightState::Off;
}

const GlobalRoadPositions& AgentAdapter::GetRoadPosition(const ObjectPoint& point) const
{
    auto position = locateResult.points.find(point);
    if (position != locateResult.points.cend())
    {
        return position->second;
    }
    Common::Vector2d globalPoint = GetAbsolutePosition(point);
    auto locatedPoint = localizer.Locate(globalPoint, GetYaw());
    auto [newElement, success] = locateResult.points.insert({point, locatedPoint});
    return newElement->second;
}

double AgentAdapter::GetLongitudinal(const ObjectPoint &objectPoint) const
{
    if (std::holds_alternative<ObjectPointCustom>(objectPoint))
    {
        return std::get<ObjectPointCustom>(objectPoint).longitudinal;
    }
    else if (std::holds_alternative<ObjectPointPredefined>(objectPoint))
    {
        switch (std::get<ObjectPointPredefined>(objectPoint))
        {
        case ObjectPointPredefined::Reference:
            return 0;
        case ObjectPointPredefined::Center:
            return GetDistanceReferencePointToLeadingEdge() - 0.5 * GetLength();
        case ObjectPointPredefined::FrontCenter:
        case ObjectPointPredefined::FrontLeft:
        case ObjectPointPredefined::FrontRight:
            return GetDistanceReferencePointToLeadingEdge();
        case ObjectPointPredefined::RearCenter:
        case ObjectPointPredefined::RearLeft:
        case ObjectPointPredefined::RearRight:
            return GetDistanceReferencePointToLeadingEdge() - GetLength();
        }
        throw std::runtime_error("Invalid value for ObjectPointPredefined");
    }
    else if (std::holds_alternative<ObjectPointRelative>(objectPoint))
    {
        throw std::invalid_argument("Relative ObjectPoint can not be resolved without reference");
    }
    else
    {
        throw std::runtime_error("Unknown type of ObjectPoint");
    }
}

double AgentAdapter::GetLateral(const ObjectPoint &objectPoint) const
{
    if (std::holds_alternative<ObjectPointCustom>(objectPoint))
    {
        return std::get<ObjectPointCustom>(objectPoint).lateral;
    }
    else if (std::holds_alternative<ObjectPointPredefined>(objectPoint))
    {
        switch (std::get<ObjectPointPredefined>(objectPoint))
        {
        case ObjectPointPredefined::Reference:
        case ObjectPointPredefined::Center:
        case ObjectPointPredefined::FrontCenter:
        case ObjectPointPredefined::RearCenter:
            return 0;
        case ObjectPointPredefined::FrontLeft:
        case ObjectPointPredefined::RearLeft:
            return 0.5 * GetWidth();
        case ObjectPointPredefined::FrontRight:
        case ObjectPointPredefined::RearRight:
            return -0.5 * GetWidth();
        }
        throw std::runtime_error("Invalid value for ObjectPointPredefined");
    }
    else if (std::holds_alternative<ObjectPointRelative>(objectPoint))
    {
        throw std::invalid_argument("Relative ObjectPoint can not be resolved without reference");
    }
    else
    {
        throw std::runtime_error("Unknown type of ObjectPoint");
    }
}

Common::Vector2d AgentAdapter::GetAbsolutePosition(const ObjectPoint &objectPoint) const
{
    double longitudinal = GetLongitudinal(objectPoint);
    double lateral = GetLateral(objectPoint);
    const auto& referencePoint = baseTrafficObject.GetReferencePointPosition();
    const auto& yaw = baseTrafficObject.GetAbsOrientation().yaw;
    double x = referencePoint.x + std::cos(yaw) * longitudinal - std::sin(yaw) * lateral;
    double y = referencePoint.y + std::sin(yaw) * longitudinal + std::cos(yaw) * lateral;
    return {x,y};
}
