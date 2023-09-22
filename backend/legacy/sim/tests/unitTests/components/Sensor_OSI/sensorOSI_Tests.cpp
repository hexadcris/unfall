/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <QtGlobal>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgent.h"
#include "fakePublisher.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"
#include "fakeWorldData.h"

#include "sensorGeometric2D.h"
#include "sensorOSI_TestsCommon.h"
#include "common/boostGeometryCommon.h"

using ::testing::_;
using ::testing::Contains;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StrEq;
using ::testing::Ne;
using ::testing::VariantWith;

class DetectObjects_Data
{
public:
    double detectionRange;
    double openingAngleH;
    double mountingPositionLongitudinal;
    double mountingPositionLateral;
    double mountingPositionYaw;
    double sensorX;
    double sensorY;
    double vehicleYaw;
    bool   enableVisualObstruction;
    std::vector<MovingObjectParameter> movingObjects;
    std::vector<StationaryObjectParameter> stationaryObjects;

    std::vector<MovingObjectParameter> expectedMovingObjects;
    std::vector<StationaryObjectParameter> expectedStationaryObjects;

    std::vector<OWL::Id> expectedVisibleMovingObjectIds;
    std::vector<OWL::Id> expectedDetectedMovingObjectIds;
};

class DetectObjects : public::testing::TestWithParam<DetectObjects_Data>
{
public:
    DetectObjects()
    {
        ON_CALL(fakeStochastics, GetUniformDistributed(_, _)).WillByDefault(Return(1));
        ON_CALL(fakeStochastics, GetLogNormalDistributed(_, _)).WillByDefault(Return(1));

        fakeDoubles = {{"FailureProbability", 0}, {"Latency", 0},
                       {"RequiredPercentageOfVisibleArea", 0.4},{"Height", 0.0},
                       {"Pitch", 0.0}, {"Roll", 0.0}};

        ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

        fakeInts = {{"Id", 0}};
        ON_CALL(fakeParameters, GetParametersInt()).WillByDefault(ReturnRef(fakeInts));

        fakeBools = {};
        ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

        ON_CALL(fakeWorldInterface, GetWorldData()).WillByDefault(Return(&fakeWorldData));

    }

    OWL::SensorView_ptr sensorView = OWL::SensorView_ptr(new osi3::SensorView());
    NiceMock<FakeStochastics> fakeStochastics;
    NiceMock<FakeParameter> fakeParameters;
    NiceMock<FakePublisher> fakePublisher;
    NiceMock<FakeWorld> fakeWorldInterface;
    NiceMock<OWL::Fakes::WorldData> fakeWorldData;
    NiceMock<FakeAgent> fakeAgent;
    std::map<std::string, double> fakeDoubles;
    std::map<std::string, int> fakeInts;
    std::map<std::string, bool> fakeBools;
};

void AddMovingObjectToSensorView (osi3::SensorView &sensorView, MovingObjectParameter &objectParameter)
{
    osi3::MovingObject* movingObject = sensorView.mutable_global_ground_truth()->add_moving_object();
    movingObject->mutable_base()->mutable_position()->set_x(objectParameter.position.x);
    movingObject->mutable_base()->mutable_position()->set_y(objectParameter.position.y);
    movingObject->mutable_base()->mutable_dimension()->set_length(5);
    movingObject->mutable_base()->mutable_dimension()->set_width(2);
    movingObject->mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->set_x(-0.5);
    movingObject->mutable_base()->mutable_orientation()->set_yaw(objectParameter.yaw);
    movingObject->mutable_base()->mutable_velocity()->set_x(objectParameter.velocity.x);
    movingObject->mutable_base()->mutable_velocity()->set_y(objectParameter.velocity.y);
    movingObject->mutable_base()->mutable_acceleration()->set_x(objectParameter.acceleration.x);
    movingObject->mutable_base()->mutable_acceleration()->set_y(objectParameter.acceleration.y);
    movingObject->mutable_id()->set_value(objectParameter.id);
}

void AddStationaryObjectToSensorView (osi3::SensorView &sensorView, StationaryObjectParameter &objectParameter)
{
    osi3::StationaryObject* stationaryObject = sensorView.mutable_global_ground_truth()->add_stationary_object();
    stationaryObject->mutable_base()->mutable_position()->set_x(objectParameter.position.x);
    stationaryObject->mutable_base()->mutable_position()->set_y(objectParameter.position.y);
    stationaryObject->mutable_base()->mutable_dimension()->set_length(5);
    stationaryObject->mutable_base()->mutable_dimension()->set_width(2);
    stationaryObject->mutable_base()->mutable_orientation()->set_yaw(objectParameter.yaw);
    stationaryObject->mutable_id()->set_value(objectParameter.id);
}

TEST_P(DetectObjects, StoresSensorDataWithDetectedObjects)
{
    auto data = GetParam();
    fakeDoubles["DetectionRange"] = data.detectionRange;
    fakeDoubles["OpeningAngleH"] = data.openingAngleH;
    fakeDoubles["Longitudinal"] = data.mountingPositionLongitudinal;
    fakeDoubles["Lateral"] = data.mountingPositionLateral;
    fakeDoubles["Yaw"] = data.mountingPositionYaw;
    fakeBools["EnableVisualObstruction"] = data.enableVisualObstruction;
    sensorView->mutable_host_vehicle_id()->set_value(1);
    const ObjectPointCustom mountingPosition{data.mountingPositionLongitudinal, data.mountingPositionLateral};
    MovingObjectParameter hostVehicle{1, {0, 0}, {9.0, 4.0}, {-2.1, 3.1}, data.vehicleYaw};
    AddMovingObjectToSensorView(*sensorView, hostVehicle);
    ON_CALL(fakeAgent, GetAbsolutePosition(VariantWith<ObjectPointCustom>(mountingPosition))).WillByDefault(Return(Common::Vector2d{data.sensorX, data.sensorY}));
    ON_CALL(fakeAgent, GetVelocity(VariantWith<ObjectPointCustom>(mountingPosition))).WillByDefault(Return(Common::Vector2d{10.0, 5.0}));
    ON_CALL(fakeAgent, GetAcceleration(VariantWith<ObjectPointCustom>(mountingPosition))).WillByDefault(Return(Common::Vector2d{-2.0, 3.0}));
    for (auto object : data.movingObjects)
    {
        AddMovingObjectToSensorView(*sensorView, object);
    }
    for (auto object : data.stationaryObjects)
    {
        AddStationaryObjectToSensorView(*sensorView, object);
    }
    ON_CALL(fakeWorldData, GetSensorView(_,_)).WillByDefault([this](auto, auto){return std::move(sensorView);}); //test::Return does not work with unique pointer
    SensorGeometric2D sensor(
                "",
                false,
                0,
                0,
                0,
                0,
                &fakeStochastics,
                &fakeWorldInterface,
                &fakeParameters,
                &fakePublisher,
                nullptr,
                &fakeAgent);

    SensorDetectionResults results = sensor.DetectObjects();

    const osi3::SensorData& sensorData = sensor.getSensorData();
    ASSERT_THAT(sensorData.moving_object_size(), Eq(data.expectedMovingObjects.size()));
    ASSERT_THAT(sensorData.stationary_object_size(), Eq(data.expectedStationaryObjects.size()));
    for (auto & movingObject : sensorData.moving_object())
    {
        ASSERT_THAT(data.expectedMovingObjects, Contains(movingObject));
    }
    for (auto & stationaryObject : sensorData.stationary_object())
    {
        ASSERT_THAT(data.expectedStationaryObjects, Contains(stationaryObject));
    }

    for(const auto id : data.expectedVisibleMovingObjectIds)
    {
        const auto iterToMatchingId = std::find_if(results.visibleMovingObjects.begin(),
                                                   results.visibleMovingObjects.end(),
                                                   [id](const auto object) -> bool
        {
            return object.id().value() == id;
        });

        ASSERT_THAT(iterToMatchingId, Ne(results.visibleMovingObjects.end()));
    }
    for (const auto id : data.expectedDetectedMovingObjectIds)
    {
        const auto iterToMatchingId = std::find_if(results.detectedMovingObjects.begin(),
                                                   results.detectedMovingObjects.end(),
                                                   [id](const auto object) -> bool
        {
            return object.id().value() == id;
        });

        ASSERT_THAT(iterToMatchingId, Ne(results.detectedMovingObjects.end()));
    }
}

MovingObjectParameter testMovingObject2{2, {110.0, 100.0}, {5.0, 7.0}, {-0.2, 0.3}, 0.5};
MovingObjectParameter testMovingObject3{3, {150.0, 54.0}, {6.0, 8.0}, {0.0, 0.0}, 0.0};
MovingObjectParameter testMovingObject4{4, {130.0, 403.0}, {7.0, 9.0}, {-0.1, 0.4}, 0.0};
MovingObjectParameter testMovingObject5{5, {101.0, 130.0}, {8.0, 10.0}, {-0.3, 0.5}, 0.0};
MovingObjectParameter testMovingObject6{6, {70.0, 134.0}, {9.0, 11.0}, {-0.3, 0.5}, -0.5};
MovingObjectParameter testMovingObject7{7, {0.0, 50.0}, {10.0, 12.0}, {-0.2, 0.2}, 0.0};
std::vector<MovingObjectParameter> testMovingObjects{testMovingObject2, testMovingObject3, testMovingObject4, testMovingObject5, testMovingObject6, testMovingObject7};

MovingObjectParameter expectedMovingObject2_a{2, {10.0, 0.0}, {-5.0, 2.0}, {1.8, -2.7}, 0.5};
MovingObjectParameter expectedMovingObject3_a{3, {50.0, -46.0}, {-4.0, 3.0}, {2.0, -3.0}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjects_a{expectedMovingObject2_a, expectedMovingObject3_a};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_a{expectedMovingObject2_a.id, expectedMovingObject3_a.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_a{expectedMovingObject2_a.id, expectedMovingObject3_a.id};

MovingObjectParameter expectedMovingObject5_b{5, {1.0, 30.0}, {-2.0, 5.0}, {1.7, -2.5}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjects_b{expectedMovingObject2_a, expectedMovingObject3_a, expectedMovingObject5_b};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_b{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_b{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id};

MovingObjectParameter expectedMovingObject6_c{6, {-30.0, 34.0}, {-1.0, 6.0}, {1.7, -2.5}, -0.5};
std::vector<MovingObjectParameter> expectedMovingObjects_c{expectedMovingObject2_a, expectedMovingObject3_a, expectedMovingObject5_b, expectedMovingObject6_c};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_c{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id, expectedMovingObject6_c.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_c{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id, expectedMovingObject6_c.id};

MovingObjectParameter expectedMovingObject7_d{7, {-100, -50.0}, {0.0, 7.0}, {1.8, -2.8}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjects_d{expectedMovingObject2_a, expectedMovingObject3_a, expectedMovingObject5_b, expectedMovingObject6_c, expectedMovingObject7_d};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_d{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id, expectedMovingObject6_c.id, expectedMovingObject7_d.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_d{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id, expectedMovingObject6_c.id, expectedMovingObject7_d.id};

std::vector<MovingObjectParameter> expectedMovingObjects_e{expectedMovingObject2_a, expectedMovingObject3_a};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_e{expectedMovingObject2_a.id, expectedMovingObject3_a.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_e{expectedMovingObject2_a.id, expectedMovingObject3_a.id};

MovingObjectParameter expectedMovingObject7_f{7, {30.0, 0.0}, {7.0, 0.0}, {-2.8, -1.8}, -M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjects_f{expectedMovingObject7_f};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_f{expectedMovingObject7_f.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_f{expectedMovingObject7_f.id};

StationaryObjectParameter testStationaryObject12{12, {110.0, 100.0}, 0.5};
StationaryObjectParameter testStationaryObject13{13, {150.0, 54.0}, 0.0};
StationaryObjectParameter testStationaryObject14{14, {130.0, 403.0}, 0.0};
StationaryObjectParameter testStationaryObject15{15, {101.0, 130.0}, 0.0};
StationaryObjectParameter testStationaryObject16{16, {70.0, 134.0}, -0.5};
StationaryObjectParameter testStationaryObject17{17, {0.0, 50.0}, 0.0};
std::vector<StationaryObjectParameter> testStationaryObjects{testStationaryObject12, testStationaryObject13, testStationaryObject14, testStationaryObject15, testStationaryObject16, testStationaryObject17};

StationaryObjectParameter expectedStationaryObject12_a{12, {10.0, 0.0}, 0.5};
StationaryObjectParameter expectedStationaryObject13_a{13, {50.0, -46.0}, 0.0};
std::vector<StationaryObjectParameter> expectedStationaryObjects_a{expectedStationaryObject12_a, expectedStationaryObject13_a};
StationaryObjectParameter expectedStationaryObject15_b{15, {1.0, 30.0}, 0.0};
std::vector<StationaryObjectParameter> expectedStationaryObjects_b{expectedStationaryObject12_a, expectedStationaryObject13_a, expectedStationaryObject15_b};
StationaryObjectParameter expectedStationaryObject16_c{16, {-30.0, 34.0}, -0.5};
std::vector<StationaryObjectParameter> expectedStationaryObjects_c{expectedStationaryObject12_a, expectedStationaryObject13_a, expectedStationaryObject15_b, expectedStationaryObject16_c};
StationaryObjectParameter expectedStationaryObject17_d{17, {-100.0, -50.0}, 0.0};
std::vector<StationaryObjectParameter> expectedStationaryObjects_d{expectedStationaryObject12_a, expectedStationaryObject13_a, expectedStationaryObject15_b, expectedStationaryObject16_c, expectedStationaryObject17_d};
std::vector<StationaryObjectParameter> expectedStationaryObjects_e{expectedStationaryObject12_a, expectedStationaryObject13_a};
StationaryObjectParameter expectedStationaryObject17_f{17, {30.0, 0.0}, -M_PI * 0.5};
std::vector<StationaryObjectParameter> expectedStationaryObjects_f{expectedStationaryObject17_f};

INSTANTIATE_TEST_CASE_P(WithoutObstruction, DetectObjects,
    ::testing::Values(
//                                 openingAngle   lateral        positionX    vehicleYaw                 movingObjects                           expectedMovingObjects
//                         range       | longitudinal|      sensorYaw | positionY  |     visualObstruction    |            stationaryObjects              |    expectedStationaryObjects
//                           |         |       |     |          |     |     |      |             |            |                   |                       |               |
        DetectObjects_Data{0.0  , M_PI * 0.5, 0.0, 0.0,         0.0, 100.0, 100.0, 0.0,       false, testMovingObjects, testStationaryObjects, {},                      {},                          {}, {}}, //zero range => no detected objects
        DetectObjects_Data{300.0, M_PI * 0.5, 0.0, 0.0,         0.0, 100.0, 100.0, 0.0,        false, {}               , {},                    {},                      {},                          {}, {}}, //no other objects beside host
        DetectObjects_Data{300.0, M_PI * 0.5, 2.0, 3.0,         0.0, 100.0, 100.0, 0.0,        false, testMovingObjects, testStationaryObjects, expectedMovingObjects_a, expectedStationaryObjects_a, expectedMovingObjectsVisibleIds_a, expectedMovingObjectsDetectedIds_a}, //opening angle small
        DetectObjects_Data{300.0, M_PI      , 2.0, 3.0,         0.0, 100.0, 100.0, 0.0,        false, testMovingObjects, testStationaryObjects, expectedMovingObjects_b, expectedStationaryObjects_b, expectedMovingObjectsVisibleIds_b, expectedMovingObjectsDetectedIds_b}, //half circle
        DetectObjects_Data{300.0, M_PI * 1.5, 2.0, 3.0,         0.0, 100.0, 100.0, 0.0,        false, testMovingObjects, testStationaryObjects, expectedMovingObjects_c, expectedStationaryObjects_c, expectedMovingObjectsVisibleIds_c, expectedMovingObjectsDetectedIds_c}, //opening angle big
        DetectObjects_Data{300.0, M_PI * 2.0, 2.0, 3.0,         0.0, 100.0, 100.0, 0.0,        false, testMovingObjects, testStationaryObjects, expectedMovingObjects_d, expectedStationaryObjects_d, expectedMovingObjectsVisibleIds_d, expectedMovingObjectsDetectedIds_d}, //full circle
        DetectObjects_Data{300.0, M_PI * 0.5, 2.0, 3.0, -M_PI * 0.5, 100.0, 100.0, M_PI * 0.5, false, testMovingObjects, testStationaryObjects, expectedMovingObjects_e, expectedStationaryObjects_e, expectedMovingObjectsVisibleIds_e, expectedMovingObjectsDetectedIds_e}, //vehicle is rotated
        DetectObjects_Data{30.5, M_PI * 0.5, 10.0, -10.0,       0.0,   0.0,  20.0, M_PI * 0.5, false, testMovingObjects, testStationaryObjects, expectedMovingObjects_f, expectedStationaryObjects_f, expectedMovingObjectsVisibleIds_f, expectedMovingObjectsDetectedIds_f}) //vehicle is pointing upwards, test correct sensorPosition
);

//Obstruction Tests (RequiredPercentageOfVisibleArea = 40%)

//Test A
//Object 3 is behind Object 2, Object 5 is only partially inside detection field
MovingObjectParameter testMovingObjectObstruction2{2, {50.0, 0.0}, {5.0, 7.0}, {-0.2, 0.3}, 0.0};
MovingObjectParameter testMovingObjectObstruction3{3, {100.0, 0.0}, {6.0, 8.0}, {-0.3, 0.4}, 0.0};// shadowed by 2
MovingObjectParameter testMovingObjectObstruction4{4, {50.0, 10.0}, {7.0, 9.0}, {-0.4, 0.5}, 0.0};
MovingObjectParameter testMovingObjectObstruction5{5, {50.0, 51.0}, {8.0, 10.0}, {-0.4, 0.5}, 0.0};// only 30% inside detection field
std::vector<MovingObjectParameter> testMovingObjectsObstruction_a{testMovingObjectObstruction2, testMovingObjectObstruction3, testMovingObjectObstruction4, testMovingObjectObstruction5};
MovingObjectParameter expectedMovingObjectObstruction2{2, {50.0, 0.0}, {-5.0, 2.0}, {1.8, -2.7}, 0.0};
MovingObjectParameter expectedMovingObjectObstruction4{4, {50.0, 10.0}, {-3.0, 4.0}, {1.6, -2.5}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_a{expectedMovingObjectObstruction2, expectedMovingObjectObstruction4};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_a{testMovingObjectObstruction2.id, testMovingObjectObstruction4.id, testMovingObjectObstruction5.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_a{testMovingObjectObstruction2.id, testMovingObjectObstruction4.id};

//Test B
//Like Test A but with StationaryObjects as Objects 2 and 5
StationaryObjectParameter testStationaryObjectObstruction2{2, {50.0, 0.0}, 0.0};
StationaryObjectParameter testStationaryObjectObstruction5{4, {50.0, 51.0}, 0.0};// only 30% inside detection field
std::vector<MovingObjectParameter> testMovingObjectsObstruction_b{testMovingObjectObstruction3, testMovingObjectObstruction4};
std::vector<StationaryObjectParameter> testStationaryObjectsObstruction_b{testStationaryObjectObstruction2, testStationaryObjectObstruction5};
StationaryObjectParameter expectedStationaryObjectObstruction2{2, {50.0, 0.0}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_b{ expectedMovingObjectObstruction4};
std::vector<StationaryObjectParameter> expectedStationaryObjectsObstruction_b{expectedStationaryObjectObstruction2};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_b{testMovingObjectObstruction4.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_b{testMovingObjectObstruction4.id};

//Test C
//Objects rotated; Object 7 is 50% shadowed by Object 6
MovingObjectParameter testMovingObjectObstruction6{6, {50.0, 2.5}, {5.0, 7.0}, {-0.2, 0.3}, M_PI * 0.5};
MovingObjectParameter testMovingObjectObstruction7{7, {70.0, 0.0}, {6.0, 8.0}, {-0.2, 0.3}, M_PI * 0.5};
std::vector<MovingObjectParameter> testMovingObjectsObstruction_c{testMovingObjectObstruction6, testMovingObjectObstruction7};
MovingObjectParameter expectedMovingObjectObstruction6{6, {50.0, 2.5}, {-5.0, 2.0}, {1.8, -2.7}, M_PI * 0.5};
MovingObjectParameter expectedMovingObjectObstruction7{7, {70.0, 0.0}, {-4.0, 3.0}, {1.8, -2.7}, M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_c{expectedMovingObjectObstruction6, expectedMovingObjectObstruction7};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_c{testMovingObjectObstruction6.id, testMovingObjectObstruction7.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_c{testMovingObjectObstruction6.id, testMovingObjectObstruction7.id};

//Test D
//Objects rotated and half circle; Object 9 is 70% shadowed by Object 8
MovingObjectParameter testMovingObjectObstruction8{8, {11.0, 7.5}, {5.0, 7.0}, {-0.2, 0.3}, M_PI * 0.5};
MovingObjectParameter testMovingObjectObstruction9{9, {101.0, 100.0}, {6.0, 8.0}, {-0.2, 0.3}, M_PI * 0.5};
std::vector<MovingObjectParameter> testMovingObjectsObstruction_d{testMovingObjectObstruction8, testMovingObjectObstruction9};
MovingObjectParameter expectedMovingObjectObstruction8{8, {11.0, 7.5}, {-5.0, 2.0}, {1.8, -2.7}, M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_d{expectedMovingObjectObstruction8};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_d{testMovingObjectObstruction8.id, testMovingObjectObstruction9.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_d{testMovingObjectObstruction8.id};

//Test E
//Objects 10 and 11 partially shadow Object 12 with less then 40% but together with more than 70%
MovingObjectParameter testMovingObjectObstruction10{10, {97.0, 3.0}, {5.0, 7.0}, {-0.2, 0.3}, M_PI * 0.5};
MovingObjectParameter testMovingObjectObstruction11{11, {97.0, -3.0}, {6.0, 8.0}, {-0.2, 0.3}, M_PI * 0.5};
MovingObjectParameter testMovingObjectObstruction12{12, {100.0, 0.0}, {6.0, 8.0}, {-0.2, 0.3}, M_PI * 0.5};
std::vector<MovingObjectParameter> testMovingObjectsObstruction_e{testMovingObjectObstruction10, testMovingObjectObstruction11, testMovingObjectObstruction12};
MovingObjectParameter expectedMovingObjectObstruction10{10, {97.0, 3.0}, {-5.0, 2.0}, {1.8, -2.7}, M_PI * 0.5};
MovingObjectParameter expectedMovingObjectObstruction11{11, {97.0, -3.0}, {-4.0, 3.0}, {1.8, -2.7}, M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_e{expectedMovingObjectObstruction10, expectedMovingObjectObstruction11};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_e{testMovingObjectObstruction10.id, testMovingObjectObstruction11.id, testMovingObjectObstruction12.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_e{testMovingObjectObstruction10.id, testMovingObjectObstruction11.id};

INSTANTIATE_TEST_CASE_P(WithObstruction, DetectObjects,
    ::testing::Values(
//                                openingAngle   lateral   positionX vehicleYaw           movingObjects                                      expectedMovingObjects
//                         range      | longitudinal|   yaw   |position | visualObstruction    |            stationaryObjects                         |                       expectedStationaryObjects
//                           |        |        |    |    |    |    |    |    |                 |                   |                                  |                                    |
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, {}                            , {},                                 {},                                 {}, {}, {}}, //no other objects beside host
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_a, {},                                 expectedMovingObjectsObstruction_a, {}, expectedVisibleMovingObjectIdsObstruction_a, expectedDetectedMovingObjectIdsObstruction_a}, //Test A
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_b, testStationaryObjectsObstruction_b, expectedMovingObjectsObstruction_b, expectedStationaryObjectsObstruction_b, expectedVisibleMovingObjectIdsObstruction_b, expectedDetectedMovingObjectIdsObstruction_b}, //Test B
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_c, {},                                 expectedMovingObjectsObstruction_c, {}, expectedVisibleMovingObjectIdsObstruction_c, expectedDetectedMovingObjectIdsObstruction_c}, //Test C
        DetectObjects_Data{300.0, M_PI * 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_d, {},                                 expectedMovingObjectsObstruction_d, {}, expectedVisibleMovingObjectIdsObstruction_d, expectedDetectedMovingObjectIdsObstruction_d}, //Test D
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_e, {},                                 expectedMovingObjectsObstruction_e, {}, expectedVisibleMovingObjectIdsObstruction_e, expectedDetectedMovingObjectIdsObstruction_e}) //Test E
);

// This test should be enabled as soon as the mounting position of the sensor is taken into account of all readings in SensorData
TEST_F(DetectObjects, DISABLED_CompareMovingObjectsWithMountingPosition)
{
    //auto data = GetParam();

    auto sensor_mounting_Yaw = M_PI / 4.0;
    auto sensor_mounting_posOffX = - .25;
    auto sensor_mounting_posOffY = - .2;

    fakeDoubles["DetectionRange"] = 300.0;
    fakeDoubles["OpeningAngleH"] = M_PI * 2.0;
    fakeDoubles["Longitudinal"] = sensor_mounting_posOffX;
    fakeDoubles["Lateral"] = sensor_mounting_posOffY;
    fakeDoubles["Yaw"] = sensor_mounting_Yaw;
    fakeBools["EnableVisualObstruction"] = false;
    sensorView->mutable_host_vehicle_id()->set_value(1);

    auto yawZero = 0.0;
    auto YawOrtho = M_PI / 2.0;
    auto YawOpposing = M_PI;

    std::vector<MovingObjectParameter> expectedMovingObjects {};
    std::vector<StationaryObjectParameter> expectedStationaryObjects {};

    MovingObjectParameter hostVehicle{1, {0.0, 0.0}, {0.0, 0.0}, {0, 0}, yawZero};
    AddMovingObjectToSensorView(*sensorView, hostVehicle);

    MovingObjectParameter orthoVehicle{2, {2.0, 2.0}, {0.0, 0.0}, {0, 0}, YawOrtho};
    MovingObjectParameter opposingVehicle{3, {8.0, 0.0}, {0.0, 0.0}, {0, 0}, YawOpposing};
    AddMovingObjectToSensorView(*sensorView, orthoVehicle);
    AddMovingObjectToSensorView(*sensorView, opposingVehicle);
    expectedMovingObjects.emplace_back(orthoVehicle);
    expectedMovingObjects.emplace_back(opposingVehicle);

    StationaryObjectParameter orthoObject {10, {2,-2}, YawOrtho};
    AddStationaryObjectToSensorView(*sensorView, orthoObject);
    expectedStationaryObjects.emplace_back(orthoObject);

    sensorView->mutable_mounting_position()->mutable_orientation()->set_yaw( sensor_mounting_Yaw);
    sensorView->mutable_mounting_position()->mutable_position()->set_x( sensor_mounting_posOffX);
    sensorView->mutable_mounting_position()->mutable_position()->set_y( sensor_mounting_posOffY);
    sensorView->mutable_mounting_position()->mutable_position()->set_z( 0.0);
    ASSERT_EQ(sensorView->has_mounting_position(), true);

    ::osi3::GroundTruth* groundTruth = sensorView->mutable_global_ground_truth();

    ON_CALL(fakeWorldData, GetSensorView(_,_)).WillByDefault([this](auto, auto){return std::move(sensorView);}); //test::Return does not work with unique pointer
    SensorGeometric2D sensor(
        "",
        false,
        0,
        0,
        0,
        0,
        &fakeStochastics,
        &fakeWorldInterface,
        &fakeParameters,
        &fakePublisher,
        nullptr,
        &fakeAgent);

    SensorDetectionResults results = sensor.DetectObjects();

    const osi3::SensorData& sensorData = sensor.getSensorData();
    ASSERT_THAT(sensorData.moving_object_size(), 2);
    ASSERT_THAT(sensorData.stationary_object_size(),  1);

    auto world_yawDelta_Ortho = YawOrtho - yawZero;
    auto world_yawDelta_Opposite = YawOpposing - yawZero;
    auto movingHost_yaw = sensorData.host_vehicle_location().orientation().yaw();

    //sensorData
    auto sensorData_m0_yaw_ortho = sensorData.moving_object(0).base().orientation().yaw();
    auto sensorData_m1_yaw_opposite = sensorData.moving_object(1).base().orientation().yaw();
    auto sensorData_s0_yaw_ortho = sensorData.moving_object(0).base().orientation().yaw();

    auto sensorData_m0_pos_X = sensorData.moving_object(0).base().position().x();
    auto sensorData_m1_pos_X = sensorData.moving_object(1).base().position().x();
    auto sensorData_s0_pos_X = sensorData.moving_object(0).base().position().x();

    auto sensorData_m0_pos_Y = sensorData.moving_object(0).base().position().y();
    auto sensorData_m1_pos_Y = sensorData.moving_object(1).base().position().y();
    auto sensorData_s0_pos_Y = sensorData.moving_object(0).base().position().y();

    auto sensorData_m0_pos_Z = sensorData.moving_object(0).base().position().z();
    auto sensorData_m1_pos_Z = sensorData.moving_object(1).base().position().z();
    auto sensorData_s0_pos_Z = sensorData.moving_object(0).base().position().z();

    //Are Moving Objects correct rotated to each other
    ASSERT_EQ(sensorData_m0_yaw_ortho - movingHost_yaw, world_yawDelta_Ortho);
    ASSERT_EQ(sensorData_m1_yaw_opposite - movingHost_yaw, world_yawDelta_Opposite);
    ASSERT_EQ(sensorData_s0_yaw_ortho - movingHost_yaw, world_yawDelta_Ortho);

   /*ASSERT_EQ(sensorData.has_mounting_position(), true);
   ASSERT_EQ(sensorData.mounting_position().orientation().yaw(), sensor_mounting_Yaw);
   ASSERT_EQ(sensorData.mounting_position().position().x(), sensor_mounting_posOffX);
   ASSERT_EQ(sensorData.mounting_position().position().y(), sensor_mounting_posOffY);*/

   ASSERT_EQ(sensorData.sensor_view(0).has_mounting_position(), true);
   ASSERT_EQ(sensorData.sensor_view(0).mounting_position().orientation().yaw(), sensor_mounting_Yaw);
   ASSERT_EQ(sensorData.sensor_view(0).mounting_position().position().x(), sensor_mounting_posOffX);
   ASSERT_EQ(sensorData.sensor_view(0).mounting_position().position().y(), sensor_mounting_posOffY);

   //Are Absolute values in SensorSpace?
   ASSERT_EQ(sensorData_m0_yaw_ortho + sensor_mounting_Yaw, world_yawDelta_Ortho );
   ASSERT_EQ(sensorData_m1_yaw_opposite + sensor_mounting_Yaw, world_yawDelta_Opposite);
   ASSERT_EQ(sensorData_s0_yaw_ortho + sensor_mounting_Yaw, world_yawDelta_Opposite);

   ASSERT_EQ(sensorData_m0_pos_X + sensor_mounting_posOffX, groundTruth->moving_object(1).base().position().x());
   ASSERT_EQ(sensorData_m1_pos_X + sensor_mounting_posOffX, groundTruth->moving_object(2).base().position().x());
   ASSERT_EQ(sensorData_s0_pos_X + sensor_mounting_posOffX, groundTruth->moving_object(1).base().position().x());

   ASSERT_EQ(sensorData_m0_pos_Z + 0.0, groundTruth->moving_object(1).base().position().z());
   ASSERT_EQ(sensorData_m1_pos_Z + 0.0, groundTruth->moving_object(2).base().position().z());
   ASSERT_EQ(sensorData_s0_pos_Z + 0.0, groundTruth->moving_object(1).base().position().z());

   ASSERT_EQ(sensorData_m0_pos_Y + sensor_mounting_posOffY, groundTruth->moving_object(1).base().position().y());
   ASSERT_EQ(sensorData_m1_pos_Y + sensor_mounting_posOffY, groundTruth->moving_object(2).base().position().y());
   ASSERT_EQ(sensorData_s0_pos_Y + sensor_mounting_posOffY, groundTruth->moving_object(1).base().position().y());



}

