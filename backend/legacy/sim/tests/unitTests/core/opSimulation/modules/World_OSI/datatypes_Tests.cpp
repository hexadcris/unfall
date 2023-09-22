/********************************************************************************
 * Copyright (c) 2019-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "DataTypes.h"
#include "TrafficLight.h"
#include "MovingObject.h"
#include "OsiDefaultValues.h"

#include "fakeRoadSignal.h"
#include "OWL/fakes/fakeMovingObject.h"

#include "fakeCallback.h"

using ::testing::Return;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::DoubleNear;
using ::testing::SizeIs;
using ::testing::ElementsAre;
using ::testing::NiceMock;

TEST(TrafficSigns, SetSpecificationWithUnsupportedMainType_ReturnsFalse)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("unsupported_type_string"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(false));
}

TEST(TrafficSigns, SetSpecificationTypeOnly)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::HighWayExit));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_HIGHWAY_EXIT));
};

TEST(TrafficSigns, SetSpecificationSubtypeDefinesValue)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("450"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("51"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::HighwayExitPole));
    ASSERT_THAT(specification.value, Eq(200.0));
    ASSERT_THAT(specification.unit, Eq(CommonTrafficSign::Unit::Meter));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_POLE_EXIT));
};

TEST(TrafficSigns, SetSpecificationSubtypeIsValue)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("278"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("80"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::EndOfMaximumSpeedLimit));
    ASSERT_THAT(specification.value, DoubleNear(80.0 / 3.6, 1e-3));
    ASSERT_THAT(specification.unit, Eq(CommonTrafficSign::Unit::MeterPerSecond));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_END));
};

TEST(TrafficSigns, SetSpecificationWithText)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("310"));
    ON_CALL(roadSignal, GetText()).WillByDefault(Return("SomeText"));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::TownBegin));
    ASSERT_THAT(specification.text, Eq("SomeText"));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_TOWN_BEGIN));
};

TEST(TrafficSigns, SetSpecificationWithSupplementarySign)
{
    FakeRoadSignal mainSignal;
    ON_CALL(mainSignal, GetType()).WillByDefault(Return("276"));
    FakeRoadSignal supplementarySignal;
    ON_CALL(supplementarySignal, GetType()).WillByDefault(Return("1004"));
    ON_CALL(supplementarySignal, GetSubType()).WillByDefault(Return("30"));
    ON_CALL(supplementarySignal, GetValue()).WillByDefault(Return(100));
    Position position{};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&mainSignal, position), Eq(true));

    trafficSign.AddSupplementarySign(&supplementarySignal, position);
    const auto specification = trafficSign.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::OvertakingBanBegin));
    ASSERT_THAT(specification.supplementarySigns.size(), Eq(1));
    ASSERT_THAT(specification.supplementarySigns.front().type, Eq(CommonTrafficSign::Type::DistanceIndication));
    ASSERT_THAT(specification.supplementarySigns.front().value, Eq(100));
    ASSERT_THAT(specification.supplementarySigns.front().unit, Eq(CommonTrafficSign::Unit::Meter));
    ASSERT_THAT(osiSign.main_sign().classification().type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OVERTAKING_BAN_BEGIN));
    ASSERT_THAT(osiSign.supplementary_sign().Get(0).classification().type(), Eq(osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_SPACE));
};

TEST(TrafficSigns, SetSpecification_SetsCorrectBaseStationary)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(5.0));
    ON_CALL(roadSignal, GetZOffset()).WillByDefault(Return(3.0));
    Position position{10, 11, -1.5, 0};

    osi3::TrafficSign osiSign;
    OWL::Implementation::TrafficSign trafficSign(&osiSign);

    ASSERT_THAT(trafficSign.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiSign.main_sign().base().position().x(), Eq(10));
    ASSERT_THAT(osiSign.main_sign().base().position().y(), Eq(11));
    ASSERT_THAT(osiSign.main_sign().base().position().z(), Eq(5.5));
    ASSERT_THAT(osiSign.main_sign().base().dimension().width(), Eq(4));
    ASSERT_THAT(osiSign.main_sign().base().dimension().height(), Eq(5));
    ASSERT_THAT(osiSign.main_sign().base().orientation().yaw(), Eq(-1.5));
}

TEST(TrafficSigns_GetSpecification, GivenSignWithoutSupplementarySigns_ReturnsCorrectEntity)
{
    osi3::TrafficSign osiSign;
    osiSign.mutable_id()->set_value(1);
    osiSign.mutable_main_sign()->mutable_classification()->mutable_value()->set_value(5.0);

    OWL::Implementation::TrafficSign sign{&osiSign};

    const auto spec = sign.GetSpecification(1.1);

    ASSERT_THAT(spec.value, DoubleEq(5.0));
    ASSERT_THAT(spec.relativeDistance, DoubleEq(1.1));
    ASSERT_THAT(spec.supplementarySigns, SizeIs(0));
}

TEST(TrafficSigns_GetSpecification, GivenSignWithOneSupplementarySign_ReturnsCorrectEntity)
{
    osi3::TrafficSign osiSign;
    auto osiSupplementarySign = osiSign.add_supplementary_sign();

    osiSign.mutable_id()->set_value(1);
    osiSign.mutable_main_sign()->mutable_classification()->mutable_value()->set_value(6.0);

    osiSupplementarySign->mutable_classification()->add_value()->set_value(7.0);
    osiSupplementarySign->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification::TYPE_SPACE);

    OWL::Implementation::TrafficSign sign{&osiSign};

    const auto spec = sign.GetSpecification(1.1);

    ASSERT_THAT(spec.value, DoubleEq(6.0));
    ASSERT_THAT(spec.relativeDistance, DoubleEq(1.1));
    ASSERT_THAT(spec.supplementarySigns, SizeIs(1));

    auto supplementary = spec.supplementarySigns.begin();

    EXPECT_THAT(supplementary->type, Eq(CommonTrafficSign::Type::DistanceIndication));
    ASSERT_THAT(supplementary->relativeDistance, DoubleEq(1.1));
    EXPECT_THAT(supplementary->value, Eq(7.0));
}

TEST(TrafficSigns_GetSpecification, GivenSignWithTwoSupplementarySigns_ReturnsCorrectEntity)
{
    osi3::TrafficSign osiSign;
    auto osiSupplementarySign1 = osiSign.add_supplementary_sign();
    auto osiSupplementarySign2 = osiSign.add_supplementary_sign();

    osiSign.mutable_id()->set_value(1);
    osiSign.mutable_main_sign()->mutable_classification()->mutable_value()->set_value(8.0);

    osiSupplementarySign1->mutable_classification()->add_value()->set_value(9.0);
    osiSupplementarySign1->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification::TYPE_SPACE);
    osiSupplementarySign2->mutable_classification()->add_value()->set_value(10.0);
    osiSupplementarySign2->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification::TYPE_SPACE);

    OWL::Implementation::TrafficSign sign{&osiSign};

    const auto spec = sign.GetSpecification(1.1);

    ASSERT_THAT(spec.value, DoubleEq(8.0));
    ASSERT_THAT(spec.relativeDistance, DoubleEq(1.1));
    ASSERT_THAT(spec.supplementarySigns, SizeIs(2));

    auto supplementary1 = spec.supplementarySigns.begin();
    auto supplementary2 = std::next(supplementary1);

    EXPECT_THAT(supplementary1->type, Eq(CommonTrafficSign::Type::DistanceIndication));
    ASSERT_THAT(supplementary1->relativeDistance, DoubleEq(1.1));
    EXPECT_THAT(supplementary1->value, Eq(9.0));
    EXPECT_THAT(supplementary2->type, Eq(CommonTrafficSign::Type::DistanceIndication));
    ASSERT_THAT(supplementary2->relativeDistance, DoubleEq(1.1));
    EXPECT_THAT(supplementary2->value, Eq(10.0));
}

TEST(RoadMarking, SetSpecificationWithUnsupportedMainType_ReturnsFalse)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("unsupported_type_string"));
    Position position{};

    osi3::RoadMarking osiMarking;
    OWL::Implementation::RoadMarking roadMarking(&osiMarking);

    ASSERT_THAT(roadMarking.SetSpecification(&roadSignal, position), Eq(false));
}

TEST(RoadMarking, SetSpecificationTypeOnly)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("294"));
    Position position{};

    osi3::RoadMarking osiMarking;
    OWL::Implementation::RoadMarking roadMarking(&osiMarking);

    ASSERT_THAT(roadMarking.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = roadMarking.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficSign::Type::Stop));
    ASSERT_THAT(osiMarking.classification().type(), Eq(osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_SYMBOLIC_TRAFFIC_SIGN));
    ASSERT_THAT(osiMarking.classification().traffic_main_sign_type(), Eq(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_STOP));
}

TEST(RoadMarking, SetSpecification_SetsCorrectBaseStationary)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("294"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    Position position{10, 11, -1.5, 0};

    osi3::RoadMarking osiMarking;
    OWL::Implementation::RoadMarking roadMarking(&osiMarking);

    ASSERT_THAT(roadMarking.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiMarking.base().position().x(), Eq(10));
    ASSERT_THAT(osiMarking.base().position().y(), Eq(11));
    ASSERT_THAT(osiMarking.base().position().z(), Eq(0));
    ASSERT_THAT(osiMarking.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiMarking.base().orientation().yaw(), DoubleEq(-1.5));
}

namespace OWL
{
bool operator== (const OWL::LaneOverlap& rhs, const OWL::LaneOverlap& lhs)
{
    return rhs.sMin == lhs.sMin && rhs.sMax == lhs.sMax;
}
}

TEST(LaneAssignmentCollector, GetDownstream_ReturnObjectsInCorrectOrder)
{
    OWL::Implementation::Lane::LaneAssignmentCollector laneAssignmentCollector;
    OWL::Fakes::MovingObject object1;
    OWL::Fakes::MovingObject object2;
    OWL::Fakes::MovingObject object3;
    OWL::LaneOverlap overlap1{GlobalRoadPosition{"",0,10,0,0},
                              GlobalRoadPosition{"",0,15,0,0},
                              GlobalRoadPosition{"",0,12,0,0},
                              GlobalRoadPosition{"",0,12,0,0}};
    OWL::LaneOverlap overlap2{GlobalRoadPosition{"",0,12,0,0},
                              GlobalRoadPosition{"",0,17,0,0},
                              GlobalRoadPosition{"",0,15,0,0},
                              GlobalRoadPosition{"",0,15,0,0}};
    OWL::LaneOverlap overlap3{GlobalRoadPosition{"",0,11,0,0},
                              GlobalRoadPosition{"",0,20,0,0},
                              GlobalRoadPosition{"",0,15,0,0},
                              GlobalRoadPosition{"",0,15,0,0}};
    laneAssignmentCollector.Insert(overlap1, &object1);
    laneAssignmentCollector.Insert(overlap2, &object2);
    laneAssignmentCollector.Insert(overlap3, &object3);

    auto result = laneAssignmentCollector.Get(true);

    ASSERT_THAT(result, ElementsAre(OWL::Interfaces::LaneAssignment{overlap1, &object1},
                                    OWL::Interfaces::LaneAssignment{overlap3, &object3},
                                    OWL::Interfaces::LaneAssignment{overlap2, &object2}));
}

TEST(LaneAssignmentCollector, GetUpstream_ReturnObjectsInCorrectOrder)
{
    OWL::Implementation::Lane::LaneAssignmentCollector laneAssignmentCollector;
    OWL::Fakes::MovingObject object1;
    OWL::Fakes::MovingObject object2;
    OWL::Fakes::MovingObject object3;
    OWL::LaneOverlap overlap1{GlobalRoadPosition{"",0,10,0,0},
                              GlobalRoadPosition{"",0,15,0,0},
                              GlobalRoadPosition{"",0,12,0,0},
                              GlobalRoadPosition{"",0,12,0,0}};
    OWL::LaneOverlap overlap2{GlobalRoadPosition{"",0,12,0,0},
                              GlobalRoadPosition{"",0,17,0,0},
                              GlobalRoadPosition{"",0,15,0,0},
                              GlobalRoadPosition{"",0,15,0,0}};
    OWL::LaneOverlap overlap3{GlobalRoadPosition{"",0,11,0,0},
                              GlobalRoadPosition{"",0,20,0,0},
                              GlobalRoadPosition{"",0,15,0,0},
                              GlobalRoadPosition{"",0,15,0,0}};
    laneAssignmentCollector.Insert(overlap1, &object1);
    laneAssignmentCollector.Insert(overlap2, &object2);
    laneAssignmentCollector.Insert(overlap3, &object3);

    auto result = laneAssignmentCollector.Get(false);

    ASSERT_THAT(result, ElementsAre(OWL::Interfaces::LaneAssignment{overlap3, &object3},
                                    OWL::Interfaces::LaneAssignment{overlap2, &object2},
                                    OWL::Interfaces::LaneAssignment{overlap1, &object1}));
}

TEST(TrafficLights, SetSpecification_ThreeLights)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.011"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("20"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightYellow;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::ThreeSignalsTrafficLight trafficLight {"openDriveId", &osiLightRed, &osiLightYellow, &osiLightGreen, &fakeCallback};
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficLight.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficLight::Type::ThreeLightsRight));
    ASSERT_THAT(osiLightRed.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT));
    ASSERT_THAT(osiLightYellow.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT));
    ASSERT_THAT(osiLightGreen.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT));

    ASSERT_THAT(osiLightRed.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_RED));
    ASSERT_THAT(osiLightYellow.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_YELLOW));
    ASSERT_THAT(osiLightGreen.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));

    ASSERT_THAT(osiLightRed.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightYellow.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightGreen.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

    ASSERT_THAT(osiLightRed.classification().counter(), Eq(0));
    ASSERT_THAT(osiLightYellow.classification().counter(), Eq(0));
    ASSERT_THAT(osiLightGreen.classification().counter(), Eq(0));

    ASSERT_THAT(osiLightRed.classification().is_out_of_service(), Eq(false));
    ASSERT_THAT(osiLightYellow.classification().is_out_of_service(), Eq(false));
    ASSERT_THAT(osiLightGreen.classification().is_out_of_service(), Eq(false));

    //Assert yaw is not nan while the rest is nan
    ASSERT_EQ(std::isnan(osiLightRed.mutable_base()->mutable_orientation()->yaw()), false);
    ASSERT_EQ(std::isnan(osiLightRed.mutable_base()->mutable_orientation()->roll()), true);
    ASSERT_EQ(std::isnan(osiLightRed.mutable_base()->mutable_orientation()->pitch()), true);

    ASSERT_EQ(std::isnan(osiLightYellow.mutable_base()->mutable_orientation()->yaw()), false);
    ASSERT_EQ(std::isnan(osiLightYellow.mutable_base()->mutable_orientation()->roll()), true);
    ASSERT_EQ(std::isnan(osiLightYellow.mutable_base()->mutable_orientation()->pitch()), true);

    ASSERT_EQ(std::isnan(osiLightGreen.mutable_base()->mutable_orientation()->yaw()), false);
    ASSERT_EQ(std::isnan(osiLightGreen.mutable_base()->mutable_orientation()->roll()), true);
    ASSERT_EQ(std::isnan(osiLightGreen.mutable_base()->mutable_orientation()->pitch()), true);

    ASSERT_EQ(osiLightRed.mutable_base()->mutable_dimension()->height(), 0.2 / 3.0);
    ASSERT_EQ(osiLightRed.mutable_base()->mutable_dimension()->width(), 0.1);
    ASSERT_EQ(osiLightRed.mutable_base()->mutable_dimension()->length(), 0.1);

    ASSERT_EQ(osiLightYellow.mutable_base()->mutable_dimension()->height(), 0.2 / 3.0);
    ASSERT_EQ(osiLightYellow.mutable_base()->mutable_dimension()->width(), 0.1);
    ASSERT_EQ(osiLightYellow.mutable_base()->mutable_dimension()->length(), 0.1);

    ASSERT_EQ(osiLightGreen.mutable_base()->mutable_dimension()->height(), 0.2 / 3.0);
    ASSERT_EQ(osiLightGreen.mutable_base()->mutable_dimension()->width(), 0.1);
    ASSERT_EQ(osiLightGreen.mutable_base()->mutable_dimension()->length(), 0.1);

};

TEST(TrafficLights, SetSpecification_TwoLights_13)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.013"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return(""));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::TwoSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightGreen, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    const auto specification = trafficLight.GetSpecification(5);

    ASSERT_THAT(specification.type, Eq(CommonTrafficLight::Type::TwoLightsBicycle));
    ASSERT_THAT(osiLightRed.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_BICYCLE));
    ASSERT_THAT(osiLightGreen.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_BICYCLE));

    ASSERT_THAT(osiLightRed.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_RED));
    ASSERT_THAT(osiLightGreen.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));

    ASSERT_THAT(osiLightRed.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightGreen.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

    ASSERT_THAT(osiLightRed.classification().counter(), Eq(0));
    ASSERT_THAT(osiLightGreen.classification().counter(), Eq(0));

    ASSERT_THAT(osiLightRed.classification().is_out_of_service(), Eq(false));
    ASSERT_THAT(osiLightGreen.classification().is_out_of_service(), Eq(false));

    //Assert yaw is not nan while the rest is nan
    ASSERT_EQ(std::isnan(osiLightRed.mutable_base()->mutable_orientation()->yaw()), false);
    ASSERT_EQ(std::isnan(osiLightRed.mutable_base()->mutable_orientation()->roll()), true);
    ASSERT_EQ(std::isnan(osiLightRed.mutable_base()->mutable_orientation()->pitch()), true);

    ASSERT_EQ(std::isnan(osiLightGreen.mutable_base()->mutable_orientation()->yaw()), false);
    ASSERT_EQ(std::isnan(osiLightGreen.mutable_base()->mutable_orientation()->roll()), true);
    ASSERT_EQ(std::isnan(osiLightGreen.mutable_base()->mutable_orientation()->pitch()), true);

    ASSERT_EQ(osiLightRed.mutable_base()->mutable_dimension()->height(), 0.2 / 2.0);
    ASSERT_EQ(osiLightRed.mutable_base()->mutable_dimension()->width(), 0.1);
    ASSERT_EQ(osiLightRed.mutable_base()->mutable_dimension()->length(), 0.1);

    ASSERT_EQ(osiLightGreen.mutable_base()->mutable_dimension()->height(), 0.2 / 2.0);
    ASSERT_EQ(osiLightGreen.mutable_base()->mutable_dimension()->width(), 0.1);
    ASSERT_EQ(osiLightGreen.mutable_base()->mutable_dimension()->length(), 0.1);
};

TEST(TrafficLights, SetSpecification_TwoLights_09_10)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.009"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("10"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightYellow;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::TwoSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightYellow, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLightRed.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));
    ASSERT_THAT(osiLightYellow.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));

    ASSERT_THAT(osiLightRed.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_RED));
    ASSERT_THAT(osiLightYellow.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_YELLOW));

    ASSERT_THAT(osiLightRed.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightYellow.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

};

TEST(TrafficLights, SetSpecification_TwoLights_09_20)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.009"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("20"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLightYellow;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::TwoSignalsTrafficLight trafficLight("openDriveId", &osiLightYellow, &osiLightGreen, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLightYellow.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));
    ASSERT_THAT(osiLightGreen.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));

    ASSERT_THAT(osiLightYellow.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_YELLOW));
    ASSERT_THAT(osiLightGreen.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));

    ASSERT_THAT(osiLightYellow.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightGreen.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

};

TEST(TrafficLights, SetSpecification_TwoLights_09_30)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.009"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("30"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::TwoSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightGreen, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLightRed.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));
    ASSERT_THAT(osiLightGreen.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));

    ASSERT_THAT(osiLightRed.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_RED));
    ASSERT_THAT(osiLightGreen.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));

    ASSERT_THAT(osiLightRed.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightGreen.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

};

TEST(TrafficLights, SetSpecification_TwoLights_Pedestrian)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.002"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return(""));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::TwoSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightGreen, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLightRed.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_PEDESTRIAN));
    ASSERT_THAT(osiLightGreen.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_PEDESTRIAN));

    ASSERT_THAT(osiLightRed.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_RED));
    ASSERT_THAT(osiLightGreen.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));

    ASSERT_THAT(osiLightRed.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightGreen.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

};

TEST(TrafficLights, SetSpecification_TwoLights_PedestrianAndBike)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.007"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return(""));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::TwoSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightGreen, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLightRed.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE));
    ASSERT_THAT(osiLightGreen.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE));

    ASSERT_THAT(osiLightRed.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_RED));
    ASSERT_THAT(osiLightGreen.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));

    ASSERT_THAT(osiLightRed.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightGreen.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

};

TEST(TrafficLights, SetSpecification_TwoLights_YellowGreenWithArrows)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.010"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("10"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::TwoSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightGreen, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLightRed.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_LEFT));
    ASSERT_THAT(osiLightGreen.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_LEFT));

    ASSERT_THAT(osiLightRed.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_YELLOW));
    ASSERT_THAT(osiLightGreen.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));

    ASSERT_THAT(osiLightRed.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightGreen.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("20"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLightRed.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT));
    ASSERT_THAT(osiLightGreen.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT));

    ASSERT_THAT(osiLightRed.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_YELLOW));
    ASSERT_THAT(osiLightGreen.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));

    ASSERT_THAT(osiLightRed.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
    ASSERT_THAT(osiLightGreen.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));
};

TEST(TrafficLights, SetSpecification_OneLight_WithArrows_Red)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.020"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return(""));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLight;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::OneSignalsTrafficLight trafficLight("openDriveId", &osiLight, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));
    ASSERT_THAT(osiLight.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_RED));
    ASSERT_THAT(osiLight.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("10"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("20"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("30"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("40"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("50"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("60"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("70"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_RIGHT));
};

TEST(TrafficLights, SetSpecification_OneLight_WithArrows_Yellow)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.008"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return(""));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLight;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::OneSignalsTrafficLight trafficLight("openDriveId", &osiLight, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));
    ASSERT_THAT(osiLight.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_YELLOW));
    ASSERT_THAT(osiLight.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("10"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("20"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("30"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("40"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("50"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("60"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("70"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_RIGHT));
};

TEST(TrafficLights, SetSpecification_OneLight_WithArrows_Green)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.012"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return(""));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(0.1));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(0.2));
    Position position{};

    osi3::TrafficLight osiLight;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::OneSignalsTrafficLight trafficLight("openDriveId", &osiLight, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));

    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_NONE));
    ASSERT_THAT(osiLight.classification().color(), Eq(osi3::TrafficLight_Classification_Color_COLOR_GREEN));
    ASSERT_THAT(osiLight.classification().mode(), Eq(osi3::TrafficLight_Classification_Mode_MODE_OTHER));

    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("10"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("20"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("30"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("40"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("50"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("60"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_LEFT));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("70"));
    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLight.classification().icon(), Eq(osi3::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_RIGHT));
};


TEST(TrafficLights, SetSpecification_SetsCorrectBaseStationaryForPositiveOrientation)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(5.0));
    ON_CALL(roadSignal, GetZOffset()).WillByDefault(Return(3.0));
    ON_CALL(roadSignal, GetHOffset()).WillByDefault(Return(2.0));
    ON_CALL(roadSignal, GetOrientation()).WillByDefault(Return(true));
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.011"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("20"));
    Position position{10, 11, -1.5, 0};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightYellow;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::ThreeSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightYellow, &osiLightGreen, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLightRed.base().position().x(), Eq(10));
    ASSERT_THAT(osiLightRed.base().position().y(), Eq(11));
    ASSERT_THAT(osiLightRed.base().position().z(), Eq(5.5 + (5.0 / 3.0)));
    ASSERT_THAT(osiLightRed.base().dimension().width(), Eq(4));
    //more then one traffic light are stacked on top of each other
    ASSERT_THAT(osiLightRed.base().dimension().height(), Eq((5.0 / 3.0)));
    ASSERT_THAT(osiLightRed.base().orientation().yaw(), DoubleEq(0.5));
    ASSERT_THAT(osiLightYellow.base().position().x(), Eq(10));
    ASSERT_THAT(osiLightYellow.base().position().y(), Eq(11));
    ASSERT_THAT(osiLightYellow.base().position().z(), Eq(5.5));
    ASSERT_THAT(osiLightYellow.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiLightYellow.base().dimension().height(), Eq((5.0 / 3.0)));
    ASSERT_THAT(osiLightYellow.base().orientation().yaw(), DoubleEq(0.5));
    ASSERT_THAT(osiLightGreen.base().position().x(), Eq(10));
    ASSERT_THAT(osiLightGreen.base().position().y(), Eq(11));
    ASSERT_THAT(osiLightGreen.base().position().z(), Eq(5.5 - (5.0 / 3.0) ));
    ASSERT_THAT(osiLightGreen.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiLightGreen.base().dimension().height(), Eq((5.0 / 3.0)));
    ASSERT_THAT(osiLightGreen.base().orientation().yaw(), DoubleEq(0.5));
}

TEST(TrafficLights, SetSpecification_SetsCorrectBaseTwoSignalTrafficLight)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(5.0));
    ON_CALL(roadSignal, GetZOffset()).WillByDefault(Return(3.0));
    ON_CALL(roadSignal, GetHOffset()).WillByDefault(Return(1.0));
    ON_CALL(roadSignal, GetOrientation()).WillByDefault(Return(false));
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.002"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return(""));
    Position position{10, 11, 1.5, 0};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightYellow;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::TwoSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightYellow, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLightRed.base().position().x(), Eq(10));
    ASSERT_THAT(osiLightRed.base().position().y(), Eq(11));
    ASSERT_THAT(osiLightRed.base().position().z(), Eq(5.5 + (5.0 / 4.0))); //center plus offset height/2 divided by 2
    ASSERT_THAT(osiLightRed.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiLightRed.base().dimension().height(), Eq((5.0 / 2.0)));
    ASSERT_THAT(osiLightRed.base().orientation().yaw(), DoubleEq(2.5 - M_PI));
    ASSERT_THAT(osiLightYellow.base().position().x(), Eq(10));
    ASSERT_THAT(osiLightYellow.base().position().y(), Eq(11));
    ASSERT_THAT(osiLightYellow.base().position().z(), Eq(5.5 - (5.0 / 4.0)));
    ASSERT_THAT(osiLightYellow.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiLightYellow.base().dimension().height(), Eq((5.0 / 2.0)));
    ASSERT_THAT(osiLightYellow.base().orientation().yaw(), DoubleEq(2.5 - M_PI));
    }

TEST(TrafficLights, SetSpecification_SetsCorrectBaseStationaryForNegativeOrientation)
{
    FakeRoadSignal roadSignal;
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("333"));
    ON_CALL(roadSignal, GetWidth()).WillByDefault(Return(4.0));
    ON_CALL(roadSignal, GetHeight()).WillByDefault(Return(5.0));
    ON_CALL(roadSignal, GetZOffset()).WillByDefault(Return(3.0));
    ON_CALL(roadSignal, GetHOffset()).WillByDefault(Return(1.0));
    ON_CALL(roadSignal, GetOrientation()).WillByDefault(Return(false));
    ON_CALL(roadSignal, GetType()).WillByDefault(Return("1.000.011"));
    ON_CALL(roadSignal, GetSubType()).WillByDefault(Return("20"));
    Position position{10, 11, 1.5, 0};

    osi3::TrafficLight osiLightRed;
    osi3::TrafficLight osiLightYellow;
    osi3::TrafficLight osiLightGreen;
    NiceMock<FakeCallback> fakeCallback;
    OWL::Implementation::ThreeSignalsTrafficLight trafficLight("openDriveId", &osiLightRed, &osiLightYellow, &osiLightGreen, &fakeCallback);
    trafficLight.initDefaultTrafficLight();

    ASSERT_THAT(trafficLight.SetSpecification(&roadSignal, position), Eq(true));
    ASSERT_THAT(osiLightRed.base().position().x(), Eq(10));
    ASSERT_THAT(osiLightRed.base().position().y(), Eq(11));
    ASSERT_THAT(osiLightRed.base().position().z(), Eq(5.5 + (5.0 / 3.0)));
    ASSERT_THAT(osiLightRed.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiLightRed.base().dimension().height(), Eq((5.0 / 3.0)));
    ASSERT_THAT(osiLightRed.base().orientation().yaw(), DoubleEq(2.5 - M_PI));
    ASSERT_THAT(osiLightYellow.base().position().x(), Eq(10));
    ASSERT_THAT(osiLightYellow.base().position().y(), Eq(11));
    ASSERT_THAT(osiLightYellow.base().position().z(), Eq(5.5));
    ASSERT_THAT(osiLightYellow.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiLightYellow.base().dimension().height(), Eq((5.0 / 3.0)));
    ASSERT_THAT(osiLightYellow.base().orientation().yaw(), DoubleEq(2.5 - M_PI));
    ASSERT_THAT(osiLightGreen.base().position().x(), Eq(10));
    ASSERT_THAT(osiLightGreen.base().position().y(), Eq(11));
    ASSERT_THAT(osiLightGreen.base().position().z(), Eq(5.5 - (5.0 / 3.0)));
    ASSERT_THAT(osiLightGreen.base().dimension().width(), Eq(4));
    ASSERT_THAT(osiLightGreen.base().dimension().height(), Eq((5.0 / 3.0)));
    ASSERT_THAT(osiLightGreen.base().orientation().yaw(), DoubleEq(2.5 - M_PI));
}

TEST(DefaultMovingObject, MovingObjectAddWheels)
{
    osi3::MovingObject osi_moving_object {};

    OWL::Implementation::MovingObject moving_object (&osi_moving_object);

    OWL::OsiDefaultValues defaultValues;

    OWL::WheelData wheelData;
    wheelData.position = {0.0, 1.0, 2.0};
    wheelData.axle = 1;
    wheelData.index = 1;
    wheelData.rotation_rate = 1.0;
    wheelData.width = 2.0;
    wheelData.wheelRadius = 3.0;
    wheelData.rim_radius = 4.0;

    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->number_wheels(),0);
    moving_object.AddWheel(wheelData);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->number_wheels(),1);

    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->wheel_data().at(0).axle(),wheelData.axle);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->wheel_data().at(0).index(),wheelData.index);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->wheel_data().at(0).rotation_rate(),wheelData.rotation_rate);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->wheel_data().at(0).width(),wheelData.width);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->wheel_data().at(0).wheel_radius(),wheelData.wheelRadius);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->wheel_data().at(0).rim_radius(),wheelData.rim_radius);

    wheelData.index = 2;
    moving_object.AddWheel(wheelData);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->wheel_data().at(1).index(),wheelData.index);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->wheel_data().at(1).rim_radius(),wheelData.rim_radius);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->number_wheels(),2);
}

TEST(DefaultMovingObject, GetDefaultValues)
{
    osi3::MovingObject osi_moving_object {};

    OWL::Implementation::MovingObject moving_object (&osi_moving_object);
    OWL::OsiDefaultValues osiDefaultValues;

    OWL::Implementation::DefaultMovingObjectFactory dmof;
    dmof.AssignDefaultValues(&osi_moving_object);

    double sig_nan = std::numeric_limits<double>::signaling_NaN();
    // NAN <- value != value
    ASSERT_NE(sig_nan, sig_nan);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation()->yaw()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation()->roll()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation()->pitch()), true);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation_rate()->yaw()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation_rate()->roll()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation_rate()->pitch()), true);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation_acceleration()->yaw()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation_acceleration()->roll()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_orientation_acceleration()->pitch()), true);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_dimension()->height()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_dimension()->width()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_dimension()->length()), true);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_position()->x()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_position()->y()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_position()->z()), true);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_velocity()->x()),true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_velocity()->y()),true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_velocity()->z()), true);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_acceleration()->x()),true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_acceleration()->y()),true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_base()->mutable_acceleration()->z()), true);

    ASSERT_EQ(osi_moving_object.mutable_base()->base_polygon().size(), 0);

    ASSERT_EQ(osi_moving_object.mutable_id()->value(), osiDefaultValues.d_identifier.value());
    ASSERT_EQ(osi_moving_object.type(), osi3::MovingObject_VehicleClassification_Type_TYPE_OTHER);
    ASSERT_EQ(osi_moving_object.mutable_assigned_lane_id()->size(), 0);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->number_wheels(), 0);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->x()),true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->y()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->z()), true);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_vehicle_attributes()->mutable_bbcenter_to_front()->x()),true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_vehicle_attributes()->mutable_bbcenter_to_front()->y()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_vehicle_attributes()->mutable_bbcenter_to_front()->z()), true);

    ASSERT_EQ(std::isnan(osi_moving_object.mutable_vehicle_attributes()->ground_clearance()), true);
    ASSERT_EQ(std::isnan(osi_moving_object.mutable_vehicle_attributes()->steering_wheel_angle()), true);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_attributes()->mutable_wheel_data()->size(), 0);


    int lightEnum_off = osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF;

    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->indicator_state(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->front_fog_light(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->rear_fog_light(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->head_light(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->high_beam(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->reversing_light(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->brake_light_state(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->license_plate_illumination_rear(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->emergency_vehicle_illumination(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->mutable_light_state()->service_vehicle_illumination(),
              lightEnum_off);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->has_trailer(),
              false);
    ASSERT_EQ(osi_moving_object.mutable_vehicle_classification()->type(),1); //other

    ASSERT_EQ(osi_moving_object.mutable_moving_object_classification()->mutable_assigned_lane_id()->size(), 0);
    ASSERT_EQ(osi_moving_object.mutable_moving_object_classification()->mutable_assigned_lane_percentage()->size(), 0);

    ASSERT_EQ(osi_moving_object.mutable_assigned_lane_id()->size(), 0);
    ASSERT_EQ(osi_moving_object.mutable_future_trajectory()->size(), 0);
    ASSERT_EQ(osi_moving_object.mutable_model_reference()->size(), 0);
    ASSERT_EQ(osi_moving_object.mutable_future_trajectory()->size(), 0);
}

