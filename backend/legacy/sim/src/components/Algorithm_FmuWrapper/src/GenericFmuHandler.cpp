/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <memory>
#include <qglobal.h>
#include <string>
#include <sstream>
#include <cstdint>
#include "include/agentInterface.h"
#include "include/egoAgentInterface.h"
#include "GenericFmuHandler.h"
#include "common/commonTools.h"
#include "include/fmuHandlerInterface.h"
#include "common/accelerationSignal.h"
#include "common/longitudinalSignal.h"
#include "common/steeringSignal.h"
#include "common/dynamicsSignal.h"
#include "common/sensorDataSignal.h"
#include "core/opSimulation/modules/World_OSI/WorldData.h"

extern "C" {
#include "fmilib.h"
#include "fmuChecker.h"
}

GenericFmuHandler::GenericFmuHandler(const FmuVariables& fmuVariables,
                                     std::map<ValueReferenceAndType, FmuValue>& fmuVariableValues,
                                     const ParameterInterface* parameters,
                                     struct fmu_check_data_t* cdata,
                                     AgentInterface* agent,
                                     const CallbackInterface* callbacks,
                                     WorldInterface* world) :
    FmuHandlerInterface(cdata, agent, callbacks),
    fmiVersion{cdata->version},
    fmuVariableValues(fmuVariableValues),
    world(world)
{
    const auto sensorList = parameters->GetParameterLists().find("SensorLinks");
    if (sensorList != parameters->GetParameterLists().cend())
    {
        for (const auto& sensorLink : sensorList->second)
        {
            if (sensorLink->GetParametersString().at("InputId") == "Camera")
            {
                sensors.push_back(sensorLink->GetParametersInt().at("SensorId"));
            }
        }
    }

    ParseChannelDefinitions(parameters, fmuVariables);
}

GenericFmuHandler::~GenericFmuHandler()
{
}

void GenericFmuHandler::Init()
{
    SetFmuParameters();
}

void GenericFmuHandler::UpdateInput(int localLinkId,
                                    const std::shared_ptr<SignalInterface const> &data,
                                    [[maybe_unused]] int time)
{
    if (localLinkId == 2)
    {
        const std::shared_ptr<SensorDataSignal const> signal =
            std::dynamic_pointer_cast<SensorDataSignal const>(data);
        if (!signal)
        {
            const std::string msg = "AlgorithmFmuHandler invalid signaltype";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }

        sensorData = signal->sensorData;
    }
}

void GenericFmuHandler::UpdateOutput(int localLinkId, std::shared_ptr<const SignalInterface> &data, int time)
{
    if (fmiVersion == fmi_version_enu_t::fmi_version_1_enu)
    {
        UpdateOutput<FMI1>(localLinkId, data, time);
    }
    else if (fmiVersion == fmi_version_enu_t::fmi_version_2_0_enu)
    {
        UpdateOutput<FMI2>(localLinkId, data, time);
    }
}

template <size_t I>
void GenericFmuHandler::UpdateOutput(int localLinkId, std::shared_ptr<const SignalInterface> &data, int time)
{
    Q_UNUSED(time)

    LOG(CbkLogLevel::Debug, "UpdateOutput started");

    try
    {
        auto componentStateValueReference = std::get<I>(fmuOutputs).find(SignalValue::ComponentState);
        ComponentState componentState = ComponentState::Acting;

        if (componentStateValueReference != std::get<I>(fmuOutputs).end())
        {
            if (cdata->version == fmi_version_1_enu)
            {
                componentState = static_cast<ComponentState>(GetValue<I>(SignalValue::ComponentState, VariableType::Enum).intValue - 1);
            }
            else
            {
                componentState = static_cast<ComponentState>(GetValue<I>(SignalValue::ComponentState, VariableType::Enum).intValue);
            }
        }

        if (localLinkId == 0)
        {
            if (std::find(outputSignals.cbegin(), outputSignals.cend(), SignalType::DynamicsSignal) != outputSignals.cend())
            {
                auto acceleration = GetValue<I>(SignalValue::DynamicsSignal_Acceleration, VariableType::Double).realValue;
                auto velocity = GetValue<I>(SignalValue::DynamicsSignal_Velocity, VariableType::Double).realValue;
                auto positionX = GetValue<I>(SignalValue::DynamicsSignal_PositionX, VariableType::Double).realValue;
                auto positionY = GetValue<I>(SignalValue::DynamicsSignal_PositionY, VariableType::Double).realValue;
                auto yaw = GetValue<I>(SignalValue::DynamicsSignal_Yaw, VariableType::Double).realValue;
                auto yawRate = GetValue<I>(SignalValue::DynamicsSignal_YawRate, VariableType::Double).realValue;
                auto yawAcceleration = GetValue<I>(SignalValue::DynamicsSignal_YawAcceleration, VariableType::Double).realValue;
                auto steeringWheelAngle = GetValue<I>(SignalValue::DynamicsSignal_SteeringWheelAngle, VariableType::Double).realValue;
                auto centripetalAcceleration = GetValue<I>(SignalValue::DynamicsSignal_CentripetalAcceleration, VariableType::Double).realValue;
                auto travelDistance = GetValue<I>(SignalValue::DynamicsSignal_TravelDistance, VariableType::Double).realValue;
                data = std::make_shared<DynamicsSignal const>(componentState,
                                                              acceleration,
                                                              velocity,
                                                              positionX,
                                                              positionY,
                                                              yaw,
                                                              yawRate,
                                                              yawAcceleration,
                                                              0.0,
                                                              steeringWheelAngle,
                                                              centripetalAcceleration,
                                                              travelDistance);
            }
            else
            {
                data = std::make_shared<DynamicsSignal const>(ComponentState::Disabled, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
            }
        }
        else if (localLinkId == 1)
        {
            if (std::find(outputSignals.cbegin(), outputSignals.cend(), SignalType::AccelerationSignal) != outputSignals.cend())
            {
                auto acceleration = GetValue<I>(SignalValue::AccelerationSignal_Acceleration, VariableType::Double).realValue;
                data = std::make_shared<AccelerationSignal const>(componentState, acceleration);
            }
            else
            {
                data = std::make_shared<AccelerationSignal const>(ComponentState::Disabled, 0.0);
            }
        }
        else if (localLinkId == 2)
        {
            if (std::find(outputSignals.cbegin(), outputSignals.cend(), SignalType::LongitudinalSignal) != outputSignals.cend())
            {
                auto accPedalPos = GetValue<I>(SignalValue::LongitudinalSignal_AccPedalPos, VariableType::Double).realValue;
                auto brakePedalPos = GetValue<I>(SignalValue::LongitudinalSignal_BrakePedalPos, VariableType::Double).realValue;
                auto gear = GetValue<I>(SignalValue::LongitudinalSignal_Gear, VariableType::Int).intValue;
                data = std::make_shared<LongitudinalSignal const>(componentState, accPedalPos, brakePedalPos, gear);
            }
            else
            {
                data = std::make_shared<LongitudinalSignal const>(ComponentState::Disabled, 0.0, 0.0, 0.0);
            }
        }
        else if (localLinkId == 3)
        {
            if (std::find(outputSignals.cbegin(), outputSignals.cend(), SignalType::SteeringSignal) != outputSignals.cend())
            {
                auto steeringWheelAngle = GetValue<I>(SignalValue::SteeringSignal_SteeringWheelAngle, VariableType::Double).realValue;
                data = std::make_shared<SteeringSignal const>(componentState, steeringWheelAngle);
            }
            else
            {
                data = std::make_shared<SteeringSignal const>(ComponentState::Disabled, 0.0);
            }
        }

    }
    catch (const std::bad_alloc& e)
    {
        throw std::runtime_error("Could not instantiate output signal: " + std::string(e.what()));
    }

    LOG(CbkLogLevel::Debug, "UpdateOutput finished");
}

void GenericFmuHandler::PreStep([[maybe_unused]]int time)
{
    LOG(CbkLogLevel::Debug, "Start of PreStep");

    const auto& objectsInRange = agent->GetEgoAgent().GetObjectsInRange(0, std::numeric_limits<double>::max(), 0);
    const auto* frontObject = objectsInRange.empty() ? nullptr : objectsInRange.at(0);
    const auto* frontfrontObject = (objectsInRange.size() < 2) ? nullptr : objectsInRange.at(1);
    auto sensorFusionInfo = CalculateSensorFusionInfo();

    if (fmiVersion == fmi_version_enu_t::fmi_version_1_enu)
    {
        fmi1_value_reference_t realvrs[150];
        fmi1_real_t realData[150];
        size_t i = 0;
        for (const auto& fmuInput : std::get<FMI1>(fmuRealInputs))
        {
            realvrs[i] = fmuInput.valueReference;
            switch (fmuInput.type)
            {
                case FmuInputType::VelocityEgo:
                    realData[i] = agent->GetVelocity().Length();
                    break;
                case FmuInputType::AccelerationEgo:
                    realData[i] = agent->GetAcceleration().Projection(agent->GetYaw());
                    break;
                case FmuInputType::CentripetalAccelerationEgo:
                    realData[i] = agent->GetCentripetalAcceleration();
                    break;
                case FmuInputType::SteeringWheelEgo:
                    realData[i] = agent->GetSteeringWheelAngle();
                    break;
                case FmuInputType::AccelerationPedalPositionEgo:
                    realData[i] = agent->GetEffAccelPedal();
                    break;
                case FmuInputType::BrakePedalPositionEgo:
                    realData[i] = agent->GetEffBrakePedal();
                    break;
                case FmuInputType::DistanceRefToFrontEdgeEgo:
                    realData[i] = agent->GetDistanceReferencePointToLeadingEdge();
                    break;
                case FmuInputType::PositionXEgo:
                    realData[i] = agent->GetPositionX();
                    break;
                case FmuInputType::PositionYEgo:
                    realData[i] = agent->GetPositionY();
                    break;
                case FmuInputType::YawEgo:
                    realData[i] = agent->GetYaw();
                    break;
                case FmuInputType::PositionSEgo:
                    realData[i] = agent->GetEgoAgent().GetReferencePointPosition().value_or(GlobalRoadPosition{}).roadPosition.s;
                    break;
                case FmuInputType::PositionTEgo:
                    realData[i] = agent->GetEgoAgent().GetReferencePointPosition().value_or(GlobalRoadPosition{}).roadPosition.t;
                    break;
                case FmuInputType::PositionXFront:
                    realData[i] = frontObject ? frontObject->GetPositionX() : 0.0;
                    break;
                case FmuInputType::PositionYFront:
                    realData[i] = frontObject ? frontObject->GetPositionY() : 0.0;
                    break;
                case FmuInputType::YawFront:
                    realData[i] = frontObject ? frontObject->GetYaw() : 0.0;
                    break;
                case FmuInputType::PositionSFront:
                    realData[i] = frontObject ? frontObject->GetRoadPosition(ObjectPointPredefined::Reference).at(agent->GetEgoAgent().GetRoadId()).roadPosition.s : 0.0;
                    break;
                case FmuInputType::PositionTFront:
                    realData[i] = frontObject ? frontObject->GetRoadPosition(ObjectPointPredefined::Reference).at(agent->GetEgoAgent().GetRoadId()).roadPosition.t : 0.0;
                    break;
                case FmuInputType::RelativeDistanceFront:
                    realData[i] = frontObject ? agent->GetEgoAgent().GetNetDistance(frontObject).value() : 0.0;
                    break;
                case FmuInputType::WidthFront:
                    realData[i] = frontObject ? frontObject->GetWidth() : 0.0;
                    break;
                case FmuInputType::LengthFront:
                    realData[i] = frontObject ? frontObject->GetLength() : 0.0;
                    break;
                case FmuInputType::DistanceRefToFrontEdgeFront:
                    realData[i] = frontObject ? frontObject->GetDistanceReferencePointToLeadingEdge() : 0.0;
                    break;
                case FmuInputType::VelocityFront:
                    realData[i] = frontObject ? frontObject->GetVelocity().Length() : 0.0;
                    break;
                case FmuInputType::PositionXFrontFront:
                    realData[i] = frontfrontObject ? frontfrontObject->GetPositionX() : 0.0;
                    break;
                case FmuInputType::PositionYFrontFront:
                    realData[i] = frontfrontObject ? frontfrontObject->GetPositionY() : 0.0;
                    break;
                case FmuInputType::VelocityFrontFront:
                    realData[i] = frontfrontObject ? frontfrontObject->GetVelocity().Length() : 0.0;
                    break;
                case FmuInputType::RelativeDistanceFrontFront:
                    realData[i] = frontfrontObject ? agent->GetEgoAgent().GetNetDistance(frontfrontObject).value() : 0.0;
                    break;
                case FmuInputType::SpeedLimit:
                    realData[i] = CalculateSpeedLimit(std::get<double>(fmuInput.additionalParameter));
                    break;
                case FmuInputType::RoadCurvature:
                    realData[i] = agent->GetEgoAgent().GetLaneCurvature(std::get<double>(fmuInput.additionalParameter)).value_or(0.0);
                    break;
                case FmuInputType::SensorFusionRelativeS:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].s;
                    break;
                case FmuInputType::SensorFusionRelativeNetS:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].net_s;
                    break;
                case FmuInputType::SensorFusionRelativeT:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].t;
                    break;
                case FmuInputType::SensorFusionRelativeNetLeft:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].t_left;
                    break;
                case FmuInputType::SensorFusionRelativeNetRight:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].t_right;
                    break;
                case FmuInputType::SensorFusionRelativeNetX:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].net_x;
                    break;
                case FmuInputType::SensorFusionRelativeNetY:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].net_y;
                    break;
                case FmuInputType::SensorFusionVelocity:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].velocity;
                    break;
                case FmuInputType::SensorFusionVelocityX:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].velocity_x;
                    break;
                case FmuInputType::SensorFusionVelocityY:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].velocity_y;
                    break;
                case FmuInputType::SensorFusionYaw:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].yaw;
                    break;
                default:
                    throw std::runtime_error("Input can not be mapped to variable of type Real");
            }
            ++i;
        }
        fmi1_value_reference_t integervrs[40];
        fmi1_integer_t integerData[40];
        i = 0;
        for (const auto& fmuInput : std::get<FMI1>(fmuIntegerInputs))
        {
            integervrs[i] = fmuInput.valueReference;
            switch (fmuInput.type)
            {
                case FmuInputType::LaneEgo:
                    integerData[i] = agent->GetEgoAgent().HasValidRoute() ? agent->GetEgoAgent().GetMainLocatePosition().value().laneId : 0;
                    break;
                case FmuInputType::LaneFront:
                    integerData[i] = frontObject ? frontObject->GetRoadPosition(ObjectPointPredefined::Reference).at(agent->GetEgoAgent().GetRoadId()).laneId : 0.0;
                    break;
                case FmuInputType::LaneFrontFront:
                    integerData[i] = frontfrontObject ? frontfrontObject->GetRoadPosition(ObjectPointPredefined::Reference).at(agent->GetEgoAgent().GetRoadId()).laneId : 0.0;
                    break;
                case FmuInputType::LaneCountLeft:
                    integerData[i] = CalculateLaneCount(Side::Left);
                    break;
                case FmuInputType::LaneCountRight:
                    integerData[i] = CalculateLaneCount(Side::Right);
                    break;
                case FmuInputType::SensorFusionObjectId:
                    integerData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? -1 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].id;
                    break;
                case FmuInputType::SensorFusionNumberOfDetectingSensors:
                    integerData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? -1 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].numberOfSensors;
                    break;
                case FmuInputType::SensorFusionLane:
                    integerData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].lane;
                    break;
                default:
                    throw std::runtime_error("Input can not be mapped to variable of type Integer");
            }
            ++i;
        }

        fmi1_value_reference_t booleanvrs[2];
        fmi1_boolean_t booleanData[2];
        i = 0;
        for (const auto& fmuInput : std::get<FMI1>(fmuBooleanInputs))
        {
            booleanvrs[i] = fmuInput.valueReference;
            switch (fmuInput.type)
            {
                case FmuInputType::ExistenceFront:
                    booleanData[i] = static_cast<bool>(frontObject);
                    break;
                case FmuInputType::ExistenceFrontFront:
                    booleanData[i] = static_cast<bool>(frontfrontObject);
                    break;
                default:
                    throw std::runtime_error("Only ExistenceFront and ExistenceFrontFront can be mapped to variable of type Boolean");
            }
            ++i;
        }

        SetRealFMI1(realvrs, realData, std::get<FMI1>(fmuRealInputs).size());
        SetIntegerFMI1(integervrs, integerData, std::get<FMI1>(fmuIntegerInputs).size());
        SetBooleanFMI1(booleanvrs, booleanData, std::get<FMI1>(fmuBooleanInputs).size());
    }

    else if (fmiVersion == fmi_version_enu_t::fmi_version_2_0_enu)
    {
        fmi2_value_reference_t realvrs[150];
        fmi2_real_t realData[150];
        size_t i = 0;
        for (const auto& fmuInput : std::get<FMI2>(fmuRealInputs))
        {
            realvrs[i] = fmuInput.valueReference;
            switch (fmuInput.type)
            {
                case FmuInputType::VelocityEgo:
                    realData[i] = agent->GetVelocity().Length();
                    break;
                case FmuInputType::AccelerationEgo:
                    realData[i] = agent->GetAcceleration().Projection(agent->GetYaw());
                    break;
                case FmuInputType::CentripetalAccelerationEgo:
                    realData[i] = agent->GetCentripetalAcceleration();
                    break;
                case FmuInputType::SteeringWheelEgo:
                    realData[i] = agent->GetSteeringWheelAngle();
                    break;
                case FmuInputType::AccelerationPedalPositionEgo:
                    realData[i] = agent->GetEffAccelPedal();
                    break;
                case FmuInputType::BrakePedalPositionEgo:
                    realData[i] = agent->GetEffBrakePedal();
                    break;
                case FmuInputType::DistanceRefToFrontEdgeEgo:
                    realData[i] = agent->GetDistanceReferencePointToLeadingEdge();
                    break;
                case FmuInputType::PositionXEgo:
                    realData[i] = agent->GetPositionX();
                    break;
                case FmuInputType::PositionYEgo:
                    realData[i] = agent->GetPositionY();
                    break;
                case FmuInputType::YawEgo:
                    realData[i] = agent->GetYaw();
                    break;
                case FmuInputType::PositionSEgo:
                    realData[i] = agent->GetEgoAgent().GetReferencePointPosition().value_or(GlobalRoadPosition{}).roadPosition.s;
                    break;
                case FmuInputType::PositionTEgo:
                    realData[i] = agent->GetEgoAgent().GetReferencePointPosition().value_or(GlobalRoadPosition{}).roadPosition.t;
                    break;
                case FmuInputType::PositionXFront:
                    realData[i] = frontObject ? frontObject->GetPositionX() : 0.0;
                    break;
                case FmuInputType::PositionYFront:
                    realData[i] = frontObject ? frontObject->GetPositionY() : 0.0;
                    break;
                case FmuInputType::YawFront:
                    realData[i] = frontObject ? frontObject->GetYaw() : 0.0;
                    break;
                case FmuInputType::PositionSFront:
                    realData[i] = frontObject ? frontObject->GetRoadPosition(ObjectPointPredefined::Reference).at(agent->GetEgoAgent().GetRoadId()).roadPosition.s : 0.0;
                    break;
                case FmuInputType::PositionTFront:
                    realData[i] = frontObject ? frontObject->GetRoadPosition(ObjectPointPredefined::Reference).at(agent->GetEgoAgent().GetRoadId()).roadPosition.t : 0.0;
                    break;
                case FmuInputType::RelativeDistanceFront:
                    realData[i] = frontObject ? agent->GetEgoAgent().GetNetDistance(frontObject).value() : 0.0;
                    break;
                case FmuInputType::WidthFront:
                    realData[i] = frontObject ? frontObject->GetWidth() : 0.0;
                    break;
                case FmuInputType::LengthFront:
                    realData[i] = frontObject ? frontObject->GetLength() : 0.0;
                    break;
                case FmuInputType::DistanceRefToFrontEdgeFront:
                    realData[i] = frontObject ? frontObject->GetDistanceReferencePointToLeadingEdge() : 0.0;
                    break;
                case FmuInputType::VelocityFront:
                    realData[i] = frontObject ? frontObject->GetVelocity().Length() : 0.0;
                    break;
                case FmuInputType::PositionXFrontFront:
                    realData[i] = frontfrontObject ? frontfrontObject->GetPositionX() : 0.0;
                    break;
                case FmuInputType::PositionYFrontFront:
                    realData[i] = frontfrontObject ? frontfrontObject->GetPositionY() : 0.0;
                    break;
                case FmuInputType::VelocityFrontFront:
                    realData[i] = frontfrontObject ? frontfrontObject->GetVelocity().Length() : 0.0;
                    break;
                case FmuInputType::RelativeDistanceFrontFront:
                    realData[i] = frontfrontObject ? agent->GetEgoAgent().GetNetDistance(frontfrontObject).value() : 0.0;
                    break;
                case FmuInputType::SpeedLimit:
                    realData[i] = CalculateSpeedLimit(std::get<double>(fmuInput.additionalParameter));
                    break;
                case FmuInputType::RoadCurvature:
                    realData[i] = agent->GetEgoAgent().GetLaneCurvature(std::get<double>(fmuInput.additionalParameter)).value_or(0.0);
                    break;
                case FmuInputType::SensorFusionRelativeS:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].s;
                    break;
                case FmuInputType::SensorFusionRelativeNetS:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].net_s;
                    break;
                case FmuInputType::SensorFusionRelativeT:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].t;
                    break;
                case FmuInputType::SensorFusionRelativeNetLeft:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].t_left;
                    break;
                case FmuInputType::SensorFusionRelativeNetRight:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].t_right;
                    break;
                case FmuInputType::SensorFusionRelativeNetX:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].net_x;
                    break;
                case FmuInputType::SensorFusionRelativeNetY:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].net_y;
                    break;
                case FmuInputType::SensorFusionVelocity:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].velocity;
                    break;
                case FmuInputType::SensorFusionVelocityX:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].velocity_x;
                    break;
                case FmuInputType::SensorFusionVelocityY:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].velocity_y;
                    break;
                case FmuInputType::SensorFusionYaw:
                    realData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].yaw;
                    break;
                default:
                    throw std::runtime_error("Input can not be mapped to variable of type Real");
            }
            ++i;
        }
        fmi2_value_reference_t integervrs[40];
        fmi2_integer_t integerData[40];
        i = 0;
        for (const auto& fmuInput : std::get<FMI2>(fmuIntegerInputs))
        {
            integervrs[i] = fmuInput.valueReference;
            switch (fmuInput.type)
            {
                case FmuInputType::LaneEgo:
                    integerData[i] = agent->GetEgoAgent().HasValidRoute() ? agent->GetEgoAgent().GetMainLocatePosition().value().laneId : 0;
                    break;
                case FmuInputType::LaneFront:
                    integerData[i] = frontObject ? frontObject->GetRoadPosition(ObjectPointPredefined::Reference).at(agent->GetEgoAgent().GetRoadId()).laneId : 0.0;
                    break;
                case FmuInputType::LaneFrontFront:
                    integerData[i] = frontfrontObject ? frontfrontObject->GetRoadPosition(ObjectPointPredefined::Reference).at(agent->GetEgoAgent().GetRoadId()).laneId : 0.0;
                    break;
                case FmuInputType::LaneCountLeft:
                    integerData[i] = CalculateLaneCount(Side::Left);
                    break;
                case FmuInputType::LaneCountRight:
                    integerData[i] = CalculateLaneCount(Side::Right);
                    break;
                case FmuInputType::SensorFusionObjectId:
                    integerData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? -1 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].id;
                    break;
                case FmuInputType::SensorFusionNumberOfDetectingSensors:
                    integerData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? -1 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].numberOfSensors;
                    break;
                case FmuInputType::SensorFusionLane:
                    integerData[i] = sensorFusionInfo.size() <= std::get<size_t>(fmuInput.additionalParameter) ? 0 : sensorFusionInfo[std::get<size_t>(fmuInput.additionalParameter)].lane;
                    break;
                default:
                    throw std::runtime_error("Input can not be mapped to variable of type Integer");
            }
            ++i;
        }

        fmi2_value_reference_t booleanvrs[2];
        fmi2_boolean_t booleanData[2];
        i = 0;
        for (const auto& fmuInput : std::get<FMI2>(fmuBooleanInputs))
        {
            booleanvrs[i] = fmuInput.valueReference;
            switch (fmuInput.type)
            {
                case FmuInputType::ExistenceFront:
                    booleanData[i] = static_cast<bool>(frontObject);
                    break;
                case FmuInputType::ExistenceFrontFront:
                    booleanData[i] = static_cast<bool>(frontfrontObject);
                    break;
                default:
                    throw std::runtime_error("Only ExistenceFront and ExistenceFrontFront can be mapped to variable of type Boolean");
            }
            ++i;
        }

        SetRealFMI2(realvrs, realData, std::get<FMI2>(fmuRealInputs).size());
        SetIntegerFMI2(integervrs, integerData, std::get<FMI2>(fmuIntegerInputs).size());
        SetBooleanFMI2(booleanvrs, booleanData, std::get<FMI2>(fmuBooleanInputs).size());
    }

    LOG(CbkLogLevel::Debug, "End of PreStep");
}

void GenericFmuHandler::PostStep([[maybe_unused]]int time)
{
    LOG(CbkLogLevel::Debug, "Start of PostStep");
    LOG(CbkLogLevel::Debug, "End of PostStep");
}

template <size_t I>
FmuHandlerInterface::FmuValue &GenericFmuHandler::GetValue(SignalValue signalValue, VariableType variableType)
{
    ValueReferenceAndType valueReferenceAndType;
    if (I == 0)
    {
        fmi1_value_reference_t valueReference = std::get<FMI1>(fmuOutputs).at(signalValue);
        valueReferenceAndType.emplace<FMI1>(valueReference, variableType);
    }
    else if (I == 1)
    {
        fmi2_value_reference_t valueReference = std::get<FMI2>(fmuOutputs).at(signalValue);
        valueReferenceAndType.emplace<FMI2>(valueReference, variableType);
    }
    return fmuVariableValues.at(valueReferenceAndType);
}

void GenericFmuHandler::ParseChannelDefinitions(const ParameterInterface* parameters, const FmuVariables& fmuVariables)
{
    ChannelDefinitionParser channelDefinitionParser(fmuVariables, cdata->version);

    for (const auto& [paramKey, paramValue] : parameters->GetParametersString())
    {
        const auto pos = paramKey.find('_');
        const auto channelCausality = paramKey.substr(0, pos);
        const auto channelType = paramKey.substr(pos + 1);

        if (channelCausality == "Output")
        {
            if (!channelDefinitionParser.AddOutputChannel(paramValue, channelType))
            {
                const std::string msg = "Unable to parse output channel definition \"" + channelType + "\"";
                LOGERROR(msg);
                throw std::runtime_error(msg);
            }
        }
        else if (channelCausality == "Input")
        {
            if (!channelDefinitionParser.AddInputChannel(paramValue, channelType))
            {
                const std::string msg = "Unable to parse input channel definition \"" + channelType + "\"";
                LOGERROR(msg);
                throw std::runtime_error(msg);
            }
        }
        else if (channelCausality == "Parameter")
        {
            if (!channelDefinitionParser.AddParameter(paramValue, channelType))
            {
                const std::string msg = "Unable to parse parameter definition \"" + channelType + "\"";
                LOGERROR(msg);
                throw std::runtime_error(msg);
            }
        }
    }

    for (const auto& [paramKey, paramValue] : parameters->GetParametersInt())
    {
        const auto pos = paramKey.find('_');
        const auto channelCausality = paramKey.substr(0, pos);
        const auto channelType = paramKey.substr(pos + 1);

        if (channelCausality == "Parameter")
        {
            if (!channelDefinitionParser.AddParameter(paramValue, channelType))
            {
                const std::string msg = "Unable to parse parameter definition \"" + channelType + "\"";
                LOGERROR(msg);
                throw std::runtime_error(msg);
            }
        }
    }

    for (const auto& [paramKey, paramValue] : parameters->GetParametersDouble())
    {
        const auto pos = paramKey.find('_');
        const auto channelCausality = paramKey.substr(0, pos);
        const auto channelType = paramKey.substr(pos + 1);

        if (channelCausality == "Parameter")
        {
            if (!channelDefinitionParser.AddParameter(paramValue, channelType))
            {
                const std::string msg = "Unable to parse parameter definition \"" + channelType + "\"";
                LOGERROR(msg);
                throw std::runtime_error(msg);
            }
        }
    }

    for (const auto& [paramKey, paramValue] : parameters->GetParametersBool())
    {
        const auto pos = paramKey.find('_');
        const auto channelCausality = paramKey.substr(0, pos);
        const auto channelType = paramKey.substr(pos + 1);

        if (channelCausality == "Parameter")
        {
            if (!channelDefinitionParser.AddParameter(paramValue, channelType))
            {
                const std::string msg = "Unable to parse parameter definition \"" + channelType + "\"";
                LOGERROR(msg);
                throw std::runtime_error(msg);
            }
        }
    }

    if (!channelDefinitionParser.ParseOutputSignalTypes())
    {
        const std::string msg = "Output signal for FMU incomplete";
        LOGERROR(msg);
        throw std::runtime_error(msg);
    }

    fmuOutputs = channelDefinitionParser.GetFmuOutputs();
    outputSignals = channelDefinitionParser.GetOutputSignals();
    fmuRealInputs = channelDefinitionParser.GetFmuRealInputs();
    fmuIntegerInputs = channelDefinitionParser.GetFmuIntegerInputs();
    fmuBooleanInputs = channelDefinitionParser.GetFmuBooleanInputs();
    fmuStringInputs = channelDefinitionParser.GetFmuStringInputs();
    fmuStringParameters = channelDefinitionParser.GetFmuStringParameters();
    fmuIntegerParameters = channelDefinitionParser.GetFmuIntegerParameters();
    fmuDoubleParameters = channelDefinitionParser.GetFmuDoubleParameters();
    fmuBoolParameters = channelDefinitionParser.GetFmuBoolParameters();
}

void GenericFmuHandler::SetFmuParameters()
{
    if (fmiVersion == fmi_version_enu_t::fmi_version_1_enu)
    {
        fmi1_value_reference_t stringvrs[1];
        fmi1_string_t stringData[1];
        for (const auto& fmuParameter : std::get<FMI1>(fmuStringParameters))
        {
            stringData[0] = fmuParameter.value.data();
            stringvrs[0] = fmuParameter.valueReference;

            SetStringFMI1(stringvrs, stringData, 1);
        }

        fmi1_value_reference_t realvrs[1];
        fmi1_real_t realData[1];
        for (const auto& fmuParameter : std::get<FMI1>(fmuDoubleParameters))
        {
            realData[0] = fmuParameter.value;
            realvrs[0] = fmuParameter.valueReference;

            SetRealFMI1(realvrs, realData, 1);
        }

        fmi1_value_reference_t intvrs[1];
        fmi1_integer_t intData[1];
        for (const auto& fmuParameter : std::get<FMI1>(fmuIntegerParameters))
        {
            intData[0] = fmuParameter.value;
            intvrs[0] = fmuParameter.valueReference;

            SetIntegerFMI1(intvrs, intData, 1);
        }

        fmi1_value_reference_t boolvrs[1];
        fmi1_boolean_t boolData[1];
        for (const auto& fmuParameter : std::get<FMI1>(fmuBoolParameters))
        {
            boolData[0] = fmuParameter.value;
            boolvrs[0] = fmuParameter.valueReference;

            SetBooleanFMI1(boolvrs, boolData, 1);
        }
    }
    else if (fmiVersion == fmi_version_enu_t::fmi_version_2_0_enu)
    {
        fmi2_value_reference_t stringvrs[1];
        fmi2_string_t stringData[1];
        for (const auto& fmuParameter : std::get<FMI2>(fmuStringParameters))
        {
            stringData[0] = fmuParameter.value.data();
            stringvrs[0] = fmuParameter.valueReference;

            SetStringFMI2(stringvrs, stringData, 1);
        }

        fmi2_value_reference_t realvrs[1];
        fmi2_real_t realData[1];
        for (const auto& fmuParameter : std::get<FMI2>(fmuDoubleParameters))
        {
            realData[0] = fmuParameter.value;
            realvrs[0] = fmuParameter.valueReference;

            SetRealFMI2(realvrs, realData, 1);
        }

        fmi2_value_reference_t intvrs[1];
        fmi2_integer_t intData[1];
        for (const auto& fmuParameter : std::get<FMI2>(fmuIntegerParameters))
        {
            intData[0] = fmuParameter.value;
            intvrs[0] = fmuParameter.valueReference;

            SetIntegerFMI2(intvrs, intData, 1);
        }

        fmi2_value_reference_t boolvrs[1];
        fmi2_boolean_t boolData[1];
        for (const auto& fmuParameter : std::get<FMI2>(fmuBoolParameters))
        {
            boolData[0] = fmuParameter.value;
            boolvrs[0] = fmuParameter.valueReference;

            SetBooleanFMI2(boolvrs, boolData, 1);
        }
    }
}

void GenericFmuHandler::SetStringFMI1(fmi1_value_reference_t stringvrs[], fmi1_string_t stringData[], size_t size)
{
    fmi1_status_t fmiStatus = fmi1_status_ok;

    fmiStatus = fmi1_import_set_string(cdata->fmu1,
                                       stringvrs,       // array of value reference
                                       size,             // number of elements
                                       stringData);     // array of values

     if (fmiStatus == fmi1_status_warning)
     {
         LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
     }
     else if (fmiStatus == fmi1_status_error)
     {
         LOG(CbkLogLevel::Error, "setting FMI variables returned error");
         throw std::logic_error("Error setting FMI variables");
     }
}

void GenericFmuHandler::SetStringFMI2(fmi2_value_reference_t stringvrs[], fmi2_string_t stringData[], size_t size)
{
    fmi2_status_t fmiStatus = fmi2_status_ok;

    fmiStatus = fmi2_import_set_string(cdata->fmu2,
                                       stringvrs,       // array of value reference
                                       size,             // number of elements
                                       stringData);     // array of values

    if (fmiStatus == fmi2_status_warning)
    {
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }
}

void GenericFmuHandler::SetRealFMI1(fmi1_value_reference_t realvrs[], fmi1_real_t realData[], size_t size)
{
    fmi1_status_t fmiStatus = fmi1_status_ok;

    fmiStatus = fmi1_import_set_real(cdata->fmu1,
                                     realvrs,       // array of value reference
                                     size,             // number of elements
                                     realData);     // array of values

    if (fmiStatus == fmi1_status_warning)
    {
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi1_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }
}

void GenericFmuHandler::SetRealFMI2(fmi1_value_reference_t realvrs[], fmi1_real_t realData[], size_t size)
{
    fmi2_status_t fmiStatus = fmi2_status_ok;

    fmiStatus = fmi2_import_set_real(cdata->fmu2,
                                     realvrs,       // array of value reference
                                     size,             // number of elements
                                     realData);     // array of values

    if (fmiStatus == fmi2_status_warning)
    {
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }
}

void GenericFmuHandler::SetIntegerFMI1(fmi1_value_reference_t intvrs[], fmi1_integer_t intData[], size_t size)
{
    fmi1_status_t fmiStatus = fmi1_status_ok;

    fmiStatus = fmi1_import_set_integer(cdata->fmu1,
                                        intvrs,       // array of value reference
                                        size,             // number of elements
                                        intData);     // array of values

    if (fmiStatus == fmi1_status_warning)
    {
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi1_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }
}

void GenericFmuHandler::SetIntegerFMI2(fmi2_value_reference_t intvrs[], fmi2_integer_t intData[], size_t size)
{
    fmi2_status_t fmiStatus = fmi2_status_ok;

    fmiStatus = fmi2_import_set_integer(cdata->fmu2,
                                        intvrs,       // array of value reference
                                        size,             // number of elements
                                        intData);     // array of values

    if (fmiStatus == fmi2_status_warning)
    {
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }
}

void GenericFmuHandler::SetBooleanFMI1(fmi1_value_reference_t boolvrs[], fmi1_boolean_t boolData[], size_t size)
{
    fmi1_status_t fmiStatus = fmi1_status_ok;

    fmiStatus = fmi1_import_set_boolean(cdata->fmu1,
                                        boolvrs,       // array of value reference
                                        size,             // number of elements
                                        boolData);     // array of values

    if (fmiStatus == fmi1_status_warning)
    {
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi1_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }
}

void GenericFmuHandler::SetBooleanFMI2(fmi2_value_reference_t boolvrs[], fmi2_boolean_t boolData[], size_t size)
{
    fmi2_status_t fmiStatus = fmi2_status_ok;

    fmiStatus = fmi2_import_set_boolean(cdata->fmu2,
                                        boolvrs,       // array of value reference
                                        size,             // number of elements
                                        boolData);     // array of values

    if (fmiStatus == fmi2_status_warning)
    {
        LOG(CbkLogLevel::Warning, "setting FMI variables returned warning");
    }
    else if (fmiStatus == fmi2_status_error)
    {
        LOG(CbkLogLevel::Error, "setting FMI variables returned error");
        throw std::logic_error("Error setting FMI variables");
    }
}

double GenericFmuHandler::CalculateSpeedLimit(double range)
{
    double speedLimit = 999.0;
    auto trafficSigns = agent->GetEgoAgent().GetTrafficSignsInRange(range);
    auto trafficSignsBehind = agent->GetEgoAgent().GetTrafficSignsInRange(std::numeric_limits<double>::lowest());
    trafficSigns.insert(trafficSigns.end(), trafficSignsBehind.begin(), trafficSignsBehind.end());
    std::sort(trafficSigns.begin(), trafficSigns.end(), [](const CommonTrafficSign::Entity& lhs, const CommonTrafficSign::Entity& rhs)
        {return lhs.relativeDistance > rhs.relativeDistance;});
    for (const auto& sign : trafficSigns)
    {
        if (sign.type == CommonTrafficSign::Type::EndOfMaximumSpeedLimit
            || sign.type == CommonTrafficSign::Type::EndOffAllSpeedLimitsAndOvertakingRestrictions
            || sign.type == CommonTrafficSign::Type::SpeedLimitZoneEnd
            || sign.type == CommonTrafficSign::Type::TrafficCalmedDistrictEnd)
        {
            break;
        }
        if (sign.type == CommonTrafficSign::Type::MaximumSpeedLimit
            || sign.type == CommonTrafficSign::Type::SpeedLimitZoneBegin)
        {
            speedLimit = sign.value;
            break;
        }
        if (sign.type == CommonTrafficSign::Type::TrafficCalmedDistrictBegin)
        {
            speedLimit = 7.0 /3.6;
            break;
        }
    }
    return speedLimit;
}

int GenericFmuHandler::CalculateLaneCount(Side side)
{
    if (!agent->GetEgoAgent().HasValidRoute())
    {
        return 0;
    }
    const auto relativeLanes = agent->GetEgoAgent().GetRelativeLanes(0).at(0);
    return std::count_if(relativeLanes.lanes.cbegin(), relativeLanes.lanes.cend(),
                  [&](const auto& lane)
    {
        return (lane.type == LaneType::Driving
                ||lane.type == LaneType::Exit
                || lane.type == LaneType::Entry
                || lane.type == LaneType::OnRamp
                || lane.type == LaneType::OffRamp)
                && ((side == Side::Left) ? (lane.relativeId > 0) : (lane.relativeId < 0))
                && lane.inDrivingDirection;
    });
}

std::vector<GenericFmuHandler::SensorFusionObjectInfo> GenericFmuHandler::CalculateSensorFusionInfo()
{
    auto worldData = static_cast<OWL::WorldData*>(world->GetWorldData());

    std::vector<std::pair<WorldObjectInterface*, int>> objects;
    std::transform(sensorData.moving_object().cbegin(),
                   sensorData.moving_object().cend(),
                   std::back_inserter(objects),
                   [&](const osi3::DetectedMovingObject& object) {
                       unsigned long osi_id = object.header().ground_truth_id(0).value();
                       return std::make_pair<WorldObjectInterface*, int>(world->GetAgent(osi_id), object.header().sensor_id_size());
                   });
    std::transform(sensorData.stationary_object().cbegin(),
                   sensorData.stationary_object().cend(),
                   std::back_inserter(objects),
                   [&](const osi3::DetectedStationaryObject& object) {
                       unsigned long osi_id = object.header().ground_truth_id(0).value();
                       return std::make_pair<WorldObjectInterface*, int>(worldData->GetStationaryObject(osi_id).GetLink<WorldObjectInterface>(), object.header().sensor_id_size());
                   });

    std::vector<GenericFmuHandler::SensorFusionObjectInfo> sensorFusionInfo;

    std::transform(objects.cbegin(),
                   objects.cend(),
                   std::back_inserter(sensorFusionInfo),
                   [&](std::pair<const WorldObjectInterface*, int> entry)
    {
        const auto object = entry.first;
        const auto numberOfSensors = entry.second;
        auto obstruction = agent->GetEgoAgent().GetObstruction(object, {ObjectPointPredefined::FrontCenter, ObjectPointRelative::Leftmost, ObjectPointRelative::Rightmost});
        auto [xDistance, yDistance] = CommonHelper::GetCartesianNetDistance(agent->GetBoundingBox2D(), object->GetBoundingBox2D());

        return GenericFmuHandler::SensorFusionObjectInfo{
            object->GetId(),
            numberOfSensors,
            obstruction.valid ? obstruction.lateralDistances.at(ObjectPointPredefined::FrontCenter) : NAN,
            obstruction.valid ? obstruction.lateralDistances.at(ObjectPointRelative::Leftmost) : NAN,
            obstruction.valid ? obstruction.lateralDistances.at(ObjectPointRelative::Rightmost) : NAN,
            agent->GetEgoAgent().GetDistanceToObject(object, ObjectPointPredefined::Reference, ObjectPointPredefined::Reference).value_or(NAN),
            agent->GetEgoAgent().GetNetDistance(object).value_or(NAN),
            xDistance,
            yDistance,
            object->GetRoadPosition(ObjectPointPredefined::FrontCenter).cbegin()->second.laneId,
            object->GetVelocity().Length(),
            object->GetVelocity().x,
            object->GetVelocity().y,
            object->GetYaw()
        };
    });

    return sensorFusionInfo;
}
