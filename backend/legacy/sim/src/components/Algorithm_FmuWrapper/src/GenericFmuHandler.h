/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

// ----------------------------------------------------------------------------
//!	@file  GenericFmuHandler.h
//!	@brief Header file of the implementation of the GenericFmuHandler
// ----------------------------------------------------------------------------

#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <set>

#include "include/signalInterface.h"

#include "ChannelDefinitionParser.h"
#include "include/fmuHandlerInterface.h"
#include "include/parameterInterface.h"
#include "include/worldInterface.h"
#include "common/globalDefinitions.h"
#include "osi3/osi_sensordata.pb.h"

class AgentInterface;
class CallbackInterface;

/*!
 * \brief The GenericFmuHandler class allows to link any FMU to OpenPASS
 */
class GenericFmuHandler : public FmuHandlerInterface
{
public:
    GenericFmuHandler(const FmuVariables& fmuVariables,
                      std::map<ValueReferenceAndType, FmuHandlerInterface::FmuValue>& fmuVariableValues,
                      const ParameterInterface *parameters,
                      struct fmu_check_data_t* cdata,
                      AgentInterface* agent,
                      const CallbackInterface* callbacks,
                      WorldInterface* world);

    virtual ~GenericFmuHandler() override;

    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time) override;
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time) override;

    void PreStep(int time) override;
    void PostStep(int time) override;

    void Init() override;

    /*!
     * \brief Parses the channel definitions from FMU wrapper parameters
     * \param parameters    parameters defined in the config
     * \param fmuVariables  variables of the fmu
     *
     * \returns     Map from output link id to FMI value reference and type hash code
     */
    void ParseChannelDefinitions(const ParameterInterface* parameters, const FmuVariables& fmuVariables);

    //! Sets the parameters in the FMU
    void SetFmuParameters();

private:

    template <size_t I>
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    template <size_t I>
    FmuHandlerInterface::FmuValue& GetValue(SignalValue signalValue, VariableType variableType);

    void SetStringFMI1(fmi1_value_reference_t stringvrs[], fmi1_string_t stringData[], size_t size);

    void SetStringFMI2(fmi2_value_reference_t stringvrs[], fmi2_string_t stringData[], size_t size);

    void SetRealFMI1(fmi1_value_reference_t realvrs[], fmi1_real_t realData[], size_t size);

    void SetRealFMI2(fmi2_value_reference_t realvrs[], fmi2_real_t realData[], size_t size);

    void SetIntegerFMI1(fmi1_value_reference_t intvrs[], fmi1_integer_t intData[], size_t size);

    void SetIntegerFMI2(fmi2_value_reference_t intvrs[], fmi2_integer_t intData[], size_t size);

    void SetBooleanFMI1(fmi1_value_reference_t boolvrs[], fmi1_boolean_t boolData[], size_t size);

    void SetBooleanFMI2(fmi2_value_reference_t boolvrs[], fmi2_boolean_t boolData[], size_t size);

    double CalculateSpeedLimit(double range);

    int CalculateLaneCount(Side side);

    struct SensorFusionObjectInfo
    {
        int id{-1};
        int numberOfSensors{0};
        double t{0};
        double t_left{0};
        double t_right{0};
        double s{0};
        double net_s{0};
        double net_x{0};
        double net_y{0};
        int lane{0};
        double velocity{0};
        double velocity_x{0};
        double velocity_y{0};
        double yaw{0};
    };

    std::vector<SensorFusionObjectInfo> CalculateSensorFusionInfo();

    fmi_version_enu_t fmiVersion;
    //! Mapping from FMI value reference and C++ type id to FmuWrapper value (union). Provided by FmuWrapper on construction.
    std::map<ValueReferenceAndType, FmuHandlerInterface::FmuValue>& fmuVariableValues;

    FmuOutputs fmuOutputs;                      //!< Mapping from component output link id to to FMI value reference and C++ type id
    std::set<SignalType> outputSignals;

    FmuInputs fmuRealInputs;
    FmuInputs fmuIntegerInputs;
    FmuInputs fmuBooleanInputs;
    FmuInputs fmuStringInputs;
    FmuParameters<int> fmuIntegerParameters;
    FmuParameters<double> fmuDoubleParameters;
    FmuParameters<bool> fmuBoolParameters;
    FmuParameters<std::string> fmuStringParameters;

    osi3::SensorData sensorData {};
    std::vector<int> sensors;
    WorldInterface* world;
};
