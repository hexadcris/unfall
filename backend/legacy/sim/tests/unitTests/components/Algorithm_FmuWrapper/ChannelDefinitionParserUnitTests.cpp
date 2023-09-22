/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ChannelDefinitionParser.h"

#include <tuple>
#include <typeindex>
#include <unordered_map>

using ::testing::Return;
using ::testing::UnorderedElementsAre;
using ::testing::Eq;

const FmuVariables testFmu1Variables =
        FmuVariables{ std::in_place_index_t<0>(),
        Fmu1Variables{{ "BoolTestChannel",       ValueReferenceAndType1{ 0, VariableType::Bool } },
                      { "IntTestChannel",        ValueReferenceAndType1{ 1, VariableType::Int } },
                      { "Real1TestChannel",      ValueReferenceAndType1{ 2, VariableType::Double } },
                      { "Real2TestChannel",      ValueReferenceAndType1{ 3, VariableType::Double } },
                      { "Real3TestChannel",      ValueReferenceAndType1{ 4, VariableType::Double } },
                      { "StringTestChannel",     ValueReferenceAndType1{ 5, VariableType::String } },
                      { "UnusedBoolTestChannel", ValueReferenceAndType1{ 6, VariableType::Bool } }},
        };

bool operator== (const Fmu1Input& lhs, const Fmu1Input& rhs)
{
    return lhs.type == rhs.type
                      && lhs.valueReference == rhs.valueReference
                      && lhs.additionalParameter == rhs.additionalParameter;
};

template <typename T>
bool operator== (const Fmu1Parameter<T>& lhs, const Fmu1Parameter<T>& rhs)
{
    return lhs.value == rhs.value
            && lhs.valueReference == rhs.valueReference;
};

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddEmptyDefinition_ReturnsFalse)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     ASSERT_FALSE(channelDefinitionParser.AddOutputChannel("DynamicsSignal_Velocity", ""));
     ASSERT_FALSE(channelDefinitionParser.AddInputChannel("VelocityEgo", " "));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddInvalidOutputChannel_ReturnsFalse)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     EXPECT_FALSE(channelDefinitionParser.AddOutputChannel("DynamicsSignal_Velocity", "NonExistantTestChannel"));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddValidOutputChannelName_ReturnsTrue)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     ASSERT_TRUE(channelDefinitionParser.AddOutputChannel("DynamicsSignal_Velocity", "Real1TestChannel"));
}
TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddOutputChannelWithWrongType_ReturnsFalse)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     ASSERT_FALSE(channelDefinitionParser.AddOutputChannel("DynamicsSignal_Velocity", "BoolTestChannel"));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddValidOutputChannelTwice_ReturnsFalse)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     EXPECT_TRUE(channelDefinitionParser.AddOutputChannel("DynamicsSignal_Velocity", "Real1TestChannel"));
     EXPECT_FALSE(channelDefinitionParser.AddOutputChannel("DynamicsSignal_Acceleration", "Real1TestChannel"));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddInvalidInputChannel_ReturnsFalse)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     EXPECT_FALSE(channelDefinitionParser.AddInputChannel("VelocityEgo", "NonExistantTestChannel"));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddValidInputChannelName_ReturnsTrue)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     ASSERT_TRUE(channelDefinitionParser.AddInputChannel("VelocityEgo", "Real1TestChannel"));
     ASSERT_TRUE(channelDefinitionParser.AddInputChannel("SpeedLimit_100.0", "Real2TestChannel"));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddInputChannelWithWrongType_ReturnsFalse)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     ASSERT_FALSE(channelDefinitionParser.AddInputChannel("VelocityEgo", "BoolTestChannel"));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, AddValidInputChannelTwice_ReturnsFalse)
{
     ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
     EXPECT_TRUE(channelDefinitionParser.AddInputChannel("VelocityEgo", "Real1TestChannel"));
     EXPECT_FALSE(channelDefinitionParser.AddInputChannel("AccelerationEgo", "Real1TestChannel"));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, ValidOutputConfiguration_GeneratesValidMap)
{
    ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
    channelDefinitionParser.AddOutputChannel("DynamicsSignal_Velocity", "Real1TestChannel");
    channelDefinitionParser.AddOutputChannel("DynamicsSignal_Acceleration", "Real2TestChannel");
    channelDefinitionParser.AddOutputChannel("DynamicsSignal_PositionX", "Real3TestChannel");

    auto outputs = std::get<FMI1>(channelDefinitionParser.GetFmuOutputs());

    ASSERT_THAT(outputs, UnorderedElementsAre(
                    std::make_pair<SignalValue, fmi1_value_reference_t>(SignalValue::DynamicsSignal_Velocity, 2),
                    std::make_pair<SignalValue, fmi1_value_reference_t>(SignalValue::DynamicsSignal_Acceleration, 3),
                    std::make_pair<SignalValue, fmi1_value_reference_t>(SignalValue::DynamicsSignal_PositionX, 4)));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, ValidInputConfiguration_GeneratesValidMap)
{
    ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
    channelDefinitionParser.AddInputChannel("VelocityEgo", "Real1TestChannel");
    channelDefinitionParser.AddInputChannel("SpeedLimit_1.0", "Real2TestChannel");
    channelDefinitionParser.AddInputChannel("SpeedLimit_10.5", "Real3TestChannel");
    channelDefinitionParser.AddInputChannel("SensorFusionLane_0", "IntTestChannel");
    channelDefinitionParser.AddInputChannel("ExistenceFront", "BoolTestChannel");

    auto realInputs = std::get<FMI1>(channelDefinitionParser.GetFmuRealInputs());
    auto booleanInputs = std::get<FMI1>(channelDefinitionParser.GetFmuBooleanInputs());
    auto intInputs = std::get<FMI1>(channelDefinitionParser.GetFmuIntegerInputs());

    ASSERT_THAT(realInputs, UnorderedElementsAre(
                    Fmu1Input(FmuInputType::VelocityEgo, {0.}, 2),
                    Fmu1Input(FmuInputType::SpeedLimit, {1.0}, 3),
                    Fmu1Input(FmuInputType::SpeedLimit, {10.5}, 4)));

    ASSERT_THAT(intInputs, UnorderedElementsAre(
                    Fmu1Input(FmuInputType::SensorFusionLane, {size_t{0}}, 1)));

    ASSERT_THAT(booleanInputs, UnorderedElementsAre(
                    Fmu1Input(FmuInputType::ExistenceFront, {0.}, 0)));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, ParseOutputSignalTypesIncompleteSignal_ReturnsFalse)
{
    ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
    channelDefinitionParser.AddOutputChannel("LongitudinalSignal_AccPedalPos", "Real1TestChannel");
    channelDefinitionParser.AddOutputChannel("LongitudinalSignal_Gear", "IntTestChannel");

    ASSERT_THAT(channelDefinitionParser.ParseOutputSignalTypes(), Eq(false));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, ParseOutputSignalTypesCompleteSignal_ReturnsTrueAndParseRightSignal)
{
    ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
    channelDefinitionParser.AddOutputChannel("LongitudinalSignal_AccPedalPos", "Real1TestChannel");
    channelDefinitionParser.AddOutputChannel("LongitudinalSignal_BrakePedalPos", "Real2TestChannel");
    channelDefinitionParser.AddOutputChannel("LongitudinalSignal_Gear", "IntTestChannel");

    ASSERT_THAT(channelDefinitionParser.ParseOutputSignalTypes(), Eq(true));
    auto signalType = channelDefinitionParser.GetOutputSignals();
    ASSERT_THAT(signalType, UnorderedElementsAre(SignalType::LongitudinalSignal));
}

TEST(FmuWrapper_ChannelDefinitionParser_UnitTests, ParseParameters)
{
    using namespace std::string_literals;

    ChannelDefinitionParser channelDefinitionParser(testFmu1Variables, fmi_version_enu_t::fmi_version_1_enu);
    channelDefinitionParser.AddParameter("Test"s, "StringTestChannel");
    channelDefinitionParser.AddParameter(12, "IntTestChannel");
    channelDefinitionParser.AddParameter(1.23, "Real1TestChannel");
    channelDefinitionParser.AddParameter(12.3, "Real2TestChannel");
    channelDefinitionParser.AddParameter(true, "BoolTestChannel");

    auto stringParameter = std::get<FMI1>(channelDefinitionParser.GetFmuStringParameters());
    auto intParameter = std::get<FMI1>(channelDefinitionParser.GetFmuIntegerParameters());
    auto doubleParameter = std::get<FMI1>(channelDefinitionParser.GetFmuDoubleParameters());
    auto boolParameter = std::get<FMI1>(channelDefinitionParser.GetFmuBoolParameters());

    ASSERT_THAT(stringParameter, UnorderedElementsAre(
                    Fmu1Parameter<std::string>("Test", 5)));

    ASSERT_THAT(intParameter, UnorderedElementsAre(
                    Fmu1Parameter<int>(12, 1)));

    ASSERT_THAT(doubleParameter, UnorderedElementsAre(
                    Fmu1Parameter<double>(1.23, 2),
                    Fmu1Parameter<double>(12.3, 3)));

    ASSERT_THAT(boolParameter, UnorderedElementsAre(
                    Fmu1Parameter<bool>(true, 0)));
}
