/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
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

#include "sensorAggregationImpl.h"
#include "common/sensorDataSignal.h"
#include "osi3/osi_sensordata.pb.h"

using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Return;

TEST(SensorAggregationOSI_Unittest, TestAppendingDetectedObjectsWithinTheSameTimestamp)
{
    osi3::SensorData sensorData1;
    auto movingObject1 = sensorData1.add_moving_object();
    movingObject1->mutable_header()->add_sensor_id()->set_value(1);
    movingObject1->mutable_header()->add_ground_truth_id()->set_value(10);
    auto movingObject2 = sensorData1.add_moving_object();
    movingObject2->mutable_header()->add_sensor_id()->set_value(1);
    movingObject2->mutable_header()->add_ground_truth_id()->set_value(11);

    osi3::SensorData sensorData2;
    auto movingObject3 = sensorData2.add_moving_object();
    movingObject3->mutable_header()->add_sensor_id()->set_value(2);
    movingObject3->mutable_header()->add_ground_truth_id()->set_value(12);

    std::shared_ptr<SensorDataSignal> fakeSignal1 = std::make_shared<SensorDataSignal>(sensorData1);
    std::shared_ptr<SensorDataSignal> fakeSignal2 = std::make_shared<SensorDataSignal>(sensorData2);

    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(0));

    SensorAggregationImplementation sensorFusion("",
                                            false,
                                            0,
                                            0,
                                            0,
                                            0,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            &fakeAgent);

    sensorFusion.UpdateInput(0, fakeSignal1, 100);
    sensorFusion.UpdateInput(1, fakeSignal2, 100);

    std::shared_ptr<SignalInterface const> response;
    sensorFusion.UpdateOutput(0, response, 100);

    const std::shared_ptr<SensorDataSignal const> result = std::dynamic_pointer_cast<SensorDataSignal const>(response);
    auto resultSensorData = result->sensorData;
    ASSERT_THAT(resultSensorData.moving_object_size(), Eq(3));
    auto resultMovingObjects = resultSensorData.moving_object();
    ASSERT_THAT(resultMovingObjects.Get(0).header().sensor_id(0).value(), Eq(1));
    ASSERT_THAT(resultMovingObjects.Get(0).header().ground_truth_id(0).value(), Eq(10));
    ASSERT_THAT(resultMovingObjects.Get(1).header().sensor_id(0).value(), Eq(1));
    ASSERT_THAT(resultMovingObjects.Get(1).header().ground_truth_id(0).value(), Eq(11));
    ASSERT_THAT(resultMovingObjects.Get(2).header().sensor_id(0).value(), Eq(2));
    ASSERT_THAT(resultMovingObjects.Get(2).header().ground_truth_id(0).value(), Eq(12));
}

TEST(SensorAggregationOSI_Unittest, TestResettingDetectedObjectsInNewTimestamp)
{
    osi3::SensorData sensorData1;
    auto movingObject1 = sensorData1.add_moving_object();
    movingObject1->mutable_header()->add_sensor_id()->set_value(1);
    movingObject1->mutable_header()->add_ground_truth_id()->set_value(10);
    auto movingObject2 = sensorData1.add_moving_object();
    movingObject2->mutable_header()->add_sensor_id()->set_value(1);
    movingObject2->mutable_header()->add_ground_truth_id()->set_value(11);

    osi3::SensorData sensorData2;
    auto movingObject3 = sensorData2.add_moving_object();
    movingObject3->mutable_header()->add_sensor_id()->set_value(2);
    movingObject3->mutable_header()->add_ground_truth_id()->set_value(12);

    std::shared_ptr<SensorDataSignal> fakeSignal1 = std::make_shared<SensorDataSignal>(sensorData1);
    std::shared_ptr<SensorDataSignal> fakeSignal2 = std::make_shared<SensorDataSignal>(sensorData2);

    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(0));

    SensorAggregationImplementation sensorFusion("",
                                            false,
                                            0,
                                            0,
                                            0,
                                            0,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            &fakeAgent);

    sensorFusion.UpdateInput(0, fakeSignal1, 100);
    sensorFusion.UpdateInput(1, fakeSignal2, 200);

    std::shared_ptr<SignalInterface const> response;
    sensorFusion.UpdateOutput(0, response, 200);

    const std::shared_ptr<SensorDataSignal const> result = std::dynamic_pointer_cast<SensorDataSignal const>(response);
    auto resultSensorData = result->sensorData;
    ASSERT_THAT(resultSensorData.moving_object_size(), Eq(1));
    auto resultMovingObjects = resultSensorData.moving_object();
    ASSERT_THAT(resultMovingObjects.Get(0).header().sensor_id(0).value(), Eq(2));
    ASSERT_THAT(resultMovingObjects.Get(0).header().ground_truth_id(0).value(), Eq(12));
}

TEST(SensorAggregationOSI_Unittest, TestAppendingSensorViewsWithinTheSameTimestamp)
{
    unsigned int idSensor1 = 101;
    unsigned int idSensor2 = 102;

    osi3::SensorData sensorData1;
    osi3::SensorData sensorData2;
    auto sensorView1 = sensorData1.add_sensor_view();
    auto sensorView2 = sensorData2.add_sensor_view();
    sensorView1->mutable_sensor_id()->set_value(idSensor1);
    sensorView2->mutable_sensor_id()->set_value(idSensor2);

    std::shared_ptr<SensorDataSignal> fakeSignal1 = std::make_shared<SensorDataSignal>(sensorData1);
    std::shared_ptr<SensorDataSignal> fakeSignal2 = std::make_shared<SensorDataSignal>(sensorData2);

    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(0));

    SensorAggregationImplementation sensorFusion("",
                                            false,
                                            0,
                                            0,
                                            0,
                                            0,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            &fakeAgent);

    sensorFusion.UpdateInput(0, fakeSignal1, 100);
    sensorFusion.UpdateInput(1, fakeSignal2, 100);

    std::shared_ptr<SignalInterface const> response;
    sensorFusion.UpdateOutput(0, response, 100);

    const std::shared_ptr<SensorDataSignal const> result = std::dynamic_pointer_cast<SensorDataSignal const>(response);
    auto resultSensorData = result->sensorData;
    ASSERT_THAT(resultSensorData.sensor_view_size(), Eq(2));
    ASSERT_THAT(resultSensorData.sensor_view(0).sensor_id().value(), Eq(idSensor1));
    ASSERT_THAT(resultSensorData.sensor_view(1).sensor_id().value(), Eq(idSensor2));
}

TEST(SensorAggregationOSI_Unittest, TestResettingSensorViewsInNewTimestamp)
{
    unsigned int idSensor1 = 101;
    unsigned int idSensor2 = 102;

    osi3::SensorData sensorData1;
    osi3::SensorData sensorData2;
    auto sensorView1 = sensorData1.add_sensor_view();
    auto sensorView2 = sensorData2.add_sensor_view();
    sensorView1->mutable_sensor_id()->set_value(idSensor1);
    sensorView2->mutable_sensor_id()->set_value(idSensor2);

    std::shared_ptr<SensorDataSignal> fakeSignal1 = std::make_shared<SensorDataSignal>(sensorData1);
    std::shared_ptr<SensorDataSignal> fakeSignal2 = std::make_shared<SensorDataSignal>(sensorData2);

    NiceMock<FakeAgent> fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(0));

    SensorAggregationImplementation sensorFusion("",
                                            false,
                                            0,
                                            0,
                                            0,
                                            0,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            &fakeAgent);

    sensorFusion.UpdateInput(0, fakeSignal1, 100);
    sensorFusion.UpdateInput(1, fakeSignal2, 200);

    std::shared_ptr<SignalInterface const> response;
    sensorFusion.UpdateOutput(0, response, 200);

    const std::shared_ptr<SensorDataSignal const> result = std::dynamic_pointer_cast<SensorDataSignal const>(response);
    auto resultSensorData = result->sensorData;
    ASSERT_THAT(resultSensorData.sensor_view_size(), Eq(1));
    ASSERT_THAT(resultSensorData.sensor_view(0).sensor_id().value(), Eq(idSensor2));
}
