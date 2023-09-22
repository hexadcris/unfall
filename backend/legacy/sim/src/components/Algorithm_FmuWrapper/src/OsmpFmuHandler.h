/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *               2019-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <QString>

#include "include/agentInterface.h"
#include "include/fmuHandlerInterface.h"
#include "include/parameterInterface.h"
#include "include/signalInterface.h"
#include "include/worldInterface.h"
#include "osi3/osi_groundtruth.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_sensorview.pb.h"
#include "osi3/osi_trafficcommand.pb.h"
#include "osi3/osi_trafficupdate.pb.h"

#ifdef USE_EXTENDED_OSI
#include "osi3/sl45_motioncommand.pb.h"
#include "osi3/sl45_vehiclecommunicationdata.pb.h"
#endif

#include <any>
#include <regex>
#include <fstream>
#include <filesystem>

#include "common/openScenarioDefinitions.h"

class CallbackInterface;

class OsmpFmuHandler : public FmuHandlerInterface
{
public:
    OsmpFmuHandler(fmu_check_data_t *cdata,
                   WorldInterface *world,
                   AgentInterface* agent,
                   const CallbackInterface* callbacks,
                   const Fmu2Variables& fmuVariables,
                   std::map<ValueReferenceAndType, FmuValue>* fmuVariableValues,
                   const ParameterInterface *parameters);


    ~OsmpFmuHandler() override;

    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, int time) override;

    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time) override;

    void Init() override;

    void PreStep(int time) override;

    void PostStep(int time) override;

    //! Public for testing
    //! Reads an output value of the FMU
    FmuHandlerInterface::FmuValue& GetValue(fmi2_value_reference_t valueReference, VariableType variableType) const;

    //! Adds a trajectory from OpenSCENARIO to a OSI TrafficAction
    static void AddTrafficCommandActionFromOpenScenarioTrajectory(osi3::TrafficAction *trafficAction,
                                                                  const openScenario::Trajectory& trajectory);

    //! Adds a position from OpenSCENARIO to a OSI TrafficAction
    static void AddTrafficCommandActionFromOpenScenarioPosition(osi3::TrafficAction *trafficAction,
                                                                const openScenario::Position &position,
                                                                WorldInterface *worldInterface,
                                                                const std::function<void(const std::string &)> &errorCallback);

    //! Parameter of the FMU that was defined in the profile
    template <typename T>
    struct FmuParameter
    {
        T value;
        fmi2_value_reference_t valueReference;

        FmuParameter(T value, fmi2_value_reference_t valueReference) :
            value(value),
            valueReference(valueReference)
        {
        }
    };

    template <typename T>
    using FmuParameters = std::vector<FmuParameter<T>>;

private:
    template <typename T>
    struct TypeContainer;

    template <typename GetFunction, typename TargetParameterField, typename UnderlyingType>
    void ParseFmuParametersByType(GetFunction getParametersType,
                                  const ParameterInterface *parameterInterface,
                                  TargetParameterField fmuTypeParameters,
                                  TypeContainer<UnderlyingType> typeContainer);

    //! Reads the parameters in the profile that should be forwarded to the FMU
    void ParseFmuParameters(const ParameterInterface *parameters);

    //! Sets the parameters in the FMU
    void SetFmuParameters();

    //! Sets the SensorView as input for the FMU
    void SetSensorViewInput(const osi3::SensorView &data);

    //! Sets the SensorData as input for the FMU
    void SetSensorDataInput(const osi3::SensorData& data);

    //! Reads the SensorData from the FMU
    void GetSensorData();

    //! Sets the SensorViewConfig as input for the FMU
    void SetSensorViewConfig();

    //! Sets the SensorViewConfigRequest
    void SetSensorViewConfigRequest();

    //! Sets the TrafficCommand as input for the FMU
    void SetTrafficCommandInput(const osi3::TrafficCommand &data);

    //! Reads the TrafficUpdate from the FMU
    void GetTrafficUpdate();

#ifdef USE_EXTENDED_OSI
    //! Reads the MotionCommand from the FMU
    void GetMotionCommand();

    //! Sets the VehicleCommunicationData as input for the FMU
    void SetVehicleCommunicationDataInput(const setlevel4to5::VehicleCommunicationData &data);

    //! Sets the MotionCommand as input for the FMU (makes it also available for json output, this needs refactoring)
    void SetMotionCommandDataInput(const setlevel4to5::MotionCommand &data);

#endif

    //! Writes an OSI message into a JSON file
    void WriteJson(const google::protobuf::Message &message, const QString &fileName);

    osi3::SensorViewConfiguration GenerateDefaultSensorViewConfiguration();

    WorldInterface* world;
    std::map<ValueReferenceAndType, FmuHandlerInterface::FmuValue>* fmuVariableValues = nullptr;
    const Fmu2Variables& fmuVariables;
    const std::string agentIdString;
    std::string fmuName;

    FmuParameters<int> fmuIntegerParameters;
    FmuParameters<double> fmuDoubleParameters;
    FmuParameters<bool> fmuBoolParameters;
    FmuParameters<std::string> fmuStringParameters;

    std::string serializedSensorDataIn;
    std::string appendedSerializedSensorDataIn;
    std::string appendedSerializedSensorDataOut;
    std::string previousSerializedSensorDataIn;
    std::string serializedSensorView;
    std::string previousSerializedSensorView;
    std::string appendedSerializedSensorView;
    void* previousSensorDataOut{nullptr};
    osi3::SensorViewConfiguration sensorViewConfig;
    osi3::SensorViewConfiguration sensorViewConfigRequest;
    std::string serializedSensorViewConfig;
    std::string appendedSerializedSensorViewConfig;
    std::string serializedSensorViewConfigRequest;
    std::string appendedSerializedSensorViewConfigRequest;
    std::string previousSerializedSensorViewConfigRequest;
    osi3::SensorData sensorDataIn;
    osi3::SensorData sensorDataOut;
    std::string serializedGroundTruth;
    std::string appendedSerializedGroundTruth;
    std::string serializedTrafficCommand;
    std::string appendedSerializedTrafficCommand;
    std::string previousSerializedTrafficCommand;
    std::string serializedTrafficUpdate;
    std::string appendedSerializedTrafficUpdate;
    osi3::TrafficUpdate trafficUpdate;
    void* previousTrafficUpdate{nullptr};
    osi3::TrafficCommand trafficCommand;
    std::map<int, std::unique_ptr<osi3::TrafficCommand>> trafficCommands{};

#ifdef USE_EXTENDED_OSI
    std::string serializedVehicleCommunicationData;
    std::string appendedSerializedVehicleCommunicationData;
    std::string previousSerializedVehicleCommunicationData;
    setlevel4to5::VehicleCommunicationData vehicleCommunicationData;
    setlevel4to5::MotionCommand motionCommand;
    std::string previousSerializedMotionCommand;
    std::string serializedMotionCommand;
    std::string appendedSerializedMotionCommand;
    void* previousMotionCommand{nullptr};
#endif

    std::optional<std::string> sensorViewVariable;
    std::optional<std::string> sensorViewConfigVariable;
    std::optional<std::string> sensorViewConfigRequestVariable;
    std::optional<std::string> sensorDataInVariable;
    std::optional<std::string> sensorDataOutVariable;
    std::optional<std::string> groundTruthVariable;
    std::optional<std::string> trafficCommandVariable;
    std::optional<std::string> trafficUpdateVariable;

#ifdef USE_EXTENDED_OSI
    std::optional<std::string> motionCommandVariable;
    std::optional<std::string> vehicleCommunicationDataVariable;
#endif

    const std::map<std::string, std::map<std::string, std::optional<std::string>&>> variableMapping{
        {"Input",
         {{"SensorView", sensorViewVariable},
          {"SensorViewConfig", sensorViewConfigVariable},
          {"SensorData", sensorDataInVariable},
          {"TrafficCommand", trafficCommandVariable},
#ifdef USE_EXTENDED_OSI
          {"VehicleCommunicationData", vehicleCommunicationDataVariable}
#endif
         }},
        {"Output",
         {{"SensorData", sensorDataOutVariable},
          {"SensorViewConfigRequest", sensorViewConfigRequestVariable},
          {"TrafficUpdate", trafficUpdateVariable},
#ifdef USE_EXTENDED_OSI
          {"MotionCommand", motionCommandVariable}
#endif
         }},
        {"Init",
         {{"GroundTruth", groundTruthVariable}}}};

    template <typename T>
    struct TypeContainer
    {
        using type = T;
        VariableType variableType;
    };

    std::regex transformationRegex{R"((\w+)\[(\w+>\w+)\]_(\w+))"};
    const std::set<std::string> parameterTransformations{"Transform", "TransformList"};
    // TODO: storing with std::any works fine but poses potential problems when any-casting to const/volatile/reference/pointer mismatch
    const std::map<std::string, std::any> parameterTransformationMappings{
        {"ScenarioName>Id", static_cast<std::function<std::string(const std::string &)>>([this](const std::string &agentScenarioName) {
             const auto agent = world->GetAgentByName(agentScenarioName);
             if (agent == nullptr)
                 LOGERRORANDTHROW("Agent '" + agentScenarioName + "' not found in world.")
             return std::to_string(agent->GetId());
         })}};

    bool writeSensorView{false};
    bool writeSensorViewConfig{false};
    bool writeSensorViewConfigRequest{false};
    bool writeSensorData{false};
    bool writeGroundTruth{false};
    bool writeTrafficCommand{false};
    bool writeTrafficUpdate{false};

    bool writeTraceSensorView{false};
    bool writeTraceSensorViewConfig{false};
    bool writeTraceSensorViewConfigRequest{false};
    bool writeTraceSensorData{false};
    bool writeTraceGroundTruth{false};
    bool writeTraceTrafficCommand{false};
    bool writeTraceTrafficUpdate{false};

#ifdef USE_EXTENDED_OSI
    bool writeMotionCommand{false};
    bool writeVehicleCommunicationData{false};

    bool writeTraceMotionCommand{false};
    bool writeTraceVehicleCommunicationData{false};
#endif

    //! check for double buffering of OSI messages allocated by FMU
    bool enforceDoubleBuffering{false};

    QString outputDir{};
    QString traceOutputDir{};
    std::string oldFileName{};

    std::map<std::string, std::pair<std::string, int>> fileToOutputTracesMap{};

    Common::Vector2d previousPosition{0.0,0.0};


    void AppendMessages(std::string &appendedMessage, std::string& message);

    std::ofstream traceOutputFile;

    double bb_center_offset_x{0.0};    //!< Offset of bounding box center to agent reference point (rear axle)
};
