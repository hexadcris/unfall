/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgent.h"

#include "sensorFusionImpl.h"
#include "common/sensorFusionQuery.h"

using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::VariantWith;

TEST(SensorFusionErrorless_Tests, SensorDataWithMovingObjects_IsMergedAppropriately)
{
    NiceMock<FakeAgent> fakeAgent;

    auto sensorFusion = SensorFusionErrorlessImplementation("",
                                                            false,
                                                            0,
                                                            0,
                                                            0,
                                                            100,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            &fakeAgent);
    unsigned int idA = 7;
    unsigned int idB = 8;
    unsigned int idC = 9;

    unsigned int idSensor1 = 101;
    unsigned int idSensor2 = 102;

    osi3::SensorData sensorData;
    auto movingObject1a = sensorData.add_moving_object();
    movingObject1a->mutable_header()->add_ground_truth_id()->set_value(idA);
    movingObject1a->mutable_header()->add_sensor_id()->set_value(idSensor1);
    movingObject1a->mutable_base()->mutable_position()->set_x(10);
    movingObject1a->mutable_base()->mutable_position()->set_y(11);
    auto movingObject1b = sensorData.add_moving_object();
    movingObject1b->mutable_header()->add_ground_truth_id()->set_value(idB);
    movingObject1b->mutable_header()->add_sensor_id()->set_value(idSensor1);
    movingObject1b->mutable_base()->mutable_position()->set_x(20);
    movingObject1b->mutable_base()->mutable_position()->set_y(21);

    auto movingObject2b = sensorData.add_moving_object();
    movingObject2b->mutable_header()->add_ground_truth_id()->set_value(idB);
    movingObject2b->mutable_header()->add_sensor_id()->set_value(idSensor2);
    movingObject2b->mutable_base()->mutable_position()->set_x(20);
    movingObject2b->mutable_base()->mutable_position()->set_y(21);
    auto movingObject2c = sensorData.add_moving_object();
    movingObject2c->mutable_header()->add_ground_truth_id()->set_value(idC);
    movingObject2c->mutable_header()->add_sensor_id()->set_value(idSensor2);
    movingObject2c->mutable_base()->mutable_position()->set_x(30);
    movingObject2c->mutable_base()->mutable_position()->set_y(31);

    auto signal = std::make_shared<SensorDataSignal>(sensorData);
    sensorFusion.UpdateInput(0, signal, 0);

    sensorFusion.Trigger(0);

    std::shared_ptr<const SignalInterface> output;
    sensorFusion.UpdateOutput(0, output, 0);
    auto outSensorDataSignal = std::dynamic_pointer_cast<const SensorDataSignal>(output);
    auto outSensorData = outSensorDataSignal->sensorData;

    ASSERT_THAT(outSensorData.moving_object_size(), Eq(3));
    ASSERT_THAT(outSensorData.moving_object(0).header().ground_truth_id(0).value(), Eq(idA));
    ASSERT_THAT(outSensorData.moving_object(0).header().sensor_id_size(), Eq(1));
    ASSERT_THAT(outSensorData.moving_object(0).header().sensor_id(0).value(), Eq(idSensor1));
    ASSERT_THAT(outSensorData.moving_object(0).base().position().x(), Eq(10));
    ASSERT_THAT(outSensorData.moving_object(0).base().position().y(), Eq(11));
    ASSERT_THAT(outSensorData.moving_object(1).header().ground_truth_id(0).value(), Eq(idB));
    ASSERT_THAT(outSensorData.moving_object(1).header().sensor_id_size(), Eq(2));
    ASSERT_THAT(outSensorData.moving_object(1).header().sensor_id(0).value(), Eq(idSensor1));
    ASSERT_THAT(outSensorData.moving_object(1).header().sensor_id(1).value(), Eq(idSensor2));
    ASSERT_THAT(outSensorData.moving_object(1).base().position().x(), Eq(20));
    ASSERT_THAT(outSensorData.moving_object(1).base().position().y(), Eq(21));
    ASSERT_THAT(outSensorData.moving_object(2).header().ground_truth_id(0).value(), Eq(idC));
    ASSERT_THAT(outSensorData.moving_object(2).header().sensor_id_size(), Eq(1));
    ASSERT_THAT(outSensorData.moving_object(2).header().sensor_id(0).value(), Eq(idSensor2));
    ASSERT_THAT(outSensorData.moving_object(2).base().position().x(), Eq(30));
    ASSERT_THAT(outSensorData.moving_object(2).base().position().y(), Eq(31));
}

TEST(SensorFusionErrorless_Tests, SensorDataWithStationaryObjects_IsMergedAppropriately)
{
    NiceMock<FakeAgent> fakeAgent;

    auto sensorFusion = SensorFusionErrorlessImplementation("",
                                                            false,
                                                            0,
                                                            0,
                                                            0,
                                                            100,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            &fakeAgent);
    unsigned int idA = 7;
    unsigned int idB = 8;
    unsigned int idC = 9;

    unsigned int idSensor1 = 101;
    unsigned int idSensor2 = 102;

    osi3::SensorData sensorData;
    auto stationaryObject1a = sensorData.add_stationary_object();
    stationaryObject1a->mutable_header()->add_ground_truth_id()->set_value(idA);
    stationaryObject1a->mutable_header()->add_sensor_id()->set_value(idSensor1);
    stationaryObject1a->mutable_base()->mutable_position()->set_x(10);
    stationaryObject1a->mutable_base()->mutable_position()->set_y(11);
    auto stationaryObject1b = sensorData.add_stationary_object();
    stationaryObject1b->mutable_header()->add_ground_truth_id()->set_value(idB);
    stationaryObject1b->mutable_header()->add_sensor_id()->set_value(idSensor1);
    stationaryObject1b->mutable_base()->mutable_position()->set_x(20);
    stationaryObject1b->mutable_base()->mutable_position()->set_y(21);

    auto stationaryObject2b = sensorData.add_stationary_object();
    stationaryObject2b->mutable_header()->add_ground_truth_id()->set_value(idB);
    stationaryObject2b->mutable_header()->add_sensor_id()->set_value(idSensor2);
    stationaryObject2b->mutable_base()->mutable_position()->set_x(20);
    stationaryObject2b->mutable_base()->mutable_position()->set_y(21);
    auto stationaryObject2c = sensorData.add_stationary_object();
    stationaryObject2c->mutable_header()->add_ground_truth_id()->set_value(idC);
    stationaryObject2c->mutable_header()->add_sensor_id()->set_value(idSensor2);
    stationaryObject2c->mutable_base()->mutable_position()->set_x(30);
    stationaryObject2c->mutable_base()->mutable_position()->set_y(31);

    auto signal = std::make_shared<SensorDataSignal>(sensorData);
    sensorFusion.UpdateInput(0, signal, 0);

    sensorFusion.Trigger(0);

    std::shared_ptr<const SignalInterface> output;
    sensorFusion.UpdateOutput(0, output, 0);
    auto outSensorDataSignal = std::dynamic_pointer_cast<const SensorDataSignal>(output);
    auto outSensorData = outSensorDataSignal->sensorData;

    ASSERT_THAT(outSensorData.stationary_object_size(), Eq(3));
    ASSERT_THAT(outSensorData.stationary_object(0).header().ground_truth_id(0).value(), Eq(idA));
    ASSERT_THAT(outSensorData.stationary_object(0).header().sensor_id_size(), Eq(1));
    ASSERT_THAT(outSensorData.stationary_object(0).header().sensor_id(0).value(), Eq(idSensor1));
    ASSERT_THAT(outSensorData.stationary_object(0).base().position().x(), Eq(10));
    ASSERT_THAT(outSensorData.stationary_object(0).base().position().y(), Eq(11));
    ASSERT_THAT(outSensorData.stationary_object(1).header().ground_truth_id(0).value(), Eq(idB));
    ASSERT_THAT(outSensorData.stationary_object(1).header().sensor_id_size(), Eq(2));
    ASSERT_THAT(outSensorData.stationary_object(1).header().sensor_id(0).value(), Eq(idSensor1));
    ASSERT_THAT(outSensorData.stationary_object(1).header().sensor_id(1).value(), Eq(idSensor2));
    ASSERT_THAT(outSensorData.stationary_object(1).base().position().x(), Eq(20));
    ASSERT_THAT(outSensorData.stationary_object(1).base().position().y(), Eq(21));
    ASSERT_THAT(outSensorData.stationary_object(2).header().ground_truth_id(0).value(), Eq(idC));
    ASSERT_THAT(outSensorData.stationary_object(2).header().sensor_id_size(), Eq(1));
    ASSERT_THAT(outSensorData.stationary_object(2).header().sensor_id(0).value(), Eq(idSensor2));
    ASSERT_THAT(outSensorData.stationary_object(2).base().position().x(), Eq(30));
    ASSERT_THAT(outSensorData.stationary_object(2).base().position().y(), Eq(31));
}

TEST(SensorFusionErrorless_Tests, SensorDataWithSensorView_IsMergedAppropriately)
{
    NiceMock<FakeAgent> fakeAgent;

    auto sensorFusion = SensorFusionErrorlessImplementation("",
                                                            false,
                                                            0,
                                                            0,
                                                            0,
                                                            100,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            nullptr,
                                                            &fakeAgent);
    unsigned int idSensor1 = 101;
    unsigned int idSensor2 = 102;

    osi3::SensorData sensorData;
    auto sensorView1 = sensorData.add_sensor_view();
    auto sensorView2 = sensorData.add_sensor_view();
    sensorView1->mutable_sensor_id()->set_value(idSensor1);
    sensorView2->mutable_sensor_id()->set_value(idSensor2);

    auto signal = std::make_shared<SensorDataSignal>(sensorData);
    sensorFusion.UpdateInput(0, signal, 0);

    sensorFusion.Trigger(0);

    std::shared_ptr<const SignalInterface> output;
    sensorFusion.UpdateOutput(0, output, 0);
    auto outSensorDataSignal = std::dynamic_pointer_cast<const SensorDataSignal>(output);
    auto outSensorData = outSensorDataSignal->sensorData;

    ASSERT_THAT(outSensorData.sensor_view_size(), Eq(2));
    ASSERT_THAT(outSensorData.sensor_view(0).sensor_id().value(), Eq(idSensor1));
    ASSERT_THAT(outSensorData.sensor_view(1).sensor_id().value(), Eq(idSensor2));
}

TEST(SensorFusionErrorless_Tests, ConvertToVehicleCoordinates_SetsCorrectPositionAndOrientation)
{
    NiceMock<FakeAgent> fakeAgent;

    osi3::DetectedMovingObject object;
    object.mutable_base()->mutable_position()->set_x(10.0);
    object.mutable_base()->mutable_position()->set_y(10.0);
    object.mutable_base()->mutable_orientation()->set_yaw(M_PI_4);

    osi3::MountingPosition mountingPosition;
    mountingPosition.mutable_position()->set_x(-2.0);
    mountingPosition.mutable_position()->set_y(1.5);
    mountingPosition.mutable_orientation()->set_yaw(M_PI_2);

    const auto result = SensorFusionHelperFunctions::ConvertToVehicleCoordinates(object, mountingPosition, fakeAgent);

    EXPECT_THAT(result.base().position().x(), DoubleEq(-12.0));
    EXPECT_THAT(result.base().position().y(), DoubleEq(11.5));
    EXPECT_THAT(result.base().orientation().yaw(), DoubleEq(3 * M_PI_4));
}

TEST(SensorFusionErrorless_Tests, ConvertToVehicleCoordinates_SetsCorrectVelocity)
{
    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetVelocity(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::Reference)))
    .WillByDefault(Return(Common::Vector2d{20.0,10.0}));
    ON_CALL(fakeAgent, GetVelocity(VariantWith<ObjectPointCustom>(ObjectPointCustom{-2.0, 1.5})))
    .WillByDefault(Return(Common::Vector2d{20.2,10.1}));
    ON_CALL(fakeAgent, GetYaw()).WillByDefault(Return(-M_PI_2));

    osi3::DetectedMovingObject object;
    object.mutable_base()->mutable_velocity()->set_x(5.0);
    object.mutable_base()->mutable_velocity()->set_y(6.0);

    osi3::MountingPosition mountingPosition;
    mountingPosition.mutable_position()->set_x(-2.0);
    mountingPosition.mutable_position()->set_y(1.5);
    mountingPosition.mutable_orientation()->set_yaw(M_PI_2);

    const auto result = SensorFusionHelperFunctions::ConvertToVehicleCoordinates(object, mountingPosition, fakeAgent);

    EXPECT_THAT(result.base().velocity().x(), DoubleEq(-6.1));
    EXPECT_THAT(result.base().velocity().y(), DoubleEq(5.2));
}

TEST(SensorFusionErrorless_Tests, ConvertToVehicleCoordinates_SetsCorrectAcceleration)
{
    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetAcceleration(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::Reference)))
    .WillByDefault(Return(Common::Vector2d{20.0,10.0}));
    ON_CALL(fakeAgent, GetAcceleration(VariantWith<ObjectPointCustom>(ObjectPointCustom{-2.0, 1.5})))
    .WillByDefault(Return(Common::Vector2d{20.2,10.1}));
    ON_CALL(fakeAgent, GetYaw()).WillByDefault(Return(-M_PI_2));

    osi3::DetectedMovingObject object;
    object.mutable_base()->mutable_acceleration()->set_x(5.0);
    object.mutable_base()->mutable_acceleration()->set_y(6.0);

    osi3::MountingPosition mountingPosition;
    mountingPosition.mutable_position()->set_x(-2.0);
    mountingPosition.mutable_position()->set_y(1.5);
    mountingPosition.mutable_orientation()->set_yaw(M_PI_2);

    const auto result = SensorFusionHelperFunctions::ConvertToVehicleCoordinates(object, mountingPosition, fakeAgent);

    EXPECT_THAT(result.base().acceleration().x(), DoubleEq(-6.1));
    EXPECT_THAT(result.base().acceleration().y(), DoubleEq(5.2));
}