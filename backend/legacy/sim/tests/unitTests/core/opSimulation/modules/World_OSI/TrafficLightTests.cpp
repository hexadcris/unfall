/*******************************************************************************
* Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "TrafficLight.h"
#include "OWL/fakes/fakeTrafficLight.h"
#include "fakeCallback.h"

#include "fakes/gmock/fakeRoadSignal.h"

using ::testing::NiceMock;

using namespace OWL::Implementation;

TEST(TrafficLight, ThreeSignalsNoLogging)
{

    const std::string opendrive_id = "testId";
    std::shared_ptr<osi3::TrafficLight> trafficLightRed = std::make_shared<osi3::TrafficLight>();
    std::shared_ptr<osi3::TrafficLight> trafficLightYellow = std::make_shared<osi3::TrafficLight>();
    std::shared_ptr<osi3::TrafficLight> trafficLightGreen = std::make_shared<osi3::TrafficLight>();
    NiceMock<FakeCallback> fakeCallback;

    trafficLightRed->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_RED);
    trafficLightYellow->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_YELLOW);
    trafficLightGreen->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_GREEN);


    ThreeSignalsTrafficLight threeSignalsTrafficLight(opendrive_id, trafficLightRed.get(),
                                                      trafficLightYellow.get(),trafficLightGreen.get(),
                                                      &fakeCallback);

    EXPECT_CALL(fakeCallback, Log).Times(0);

    threeSignalsTrafficLight.SetState(CommonTrafficLight::State::Red);
    threeSignalsTrafficLight.SetState(CommonTrafficLight::State::Yellow);
    threeSignalsTrafficLight.SetState(CommonTrafficLight::State::Green);

}

TEST(TrafficLight, ThreeSignalsTrafficLightTestUnknownState)
{

    const std::string opendrive_id = "testId";
    osi3::TrafficLight trafficLightRed;
    osi3::TrafficLight trafficLightYellow;
    osi3::TrafficLight trafficLightGreen;
    NiceMock<FakeCallback> fakeCallback;

    ThreeSignalsTrafficLight threeSignalsTrafficLight(opendrive_id, &trafficLightRed,
                                                      &trafficLightYellow, &trafficLightGreen,
                                                      &fakeCallback);

    EXPECT_CALL(fakeCallback, Log).Times(1);

    threeSignalsTrafficLight.SetState(CommonTrafficLight::State::Unknown);

}

TEST(TrafficLight, TwoSignalsNoLogging)
{

    const std::string opendrive_id = "testId";
    std::shared_ptr<osi3::TrafficLight> trafficLightTop = std::make_shared<osi3::TrafficLight>();
    std::shared_ptr<osi3::TrafficLight> trafficLightBottom = std::make_shared<osi3::TrafficLight>();
    NiceMock<FakeCallback> fakeCallback;

    trafficLightBottom->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_RED);
    trafficLightTop->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_GREEN);


    TwoSignalsTrafficLight twoSignalsTrafficLight(opendrive_id, trafficLightTop.get(),
                                                  trafficLightBottom.get(),
                                                  &fakeCallback);


    EXPECT_CALL(fakeCallback, Log).Times(0);

    twoSignalsTrafficLight.SetState(CommonTrafficLight::State::Red);
    twoSignalsTrafficLight.SetState(CommonTrafficLight::State::Green);

}

TEST(TrafficLight, TwoSignalsTrafficLightTestUnknownState)
{

    const std::string opendrive_id = "testId";
    osi3::TrafficLight trafficLightTop;
    osi3::TrafficLight trafficLightBottom;
    NiceMock<FakeCallback> fakeCallback;

    TwoSignalsTrafficLight twoSignalsTrafficLight(opendrive_id, &trafficLightTop,
                                                  &trafficLightBottom,
                                                  &fakeCallback);


    EXPECT_CALL(fakeCallback, Log).Times(1);

    twoSignalsTrafficLight.SetState(CommonTrafficLight::State::Unknown);

}

TEST(TrafficLight, TwoSignalsTrafficLightOnlyOneLight)
{

    const std::string opendrive_id = "testId";
    std::shared_ptr<osi3::TrafficLight> trafficLightTop = std::make_shared<osi3::TrafficLight>();
    NiceMock<FakeCallback> fakeCallback;

    trafficLightTop->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_RED);


    TwoSignalsTrafficLight twoSignalsTrafficLight(opendrive_id, trafficLightTop.get(),
                                                  nullptr,
                                                  &fakeCallback);


    EXPECT_CALL(fakeCallback, Log).Times(1);

    twoSignalsTrafficLight.SetState(CommonTrafficLight::State::Red);

}
TEST(TrafficLight, OneSignalsTrafficLightNoLogging)
{

    const std::string opendrive_id = "testId";
    std::shared_ptr<osi3::TrafficLight> trafficLight = std::make_shared<osi3::TrafficLight>();
    NiceMock<FakeCallback> fakeCallback;

    trafficLight->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_RED);

    OneSignalsTrafficLight oneSignalsTrafficLight(opendrive_id, trafficLight.get(),
                                                  &fakeCallback);

    EXPECT_CALL(fakeCallback, Log).Times(0);

    oneSignalsTrafficLight.SetState(CommonTrafficLight::State::Red);

}


TEST(TrafficLight, OneSignalsTrafficLightTestUnknownState)
{

    const std::string opendrive_id = "testId";
    std::shared_ptr<osi3::TrafficLight> trafficLight = std::make_shared<osi3::TrafficLight>();
    NiceMock<FakeCallback> fakeCallback;

    OneSignalsTrafficLight oneSignalsTrafficLight(opendrive_id, trafficLight.get(),
                                                  &fakeCallback);

    EXPECT_CALL(fakeCallback, Log).Times(1);

    oneSignalsTrafficLight.SetState(CommonTrafficLight::State::Unknown);

    EXPECT_CALL(fakeCallback, Log).Times(1);

    oneSignalsTrafficLight.SetState(CommonTrafficLight::State::RedYellow);

}

TEST(TrafficLight, SetSpecificationNoLog)
{

    const std::string opendrive_id = "testId";
    std::shared_ptr<osi3::TrafficLight> trafficLight = std::make_shared<osi3::TrafficLight>();
    NiceMock<FakeCallback> fakeCallback;


    OneSignalsTrafficLight oneSignalsTrafficLight(opendrive_id, trafficLight.get(),
                                                  &fakeCallback);

    NiceMock<FakeRoadSignal> fakeRoadSignal;
    std::string type = "1.000.002";
    std::string subtype = "10";
    ON_CALL(fakeRoadSignal, GetType).WillByDefault(testing::Return(type));
    ON_CALL(fakeRoadSignal, GetSubType).WillByDefault(testing::Return(subtype));
    Position position;

    EXPECT_CALL(fakeCallback, Log).Times(0);

    oneSignalsTrafficLight.SetSpecification(&fakeRoadSignal, position);

}

TEST(TrafficLight, SetSpecificationWrongType)
{

    const std::string opendrive_id = "testId";
    std::shared_ptr<osi3::TrafficLight> trafficLight = std::make_shared<osi3::TrafficLight>();
    NiceMock<FakeCallback> fakeCallback;


    OneSignalsTrafficLight oneSignalsTrafficLight(opendrive_id, trafficLight.get(),
                                                  &fakeCallback);

    NiceMock<FakeRoadSignal> fakeRoadSignal;
    std::string type = "wrongType";
    std::string subtype = "10";
    ON_CALL(fakeRoadSignal, GetType).WillByDefault(testing::Return(type));
    ON_CALL(fakeRoadSignal, GetSubType).WillByDefault(testing::Return(subtype));
    Position position;

    //Color and icon log
    EXPECT_CALL(fakeCallback, Log).Times(2);

    oneSignalsTrafficLight.SetSpecification(&fakeRoadSignal, position);

}

TEST(TrafficLight, SetSpecificationWrongSubtype)
{
    const std::string opendrive_id = "testId";
    std::shared_ptr<osi3::TrafficLight> trafficLight = std::make_shared<osi3::TrafficLight>();

    NiceMock<FakeCallback> fakeCallback;


    OneSignalsTrafficLight oneSignalsTrafficLight(opendrive_id, trafficLight.get(),
                                                  &fakeCallback);

    NiceMock<FakeRoadSignal> fakeRoadSignal;
    std::string type = "1.000.002";
    std::string subtype = "wrong";
    ON_CALL(fakeRoadSignal, GetType).WillByDefault(testing::Return(type));
    ON_CALL(fakeRoadSignal, GetSubType).WillByDefault(testing::Return(subtype));
    Position position;

    //Color and icon log
    EXPECT_CALL(fakeCallback, Log).Times(2);

    oneSignalsTrafficLight.SetSpecification(&fakeRoadSignal, position);

}