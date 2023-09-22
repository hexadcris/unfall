/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <set>

#include "FMI1/fmi1_import_variable_list.h"
#include "include/fmuHandlerInterface.h"

using Fmu1Variables = std::unordered_map<std::string, ValueReferenceAndType1>;
using Fmu2Variables = std::unordered_map<std::string, ValueReferenceAndType2>;
using FmuVariables = std::variant<Fmu1Variables, Fmu2Variables>;

//! List of openPASS values (e.g. provided by AgentInterface) that can be assigned to FMU inputs
enum class FmuInputType
{
    VelocityEgo,
    AccelerationEgo,
    CentripetalAccelerationEgo,
    SteeringWheelEgo,
    AccelerationPedalPositionEgo,
    BrakePedalPositionEgo,
    DistanceRefToFrontEdgeEgo,
    PositionXEgo,
    PositionYEgo,
    YawEgo,
    PositionSEgo,
    PositionTEgo,
    LaneEgo,
    ExistenceFront,
    PositionXFront,
    PositionYFront,
    YawFront,
    PositionSFront,
    PositionTFront,
    RelativeDistanceFront,
    WidthFront,
    LengthFront,
    DistanceRefToFrontEdgeFront,
    VelocityFront,
    LaneFront,
    ExistenceFrontFront,
    PositionXFrontFront,
    PositionYFrontFront,
    RelativeDistanceFrontFront,
    VelocityFrontFront,
    LaneFrontFront,
    LaneCountLeft,
    LaneCountRight,
    SpeedLimit,
    RoadCurvature,
    SensorFusionObjectId,
    SensorFusionNumberOfDetectingSensors,
    SensorFusionRelativeS,
    SensorFusionRelativeNetS,
    SensorFusionRelativeT,
    SensorFusionRelativeNetLeft,
    SensorFusionRelativeNetRight,
    SensorFusionRelativeNetX,
    SensorFusionRelativeNetY,
    SensorFusionLane,
    SensorFusionVelocity,
    SensorFusionVelocityX,
    SensorFusionVelocityY,
    SensorFusionYaw
};

//! Type of output signal
enum class SignalType
{
    LateralSignal,
    AccelerationSignal,
    LongitudinalSignal,
    SteeringSignal,
    DynamicsSignal,
};

//! Value in an output signal
enum class SignalValue
{
    ComponentState,
    AccelerationSignal_Acceleration,
    LongitudinalSignal_AccPedalPos,
    LongitudinalSignal_BrakePedalPos,
    LongitudinalSignal_Gear,
    SteeringSignal_SteeringWheelAngle,
    DynamicsSignal_Acceleration,
    DynamicsSignal_Velocity,
    DynamicsSignal_PositionX,
    DynamicsSignal_PositionY,
    DynamicsSignal_Yaw,
    DynamicsSignal_YawRate,
    DynamicsSignal_YawAcceleration,
    DynamicsSignal_SteeringWheelAngle,
    DynamicsSignal_CentripetalAcceleration,
    DynamicsSignal_TravelDistance
};

//! Assignment of a FMU input variable to a value / method of calculation in OpenPASS
struct Fmu1Input
{
    FmuInputType type;                                  //!< Corresponding value in OpenPASS
    std::variant<double, size_t> additionalParameter;   //!< Additional parameter for calculation of the value, if necessary (e.g. distance, index)
    fmi1_value_reference_t valueReference;              //!< Value reference of the FMU

    Fmu1Input(FmuInputType type, std::variant<double, size_t> additionalParameter, fmi1_value_reference_t valueReference) :
        type(type),
        additionalParameter(additionalParameter),
        valueReference(valueReference)
    {}
};

//! Assignment of a FMU input variable to a value / method of calculation in OpenPASS
struct Fmu2Input
{
    FmuInputType type;                                  //!< Corresponding value in OpenPASS
    std::variant<double, size_t> additionalParameter;   //!< Additional parameter for calculation of the value, if necessary (e.g. distance, index)
    fmi2_value_reference_t valueReference;              //!< Value reference of the FMU

    Fmu2Input(FmuInputType type, std::variant<double, size_t> additionalParameter, fmi2_value_reference_t valueReference) :
        type(type),
        additionalParameter(additionalParameter),
        valueReference(valueReference)
    {}
};

using Fmu1Inputs = std::vector<Fmu1Input>;
using Fmu1Outputs = std::map<SignalValue, fmi1_value_reference_t>;
using Fmu2Inputs = std::vector<Fmu2Input>;
using Fmu2Outputs = std::map<SignalValue, fmi2_value_reference_t>;
using FmuInputs = std::variant<Fmu1Inputs, Fmu2Inputs>;
using FmuOutputs = std::variant<Fmu1Outputs, Fmu2Outputs>;

template <typename T>
struct Fmu1Parameter
{
    T value;
    fmi1_value_reference_t valueReference;

    Fmu1Parameter(T value, fmi1_value_reference_t valueReference) :
        value(value),
        valueReference(valueReference)
    {}
};

template <typename T>
struct Fmu2Parameter
{
    T value;
    fmi2_value_reference_t valueReference;

    Fmu2Parameter(T value, fmi2_value_reference_t valueReference) :
        value(value),
        valueReference(valueReference)
    {}
};

template <typename T>
using Fmu1Parameters = std::vector<Fmu1Parameter<T>>;
template <typename T>
using Fmu2Parameters = std::vector<Fmu2Parameter<T>>;
template <typename T>
using FmuParameters = std::variant<Fmu1Parameters<T>, Fmu2Parameters<T>>;

//! maps the string of the parameter to the enum value for the input tpyes
static std::map<std::string, std::pair<FmuInputType,VariableType>> stringToFmuTypeMap
{
    {"VelocityEgo", {FmuInputType::VelocityEgo, VariableType::Double}},
    {"AccelerationEgo", {FmuInputType::AccelerationEgo, VariableType::Double}},
    {"CentripetalAccelerationEgo", {FmuInputType::CentripetalAccelerationEgo, VariableType::Double}},
    {"SteeringWheelEgo", {FmuInputType::SteeringWheelEgo, VariableType::Double}},
    {"AccelerationPedalPositionEgo", {FmuInputType::AccelerationPedalPositionEgo, VariableType::Double}},
    {"BrakePedalPositionEgo", {FmuInputType::BrakePedalPositionEgo, VariableType::Double}},
    {"DistanceRefToFrontEdgeEgo", {FmuInputType::DistanceRefToFrontEdgeEgo, VariableType::Double}},
    {"PositionXEgo", {FmuInputType::PositionXEgo, VariableType::Double}},
    {"PositionYEgo", {FmuInputType::PositionYEgo, VariableType::Double}},
    {"LaneEgo", {FmuInputType::LaneEgo, VariableType::Int}},
    {"YawEgo", {FmuInputType::YawEgo, VariableType::Double}},
    {"PositionSEgo", {FmuInputType::PositionSEgo, VariableType::Double}},
    {"PositionTEgo", {FmuInputType::PositionTEgo, VariableType::Double}},
    {"ExistenceFront", {FmuInputType::ExistenceFront, VariableType::Bool}},
    {"PositionXFront", {FmuInputType::PositionXFront, VariableType::Double}},
    {"PositionYFront", {FmuInputType::PositionYFront, VariableType::Double}},
    {"YawFront", {FmuInputType::YawFront, VariableType::Double}},
    {"PositionSFront", {FmuInputType::PositionSFront, VariableType::Double}},
    {"PositionTFront", {FmuInputType::PositionTFront, VariableType::Double}},
    {"RelativeDistanceFront", {FmuInputType::RelativeDistanceFront, VariableType::Double}},
    {"WidthFront", {FmuInputType::WidthFront, VariableType::Double}},
    {"LengthFront", {FmuInputType::LengthFront, VariableType::Double}},
    {"DistanceRefToFrontEdgeFront", {FmuInputType::DistanceRefToFrontEdgeFront, VariableType::Double}},
    {"VelocityFront", {FmuInputType::VelocityFront, VariableType::Double}},
    {"LaneFront", {FmuInputType::LaneFront, VariableType::Int}},
    {"ExistenceFrontFront", {FmuInputType::ExistenceFrontFront, VariableType::Bool}},
    {"PositionXFrontFront", {FmuInputType::PositionXFrontFront, VariableType::Double}},
    {"PositionYFrontFront", {FmuInputType::PositionYFrontFront, VariableType::Double}},
    {"RelativeDistanceFrontFront", {FmuInputType::RelativeDistanceFrontFront, VariableType::Double}},
    {"VelocityFrontFront", {FmuInputType::VelocityFrontFront, VariableType::Double}},
    {"LaneFrontFront", {FmuInputType::LaneFrontFront, VariableType::Int}},
    {"LaneCountLeft", {FmuInputType::LaneCountLeft, VariableType::Int}},
    {"LaneCountRight", {FmuInputType::LaneCountRight, VariableType::Int}},
    {"SpeedLimit", {FmuInputType::SpeedLimit, VariableType::Double}},
    {"RoadCurvature", {FmuInputType::RoadCurvature, VariableType::Double}},
    {"SensorFusionObjectId", {FmuInputType::SensorFusionObjectId, VariableType::Int}},
    {"SensorFusionNumberOfDetectingSensors", {FmuInputType::SensorFusionNumberOfDetectingSensors, VariableType::Int}},
    {"SensorFusionRelativeS", {FmuInputType::SensorFusionRelativeS, VariableType::Double}},
    {"SensorFusionRelativeNetS", {FmuInputType::SensorFusionRelativeNetS, VariableType::Double}},
    {"SensorFusionRelativeT", {FmuInputType::SensorFusionRelativeT, VariableType::Double}},
    {"SensorFusionRelativeNetLeft", {FmuInputType::SensorFusionRelativeNetLeft, VariableType::Double}},
    {"SensorFusionRelativeNetRight", {FmuInputType::SensorFusionRelativeNetRight, VariableType::Double}},
    {"SensorFusionRelativeNetX", {FmuInputType::SensorFusionRelativeNetX, VariableType::Double}},
    {"SensorFusionRelativeNetY", {FmuInputType::SensorFusionRelativeNetY, VariableType::Double}},
    {"SensorFusionLane", {FmuInputType::SensorFusionLane, VariableType::Int}},
    {"SensorFusionVelocity", {FmuInputType::SensorFusionVelocity, VariableType::Double}},
    {"SensorFusionVelocityX", {FmuInputType::SensorFusionVelocityX, VariableType::Double}},
    {"SensorFusionVelocityY", {FmuInputType::SensorFusionVelocityY, VariableType::Double}},
    {"SensorFusionYaw", {FmuInputType::SensorFusionYaw, VariableType::Double}} };

//! maps the string of the parameter to the enum value for the output tpyes
static std::map<std::string, std::pair<SignalValue,VariableType>> stringToSignalValueMap
{
    {"ComponentState", {SignalValue::ComponentState, VariableType::Enum}},
    {"AccelerationSignal_Acceleration", {SignalValue::AccelerationSignal_Acceleration, VariableType::Double}},
    {"LongitudinalSignal_AccPedalPos", {SignalValue::LongitudinalSignal_AccPedalPos, VariableType::Double}},
    {"LongitudinalSignal_BrakePedalPos", {SignalValue::LongitudinalSignal_BrakePedalPos, VariableType::Double}},
    {"LongitudinalSignal_Gear", {SignalValue::LongitudinalSignal_Gear, VariableType::Int}},
    {"SteeringSignal_SteeringWheelAngle", {SignalValue::SteeringSignal_SteeringWheelAngle, VariableType::Double}},
    {"DynamicsSignal_Acceleration", {SignalValue::DynamicsSignal_Acceleration, VariableType::Double}},
    {"DynamicsSignal_Velocity", {SignalValue::DynamicsSignal_Velocity, VariableType::Double}},
    {"DynamicsSignal_PositionX", {SignalValue::DynamicsSignal_PositionX, VariableType::Double}},
    {"DynamicsSignal_PositionY", {SignalValue::DynamicsSignal_PositionY, VariableType::Double}},
    {"DynamicsSignal_Yaw", {SignalValue::DynamicsSignal_Yaw, VariableType::Double}},
    {"DynamicsSignal_YawRate", {SignalValue::DynamicsSignal_YawRate, VariableType::Double}},
    {"DynamicsSignal_YawAcceleration", {SignalValue::DynamicsSignal_YawAcceleration, VariableType::Double}},
    {"DynamicsSignal_SteeringWheelAngle", {SignalValue::DynamicsSignal_SteeringWheelAngle, VariableType::Double}},
    {"DynamicsSignal_CentripetalAcceleration", {SignalValue::DynamicsSignal_CentripetalAcceleration, VariableType::Double}},
    {"DynamicsSignal_TravelDistance", {SignalValue::DynamicsSignal_TravelDistance, VariableType::Double}}
};

//! defines which values are needed for each signal
static std::map<SignalType, std::vector<SignalValue>> valuesOfSignalType
{
    {SignalType::AccelerationSignal, {SignalValue::AccelerationSignal_Acceleration}},
    {SignalType::LongitudinalSignal, {SignalValue::LongitudinalSignal_AccPedalPos,
                                      SignalValue::LongitudinalSignal_BrakePedalPos,
                                      SignalValue::LongitudinalSignal_Gear}},
    {SignalType::SteeringSignal, {SignalValue::SteeringSignal_SteeringWheelAngle}},
    {SignalType::DynamicsSignal, {SignalValue::DynamicsSignal_Acceleration,
                                  SignalValue::DynamicsSignal_Velocity,
                                  SignalValue::DynamicsSignal_PositionX,
                                  SignalValue::DynamicsSignal_PositionY,
                                  SignalValue::DynamicsSignal_Yaw,
                                  SignalValue::DynamicsSignal_YawRate,
                                  SignalValue::DynamicsSignal_YawAcceleration,
                                  SignalValue::DynamicsSignal_SteeringWheelAngle,
                                  SignalValue::DynamicsSignal_CentripetalAcceleration,
                                  SignalValue::DynamicsSignal_TravelDistance}}
};

class ChannelDefinitionParser
{
    public:
        /*!
         * \brief ChannelDefinitionParser constructor
         *
         * \param[in]       fmuVariables        Map from FMU variable names to their value references
         * \param[in]       fmiVersion          interface version of FMI
         */
        ChannelDefinitionParser(const FmuVariables& fmuVariables, fmi_version_enu_t fmiVersion);

        /*!
         * \brief Adds a FMU output channel mapping
         *
         * \param[in]       outputType      type of the output variable
         * \param[in]       variableName    FMU variable name
         *
         * \return          true on success, false otherwise.
         */
        bool AddOutputChannel(const std::string& outputType, const std::string& variableName);

        /*!
         * \brief Adds a FMU input channel mapping
         *
         * \param[in]       inputType       type of the input variable (e.g. "VelocityEgo")
         * \param[in]       variableName    FMU variable name
         *
         * \return          true on success, false otherwise.
         */
        bool AddInputChannel(const std::string& inputType, const std::string& variableName);

        /*!
         * \brief Adds a FMU parameter channel mapping
         *
         * \param[in]       inputValue      value of the parameter
         * \param[in]       variableName    FMU channel definition
         *
         * \return          true on success, false otherwise.
         */
        template<typename T>
        bool AddParameter(const T& inputValue, const std::string& variableName)
        {
            if (fmiVersion == fmi_version_enu_t::fmi_version_1_enu)
            {
                return AddParameter<T,FMI1>(inputValue, variableName);
            }
            else if (fmiVersion == fmi_version_enu_t::fmi_version_2_0_enu)
            {
                return AddParameter<T,FMI2>(inputValue, variableName);
            }
            else
            {
                return false;
            }
        }

        /*!
         * \brief Adds a FMU parameter channel mapping
         *
         * \param[in]       inputValue      value of the parameter
         * \param[in]       variableName    FMU channel definition
         *
         * \return          true on success, false otherwise.
         */
        template<typename T, size_t Version>
        bool AddParameter(const T& inputValue, const std::string& variableName)
        {
            auto unmappedVariableItem = std::get<Version>(unmappedFmuVariables).find(variableName);

            if (unmappedVariableItem == std::get<Version>(unmappedFmuVariables).end())
            {
                return false;
            }

            const auto& [valueReference, variableType] = unmappedVariableItem->second;

            if constexpr (std::is_same_v<T, std::string>)
            {
                if (variableType == VariableType::String)
                {
                    std::get<Version>(fmuStringParameters).emplace_back(inputValue, valueReference);
                    std::get<Version>(unmappedFmuVariables).erase(unmappedVariableItem);
                    return true;
                }
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                if (variableType == VariableType::Int)
                {
                    std::get<Version>(fmuIntegerParameters).emplace_back(inputValue, valueReference);
                    std::get<Version>(unmappedFmuVariables).erase(unmappedVariableItem);
                    return true;
                }
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                if (variableType == VariableType::Double)
                {
                    std::get<Version>(fmuDoubleParameters).emplace_back(inputValue, valueReference);
                    std::get<Version>(unmappedFmuVariables).erase(unmappedVariableItem);
                    return true;
                }
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                if (variableType == VariableType::Bool)
                {
                    std::get<Version>(fmuBoolParameters).emplace_back(inputValue, valueReference);
                    std::get<Version>(unmappedFmuVariables).erase(unmappedVariableItem);
                    return true;
                }
            }

            return false;
        }

        /*!
         * \brief Retrieve the output link id mapping
         *
         * \return          Map from output link id to tuple of value reference with type id hash code
         */
        FmuOutputs GetFmuOutputs();

        std::set<SignalType> GetOutputSignals();

        bool ParseOutputSignalTypes();

        /*!
         * \brief Retrieve the input link id mapping for real variables
         *
         * \return          Map from input type to corresponding FMU value reference
         */
        FmuInputs GetFmuRealInputs();

        /*!
         * \brief Retrieve the input link id mapping for integer variables
         *
         * \return          Map from input type to corresponding FMU value reference
         */
        FmuInputs GetFmuIntegerInputs();

        /*!
         * \brief Retrieve the input link id mapping for boolean variables
         *
         * \return          Map from input type to corresponding FMU value reference
         */
        FmuInputs GetFmuBooleanInputs();

        /*!
         * \brief Retrieve the input link id mapping for string variables
         *
         * \return          Map from input type to corresponding FMU value reference
         */
        FmuInputs GetFmuStringInputs();

        /*!
         * \brief Retrieve the input link id mapping for string parameters
         *
         * \return          List of FMU parameters and their value
         */
        FmuParameters<std::string> GetFmuStringParameters();

        /*!
         * \brief Retrieve the input link id mapping for integer parameters
         *
         * \return          List of FMU parameters and their value
         */
        FmuParameters<int> GetFmuIntegerParameters();

        /*!
         * \brief Retrieve the input link id mapping for double parameters
         *
         * \return          List of FMU parameters and their value
         */
        FmuParameters<double> GetFmuDoubleParameters();

        /*!
         * \brief Retrieve the input link id mapping for bool parameters
         *
         * \return          List of FMU parameters and their value
         */
        FmuParameters<bool> GetFmuBoolParameters();

    private:
        const fmi_version_enu_t fmiVersion;

        FmuVariables unmappedFmuVariables;      //!< FMU variables without output link mapped
        FmuOutputs fmuOutputs;                  //!< output link id mapping
        std::set<SignalType> outputSignals;
        FmuInputs fmuRealInputs;
        FmuInputs fmuIntegerInputs;
        FmuInputs fmuBooleanInputs;
        FmuInputs fmuStringInputs;
        FmuParameters<int> fmuIntegerParameters;
        FmuParameters<double> fmuDoubleParameters;
        FmuParameters<bool> fmuBoolParameters;
        FmuParameters<std::string> fmuStringParameters;
};
