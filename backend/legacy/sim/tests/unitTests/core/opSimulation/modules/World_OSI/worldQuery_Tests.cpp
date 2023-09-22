/********************************************************************************
 * Copyright (c) 2018-2021 in-tech GmbH
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

#include "WorldDataQuery.h"
#include "common/globalDefinitions.h"

#include "fakeWorld.h"
#include "fakeSection.h"
#include "fakeLane.h"
#include "fakeLaneBoundary.h"
#include "fakeWorldData.h"
#include "fakeLaneManager.h"
#include "fakeMovingObject.h"
#include "fakeAgent.h"
#include "fakeTrafficSign.h"
#include "fakeRoadMarking.h"
#include "fakeOWLJunction.h"
#include "dontCare.h"

#include "Generators/laneGeometryElementGenerator.h"

using namespace OWL;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsNull;
using ::testing::_;
using ::testing::Eq;
using ::testing::Ne;
using ::testing::DoubleEq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Lt;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::UnorderedElementsAre;
using ::testing::IsEmpty;
using ::testing::SizeIs;
using ::testing::Const;
using ::testing::DontCare;
using ::testing::NiceMock;

class FakeLaneMultiStream
{
public:
    FakeLaneMultiStream()
    {
    }

    ~FakeLaneMultiStream()
    {
        for (auto node : nodes)
        {
            delete node->element->element;
        }
    }

    LaneMultiStream Get()
    {
        return LaneMultiStream{root};
    }

    std::pair<LaneMultiStream::Node*, Fakes::Lane*> AddRoot(double length, bool inStreamDirection)
    {
        Fakes::Lane* lane = new Fakes::Lane;
        ON_CALL(*lane, GetLength()).WillByDefault(Return(length));
        LaneStreamInfo laneInfo{lane, inStreamDirection ? 0 : length, inStreamDirection};
        root = {laneInfo, std::vector<LaneMultiStream::Node>{}, vertexCount};
        vertexCount++;
        root.next.reserve(10);
        nodes.push_back(&root);
        return {&root, lane};
    }

    std::pair<LaneMultiStream::Node*, Fakes::Lane*> AddLane(double length, bool inStreamDirection, LaneMultiStream::Node& predecessor)
    {
        Fakes::Lane* lane = new Fakes::Lane;
        ON_CALL(*lane, GetLength()).WillByDefault(Return(length));
        LaneStreamInfo laneInfo{lane, predecessor.element->EndS() + (inStreamDirection ? 0 : length), inStreamDirection};
        predecessor.next.push_back({laneInfo, std::vector<LaneMultiStream::Node>{}, vertexCount});
        vertexCount++;
        auto newNode = &predecessor.next.back();
        newNode->next.reserve(10);
        nodes.push_back(newNode);
        return {newNode, lane};
    }

private:
    LaneMultiStream::Node root;
    std::vector<LaneMultiStream::Node *> nodes;
    size_t vertexCount{0};
};

class FakeRoadMultiStream
{
public:
    FakeRoadMultiStream()
    {
    }

    ~FakeRoadMultiStream()
    {
        for (auto node : nodes)
        {
            delete node->element->element;
        }
    }

    RoadMultiStream Get()
    {
        return RoadMultiStream{root};
    }

    std::pair<RoadMultiStream::Node*, Fakes::Road*> AddRoot(double length, bool inStreamDirection)
    {
        Fakes::Road* road = new Fakes::Road;
        ON_CALL(*road, GetLength()).WillByDefault(Return(length));
        RoadStreamInfo roadInfo{road, inStreamDirection ? 0 : length, inStreamDirection};
        root = {roadInfo, std::vector<RoadMultiStream::Node>{}, vertexCount};
        vertexCount++;
        root.next.reserve(10);
        nodes.push_back(&root);
        return {&root, road};
    }

    std::pair<RoadMultiStream::Node*, Fakes::Road*> AddRoad(double length, bool inStreamDirection, RoadMultiStream::Node& predecessor)
    {
        Fakes::Road* road = new Fakes::Road;
        ON_CALL(*road, GetLength()).WillByDefault(Return(length));
        RoadStreamInfo roadInfo{road, predecessor.element->EndS() + (inStreamDirection ? 0 : length), inStreamDirection};
        predecessor.next.push_back({roadInfo, std::vector<RoadMultiStream::Node>{}, vertexCount});
        vertexCount++;
        auto newNode = &predecessor.next.back();
        newNode->next.reserve(10);
        nodes.push_back(newNode);
        return {newNode, road};
    }

private:
    RoadMultiStream::Node root;
    std::vector<RoadMultiStream::Node *> nodes;
    size_t vertexCount{0};
};

TEST(GetDistanceToEndOfLane, LinearTreeWithOneLanePerRoad)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    ON_CALL(*lane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node2);
    ON_CALL(*lane3, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneMultiStream.Get(), 40.0, 1000.0, {LaneType::Driving});
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, 60.0),
                                             std::make_pair(node2->roadGraphVertex, 260.0),
                                             std::make_pair(node3->roadGraphVertex, 560.0)));
}

TEST(GetDistanceToEndOfLane, LinearTreeWithMultipleLanesPerRoad)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node2, lane2] = laneMultiStream.AddLane(200, true, *node1);
    ON_CALL(*lane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node2);
    ON_CALL(*lane3, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    node2->roadGraphVertex = node1->roadGraphVertex;
    node3->roadGraphVertex = node1->roadGraphVertex;
    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneMultiStream.Get(), 40.0, 1000.0, {LaneType::Driving});
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, 560.0)));
}

TEST(GetDistanceToEndOfLane, LinearTreeWithLaneOfWrongType)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    ON_CALL(*lane2, GetLaneType()).WillByDefault(Return(LaneType::None));
    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node2);
    ON_CALL(*lane3, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneMultiStream.Get(), 40.0, 1000.0, {LaneType::Driving});
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, 60.0),
                                             std::make_pair(node2->roadGraphVertex, 60.0),
                                             std::make_pair(node3->roadGraphVertex, 60.0)));
}

TEST(GetDistanceToEndOfLane, LinearTreeWithLengthBiggerSearchDistance)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    ON_CALL(*lane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node2);
    ON_CALL(*lane3, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneMultiStream.Get(), 40.0, 200.0, {LaneType::Driving});
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, 60.0),
                                             std::make_pair(node2->roadGraphVertex, std::numeric_limits<double>::infinity()),
                                             std::make_pair(node3->roadGraphVertex, std::numeric_limits<double>::infinity())));
}

TEST(GetDistanceToEndOfLane, BranchingTree)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    ON_CALL(*lane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node1);
    ON_CALL(*lane3, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    Fakes::WorldData worldData;

    WorldDataQuery wdQuery(worldData);
    auto result = wdQuery.GetDistanceToEndOfLane(laneMultiStream.Get(), 40.0, 1000.0, {LaneType::Driving});
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, 60.0),
                                             std::make_pair(node2->roadGraphVertex, 260.0),
                                             std::make_pair(node3->roadGraphVertex, 360.0)));
}

//////////////////////////////////////////////////////////////////////////////

TEST(GetObjectsOfTypeInRange, NoObjectstInRange_ReturnsEmptyVector)
{
    FakeLaneMultiStream laneMultiStream;
    OWL::Interfaces::LaneAssignments emptyObjectsList{};
    Fakes::Road road1;
    std::string idRoad1 = "Road1";
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetRoad()).WillByDefault(ReturnRef(road1));
    ON_CALL(*lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjectsList));
    Fakes::Road road2;
    std::string idRoad2 = "Road2";
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    ON_CALL(*lane2, GetWorldObjects(false)).WillByDefault(ReturnRef(emptyObjectsList));
    ON_CALL(*lane2, GetRoad()).WillByDefault(ReturnRef(road2));
    Fakes::Road road3;
    std::string idRoad3 = "Road3";
    ON_CALL(road3, GetId()).WillByDefault(ReturnRef(idRoad3));
    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node1);
    ON_CALL(*lane3, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyObjectsList));
    ON_CALL(*lane3, GetRoad()).WillByDefault(ReturnRef(road3));
    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(laneMultiStream.Get(), 0, 1000);
    std::vector<const OWL::Interfaces::WorldObject*> emptyResult{};
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, emptyResult),
                                             std::make_pair(node2->roadGraphVertex, emptyResult),
                                             std::make_pair(node3->roadGraphVertex, emptyResult)));
}

TEST(GetObjectsOfTypeInRange, OneObjectInEveryNode_ReturnsFirstObjectForAllNodes)
{
    FakeLaneMultiStream laneMultiStream;
    OWL::LaneOverlap overlap1{GlobalRoadPosition{"",0,40,0,0},
                              GlobalRoadPosition{"",0,55,0,0},
                              GlobalRoadPosition{"",0,50,0,0},
                              GlobalRoadPosition{"",0,50,0,0}};
    OWL::LaneOverlap overlap2{GlobalRoadPosition{"",0,95,0,0},
                              GlobalRoadPosition{"",0,105,0,0},
                              GlobalRoadPosition{"",0,100,0,0},
                              GlobalRoadPosition{"",0,100,0,0}};
    OWL::LaneOverlap overlap3{GlobalRoadPosition{"",0,95,0,0},
                              GlobalRoadPosition{"",0,105,0,0},
                              GlobalRoadPosition{"",0,100,0,0},
                              GlobalRoadPosition{"",0,100,0,0}};
    Fakes::MovingObject object1;
    OWL::Interfaces::LaneAssignments objectsList1{{overlap1,&object1}};
    Fakes::MovingObject object2;
    OWL::Interfaces::LaneAssignments objectsList2{{overlap2,&object2}};
    Fakes::MovingObject object3;
    OWL::Interfaces::LaneAssignments objectsList3{{overlap3,&object3}};
    Fakes::Road road1;
    std::string idRoad1 = "Road1";
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetRoad()).WillByDefault(ReturnRef(road1));
    ON_CALL(*lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsList1));
    Fakes::Road road2;
    std::string idRoad2 = "Road2";
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    ON_CALL(*lane2, GetWorldObjects(false)).WillByDefault(ReturnRef(objectsList2));
    ON_CALL(*lane2, GetRoad()).WillByDefault(ReturnRef(road2));
    Fakes::Road road3;
    std::string idRoad3 = "Road3";
    ON_CALL(road3, GetId()).WillByDefault(ReturnRef(idRoad3));
    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node1);
    ON_CALL(*lane3, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsList3));
    ON_CALL(*lane3, GetRoad()).WillByDefault(ReturnRef(road3));
    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(laneMultiStream.Get(), 50, 200);
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult1{&object1};
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult2{&object1, &object2};
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult3{&object1, &object3};
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, expectedResult1),
                                             std::make_pair(node2->roadGraphVertex, expectedResult2),
                                             std::make_pair(node3->roadGraphVertex, expectedResult3)));
}

TEST(GetObjectsOfTypeInRange, OneObjectInTwoNodes_ReturnsObjectOnlyOnce)
{
    FakeLaneMultiStream laneMultiStream;
    Fakes::MovingObject object;
    OWL::LaneOverlap overlap{GlobalRoadPosition{"",0,100,0,0},
                             GlobalRoadPosition{"",0,100,0,0},
                             GlobalRoadPosition{"",0,100,0,0},
                             GlobalRoadPosition{"",0,100,0,0}};
    OWL::Interfaces::LaneAssignments objectsList1{{overlap,&object}};
    OWL::Interfaces::LaneAssignments objectsList2{{overlap,&object}};
    Fakes::Road road1;
    std::string idRoad1 = "Road1";
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetRoad()).WillByDefault(ReturnRef(road1));
    ON_CALL(*lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsList1));
    Fakes::Road road2;
    std::string idRoad2 = "Road2";
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    ON_CALL(*lane2, GetWorldObjects(false)).WillByDefault(ReturnRef(objectsList2));
    ON_CALL(*lane2, GetRoad()).WillByDefault(ReturnRef(road2));
    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(laneMultiStream.Get(), 0, 1000);
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult{&object};
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, expectedResult),
                                             std::make_pair(node2->roadGraphVertex, expectedResult)));
}

TEST(GetObjectsOfTypeInRange, TwoObjectInTwoNodes_ReturnsObjectsInCorrectOrder)
{
    FakeLaneMultiStream laneMultiStream;
    OWL::LaneOverlap overlap1{GlobalRoadPosition{"",0,10,0,0},
                              GlobalRoadPosition{"",0,10,0,0},
                              GlobalRoadPosition{"",0,10,0,0},
                              GlobalRoadPosition{"",0,10,0,0}};
    OWL::LaneOverlap overlap2{GlobalRoadPosition{"",0,100,0,0},
                              GlobalRoadPosition{"",0,100,0,0},
                              GlobalRoadPosition{"",0,100,0,0},
                              GlobalRoadPosition{"",0,100,0,0}};
    OWL::LaneOverlap overlap3{GlobalRoadPosition{"",0,50,0,0},
                              GlobalRoadPosition{"",0,50,0,0},
                              GlobalRoadPosition{"",0,50,0,0},
                              GlobalRoadPosition{"",0,50,0,0}};
    Fakes::MovingObject object1;
    OWL::Interfaces::LaneAssignments objectsList1{{overlap1,&object1}};
    Fakes::MovingObject object2;
    Fakes::MovingObject object3;
    OWL::Interfaces::LaneAssignments objectsList2{{overlap3,&object3}, {overlap2,&object2}};
    Fakes::Road road1;
    std::string idRoad1 = "Road1";
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetRoad()).WillByDefault(ReturnRef(road1));
    ON_CALL(*lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsList1));
    Fakes::Road road2;
    std::string idRoad2 = "Road2";
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    auto [node2, lane2] = laneMultiStream.AddLane(200, true, *node1);
    ON_CALL(*lane2, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsList2));
    ON_CALL(*lane2, GetRoad()).WillByDefault(ReturnRef(road2));
    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(laneMultiStream.Get(), 0, 1000);
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult1{&object1};
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult2{&object1, &object3, &object2};
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, expectedResult1),
                                             std::make_pair(node2->roadGraphVertex, expectedResult2)));
}

TEST(GetObjectsOfTypeInRange, ObjectsOutsideRange_ReturnsOnlyObjectsInRange)
{
    FakeLaneMultiStream laneMultiStream;
    OWL::LaneOverlap overlap1{GlobalRoadPosition{"",0,10,0,0},
                              GlobalRoadPosition{"",0,10,0,0},
                              GlobalRoadPosition{"",0,10,0,0},
                              GlobalRoadPosition{"",0,10,0,0}};
    OWL::LaneOverlap overlap2{GlobalRoadPosition{"",0,110,0,0},
                              GlobalRoadPosition{"",0,110,0,0},
                              GlobalRoadPosition{"",0,110,0,0},
                              GlobalRoadPosition{"",0,110,0,0}};
    OWL::LaneOverlap overlap3{GlobalRoadPosition{"",0,160,0,0},
                              GlobalRoadPosition{"",0,160,0,0},
                              GlobalRoadPosition{"",0,160,0,0},
                              GlobalRoadPosition{"",0,160,0,0}};
    Fakes::MovingObject object1;
    OWL::Interfaces::LaneAssignments objectsList1{{overlap1, &object1}};
    Fakes::MovingObject object2a;
    Fakes::MovingObject object2b;
    OWL::Interfaces::LaneAssignments objectsList2{{overlap2, &object2b}, {overlap1, &object2a}};
    Fakes::MovingObject object3a;
    Fakes::MovingObject object3b;
    OWL::Interfaces::LaneAssignments objectsList3{{overlap2, &object3a}, {overlap3, &object3b}};
    Fakes::Road road1;
    std::string idRoad1 = "Road1";
    ON_CALL(road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    ON_CALL(*lane1, GetRoad()).WillByDefault(ReturnRef(road1));
    ON_CALL(*lane1, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsList1));
    Fakes::Road road2;
    std::string idRoad2 = "Road2";
    ON_CALL(road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    ON_CALL(*lane2, GetWorldObjects(false)).WillByDefault(ReturnRef(objectsList2));
    ON_CALL(*lane2, GetRoad()).WillByDefault(ReturnRef(road2));
    Fakes::Road road3;
    std::string idRoad3 = "Road3";
    ON_CALL(road3, GetId()).WillByDefault(ReturnRef(idRoad3));
    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node1);
    ON_CALL(*lane3, GetWorldObjects(true)).WillByDefault(ReturnRef(objectsList3));
    ON_CALL(*lane3, GetRoad()).WillByDefault(ReturnRef(road3));
    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetObjectsOfTypeInRange<OWL::Interfaces::WorldObject>(laneMultiStream.Get(), 50, 250);
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult1{};
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult2{&object2b};
    std::vector<const OWL::Interfaces::WorldObject*> expectedResult3{&object3a};
    ASSERT_THAT(result, UnorderedElementsAre(std::make_pair(node1->roadGraphVertex, expectedResult1),
                                             std::make_pair(node2->roadGraphVertex, expectedResult2),
                                             std::make_pair(node3->roadGraphVertex, expectedResult3)));
}

/////////////////////////////////////////////////////////////////////////////

TEST(GetLanesOfLaneTypeAtDistance, NoLanesInWorld_ReturnsNoLanes)
{
    std::vector<const OWL::Interfaces::Section*> sections;

    Fakes::Road fakeRoad;
    std::string roadId = "TestRoadId";
    ON_CALL(fakeRoad, GetSections()).WillByDefault(ReturnRef(sections));
    ON_CALL(fakeRoad, GetId()).WillByDefault(ReturnRef(roadId));

    std::unordered_map<std::string, OWL::Interfaces::Road*> roads = { {roadId, &fakeRoad } };

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));

    WorldDataQuery wdQuery(worldData);

    OWL::CLanes lanes = wdQuery.GetLanesOfLaneTypeAtDistance("TestRoadId", 0.0, {LaneType::Driving});

    ASSERT_EQ(lanes.size(), 0);
}

TEST(GetLanesOfLaneTypeAtDistance, ThreeLanesOneLongerThanTwo_ReturnsCorrectListForFirstSection)
{
    FakeLaneManager flm(3, 3, 3.0, {100, 100, 100}, "TestRoadId");
    Fakes::WorldData worldData;

    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));

    ON_CALL(*flm.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][0], GetLaneType()).WillByDefault(Return(LaneType::None));
    ON_CALL(*flm.lanes[2][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][2], GetLaneType()).WillByDefault(Return(LaneType::None));

    WorldDataQuery wdQuery(worldData);

    OWL::CLanes lanes = wdQuery.GetLanesOfLaneTypeAtDistance("TestRoadId", 50.0, {LaneType::Driving});

    ASSERT_EQ(lanes.size(), 3);

    unsigned int i = 0;
    for (const auto& lane : lanes)
    {
        ASSERT_EQ(lane, flm.lanes[0][i++]);
    }
}

TEST(GetLanesOfLaneTypeAtDistance, ThreeLanesOneLongerThanTwo_ReturnsCorrectListForSecondSection)
{
    FakeLaneManager flm(3, 3, 3.0, {100, 100, 100}, "TestRoadId");
    Fakes::WorldData worldData;

    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));

    ON_CALL(*flm.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][0], GetLaneType()).WillByDefault(Return(LaneType::None));
    ON_CALL(*flm.lanes[2][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][2], GetLaneType()).WillByDefault(Return(LaneType::None));

    WorldDataQuery wdQuery(worldData);

    OWL::CLanes lanes = wdQuery.GetLanesOfLaneTypeAtDistance("TestRoadId", 150.0, {LaneType::Driving});

    ASSERT_EQ(lanes.size(), 3);

    unsigned int i = 0;
    for (const auto& lane : lanes)
    {
        ASSERT_EQ(lane, flm.lanes[1][i++]);
    }
}

TEST(GetLanesOfLaneTypeAtDistance, ThreeLanesOneLongerThanTwo_ReturnsCorrectListForThirdSection)
{
    FakeLaneManager flm(3, 3, 3.0, {100, 100, 100}, "TestRoadId");
    Fakes::WorldData worldData;

    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));

    ON_CALL(*flm.lanes[0][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[0][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][0], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[1][2], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][0], GetLaneType()).WillByDefault(Return(LaneType::None));
    ON_CALL(*flm.lanes[2][1], GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(*flm.lanes[2][2], GetLaneType()).WillByDefault(Return(LaneType::None));

    WorldDataQuery wdQuery(worldData);

    OWL::CLanes lanes = wdQuery.GetLanesOfLaneTypeAtDistance("TestRoadId", 250.0, {LaneType::Driving});

    ASSERT_EQ(lanes.size(), 1);
    ASSERT_EQ(lanes.front(), flm.lanes[2][1]);
}

/////////////////////////////////////////////////////////////////////////////

TEST(GetLaneByOdId, CheckFakeLaneManagerImplementation)
{
    FakeLaneManager flm(2, 2, 3.0, {100, 100}, "TestRoadId");

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));

    WorldDataQuery wdQuery(worldData);

    EXPECT_THAT(flm.lanes[0][0]->Exists(), Eq(true));
    ASSERT_EQ(flm.lanes[0][0], &(wdQuery.GetLaneByOdId("TestRoadId", -1, 50)));

    EXPECT_THAT(flm.lanes[1][0]->Exists(), Eq(true));
    ASSERT_EQ(flm.lanes[1][0], &(wdQuery.GetLaneByOdId("TestRoadId", -1, 150)));

    EXPECT_THAT(flm.lanes[0][1]->Exists(), Eq(true));
    ASSERT_EQ(flm.lanes[0][1], &(wdQuery.GetLaneByOdId("TestRoadId", -2, 50)));

    EXPECT_THAT(flm.lanes[1][1]->Exists(), Eq(true));
    ASSERT_EQ(flm.lanes[1][1], &(wdQuery.GetLaneByOdId("TestRoadId", -2, 150)));
}

TEST(GetLaneByOdId, TwoSectionsWithVariableLanes_ReturnsCorrectLanes)
{ 
    std::vector<const OWL::Interfaces::Section*> sections;

    Fakes::Road fakeRoad;
    std::string roadId = "TestRoadId";
    std::unordered_map<std::string, OWL::Interfaces::Road*> roads = { {roadId, &fakeRoad } };
    ON_CALL(fakeRoad, GetSections()).WillByDefault(ReturnRef(sections));
    ON_CALL(fakeRoad, GetId()).WillByDefault(ReturnRef(roadId));

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));

    WorldDataQuery wdQuery(worldData);
    Fakes::Section section1;
    Fakes::Section section2;

    Fakes::Lane lane1_minus1; //Id 1
    Fakes::Lane lane1_minus2; //Id 2
    Fakes::Lane lane2_minus2; //Id 3

    ON_CALL(lane1_minus1, GetId()).WillByDefault(Return(1));
    ON_CALL(lane1_minus2, GetId()).WillByDefault(Return(2));
    ON_CALL(lane2_minus2, GetId()).WillByDefault(Return(3));

    ON_CALL(lane1_minus1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1_minus2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2_minus2, GetOdId()).WillByDefault(Return(-2));

    const std::vector<const OWL::Interfaces::Lane*> lanesSection1 = {{&lane1_minus1, &lane1_minus2}} ;
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanesSection1));
    ON_CALL(section1, Covers(_)).WillByDefault(Return(false));
    ON_CALL(section1, Covers(50)).WillByDefault(Return(true));

    const std::vector<const OWL::Interfaces::Lane*> lanesSection2 = {{&lane2_minus2}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanesSection2));
    ON_CALL(section2, Covers(_)).WillByDefault(Return(false));
    ON_CALL(section2, Covers(150)).WillByDefault(Return(true));

    sections = {{&section1, &section2}};

    ASSERT_EQ(wdQuery.GetLaneByOdId("TestRoadId", -1, 50).GetId(), 1);
    ASSERT_EQ(wdQuery.GetLaneByOdId("TestRoadId", -2, 50).GetId(), 2);
    ASSERT_FALSE(wdQuery.GetLaneByOdId("TestRoadId", -1, 150).Exists());
    ASSERT_EQ(wdQuery.GetLaneByOdId("TestRoadId", -2, 150).GetId(), 3);
}

TEST(GetLaneByOffset, ReturnsCorrectLaneAndOffset)
{
    FakeLaneManager flm(2, 2, 3.0, {100, 100}, "TestRoadId");
    flm.SetWidth(0, 0, 5.0, 50.0);
    flm.SetWidth(1, 0, 4.0, 150.0);
    flm.SetWidth(0, 1, 4.0, 50.0);
    flm.SetWidth(1, 1, 4.0, 150.0);

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(flm.GetRoads()));

    WorldDataQuery wdQuery(worldData);

    auto result1 = wdQuery.GetLaneByOffset("TestRoadId", -2, 50);
    EXPECT_THAT(&result1.first, Eq(flm.lanes[0][0]));
    EXPECT_THAT(result1.second, DoubleEq(0.5));

    auto result2 = wdQuery.GetLaneByOffset("TestRoadId", -2, 150);
    EXPECT_THAT(&result2.first, Eq(flm.lanes[1][0]));
    EXPECT_THAT(result2.second, DoubleEq(0.0));

    auto result3 = wdQuery.GetLaneByOffset("TestRoadId", -6.5, 50);
    EXPECT_THAT(&result3.first, Eq(flm.lanes[0][1]));
    EXPECT_THAT(result3.second, DoubleEq(0.5));

    auto result4 = wdQuery.GetLaneByOffset("TestRoadId", -6.5, 150);
    EXPECT_THAT(&result4.first, Eq(flm.lanes[1][1]));
    EXPECT_THAT(result4.second, DoubleEq(-0.5));
}
/////////////////////////////////////////////////////////////////////////////

TEST(GetValidSOnLane, CheckIfSIsValid_ReturnsTrue)
{
    FakeLaneManager laneManager(3, 1, 3.0, {100, 100, 100}, "TestRoadId");

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));

    WorldDataQuery wdQuery(worldData);

    bool result = wdQuery.IsSValidOnLane("TestRoadId", -1, 50.0);
    ASSERT_TRUE(result);

    result = wdQuery.IsSValidOnLane("TestRoadId", -1, 150.0);
    ASSERT_TRUE(result);

    result = wdQuery.IsSValidOnLane("TestRoadId", -1, 250.0);
    ASSERT_TRUE(result);

    result = wdQuery.IsSValidOnLane("TestRoadId", -1, 0.0);
    ASSERT_TRUE(result);

    result = wdQuery.IsSValidOnLane("TestRoadId", -1, 300.0);
    ASSERT_TRUE(result);
}

TEST(GetValidSOnLane, CheckIfSIsValid_ReturnsFalse)
{
    FakeLaneManager laneManager(1, 2, 3.0, {100}, "TestRoadId");

    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));

    WorldDataQuery wdQuery(worldData);
    bool result = wdQuery.IsSValidOnLane("TestRoadId", -1, 100.1);
    ASSERT_FALSE(result);
}

//////////////////////////////////////////////////////////////////

TEST(GetTrafficSignsInRange, ReturnsCorrectTrafficSigns)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    Fakes::TrafficSign fakeSign1a;
    ON_CALL(fakeSign1a, GetS()).WillByDefault(Return(10.0));
    Fakes::TrafficSign fakeSign1b;
    CommonTrafficSign::Entity specificationSign1b{CommonTrafficSign::Type::Stop, CommonTrafficSign::Unit::None, 60.0, 10.0, 0.0, "", {}};
    ON_CALL(fakeSign1b, GetS()).WillByDefault(Return(60.0));
    ON_CALL(fakeSign1b, GetSpecification(DoubleEq(10.0))).WillByDefault(Return(specificationSign1b));
    OWL::Interfaces::TrafficSigns signsLane1{ {&fakeSign1a, &fakeSign1b} };
    ON_CALL(*lane1, GetTrafficSigns()).WillByDefault(ReturnRef(signsLane1));

    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    Fakes::TrafficSign fakeSign2a;
    ON_CALL(fakeSign2a, GetS()).WillByDefault(Return(110.0));
    Fakes::TrafficSign fakeSign2b;
    CommonTrafficSign::Entity specificationSign2b{CommonTrafficSign::Type::MaximumSpeedLimit, CommonTrafficSign::Unit::MeterPerSecond, 160.0, 90.0, 10.0, "", {}};
    ON_CALL(fakeSign2b, GetS()).WillByDefault(Return(160.0));
    ON_CALL(fakeSign2b, GetSpecification(DoubleEq(90.0))).WillByDefault(Return(specificationSign2b));
    OWL::Interfaces::TrafficSigns signsLane2{ {&fakeSign2a, &fakeSign2b} };
    ON_CALL(*lane2, GetTrafficSigns()).WillByDefault(ReturnRef(signsLane2));

    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node1);
    Fakes::TrafficSign fakeSign3a;
    CommonTrafficSign::Entity specificationSign3a{CommonTrafficSign::Type::TownBegin, CommonTrafficSign::Unit::None, 10.0, 60.0, 0.0, "Town", {}};
    ON_CALL(fakeSign3a, GetS()).WillByDefault(Return(10.0));
    ON_CALL(fakeSign3a, GetSpecification(DoubleEq(60.0))).WillByDefault(Return(specificationSign3a));
    Fakes::TrafficSign fakeSign3b;
    ON_CALL(fakeSign3b, GetS()).WillByDefault(Return(60.0));
    OWL::Interfaces::TrafficSigns signsLane3{ {&fakeSign3a, &fakeSign3b} };
    ON_CALL(*lane3, GetTrafficSigns()).WillByDefault(ReturnRef(signsLane3));

    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetTrafficSignsInRange(laneMultiStream.Get(), 50.0, 100.0);

    ASSERT_THAT(result, SizeIs(3));
    auto& result1 = result[node1->roadGraphVertex];
    ASSERT_THAT(result1, SizeIs(1));
    ASSERT_THAT(result1.at(0).type, Eq(specificationSign1b.type));
    ASSERT_THAT(result1.at(0).distanceToStartOfRoad, Eq(specificationSign1b.distanceToStartOfRoad));
    ASSERT_THAT(result1.at(0).relativeDistance, Eq(specificationSign1b.relativeDistance));

    auto& result2 = result[node2->roadGraphVertex];
    ASSERT_THAT(result2, SizeIs(2));
    ASSERT_THAT(result2.at(0).type, Eq(specificationSign1b.type));
    ASSERT_THAT(result2.at(0).distanceToStartOfRoad, Eq(specificationSign1b.distanceToStartOfRoad));
    ASSERT_THAT(result2.at(0).relativeDistance, Eq(specificationSign1b.relativeDistance));
    ASSERT_THAT(result2.at(1).type, Eq(specificationSign2b.type));
    ASSERT_THAT(result2.at(1).distanceToStartOfRoad, Eq(specificationSign2b.distanceToStartOfRoad));
    ASSERT_THAT(result2.at(1).relativeDistance, Eq(specificationSign2b.relativeDistance));

    auto& result3 = result[node3->roadGraphVertex];
    ASSERT_THAT(result3, SizeIs(2));
    ASSERT_THAT(result3.at(0).type, Eq(specificationSign1b.type));
    ASSERT_THAT(result3.at(0).distanceToStartOfRoad, Eq(specificationSign1b.distanceToStartOfRoad));
    ASSERT_THAT(result3.at(0).relativeDistance, Eq(specificationSign1b.relativeDistance));
    ASSERT_THAT(result3.at(1).type, Eq(specificationSign3a.type));
    ASSERT_THAT(result3.at(1).distanceToStartOfRoad, Eq(specificationSign3a.distanceToStartOfRoad));
    ASSERT_THAT(result3.at(1).relativeDistance, Eq(specificationSign3a.relativeDistance));
}

TEST(GetTrafficSignsInRange, NegativeRange_ReturnsSignsBehind)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    Fakes::TrafficSign fakeSign1a;
    ON_CALL(fakeSign1a, GetS()).WillByDefault(Return(10.0));
    Fakes::TrafficSign fakeSign1b;
    CommonTrafficSign::Entity specificationSign1b{CommonTrafficSign::Type::Stop, CommonTrafficSign::Unit::None, 60.0, -90.0, 0.0, "", {}};
    ON_CALL(fakeSign1b, GetS()).WillByDefault(Return(60.0));
    ON_CALL(fakeSign1b, GetSpecification(DoubleEq(-90.0))).WillByDefault(Return(specificationSign1b));
    OWL::Interfaces::TrafficSigns signsLane1{ {&fakeSign1a, &fakeSign1b} };
    ON_CALL(*lane1, GetTrafficSigns()).WillByDefault(ReturnRef(signsLane1));

    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    Fakes::TrafficSign fakeSign2a;
    ON_CALL(fakeSign2a, GetS()).WillByDefault(Return(110.0));
    Fakes::TrafficSign fakeSign2b;
    CommonTrafficSign::Entity specificationSign2b{CommonTrafficSign::Type::MaximumSpeedLimit, CommonTrafficSign::Unit::MeterPerSecond, 160.0, -10.0, 10.0, "", {}};
    ON_CALL(fakeSign2b, GetS()).WillByDefault(Return(160.0));
    ON_CALL(fakeSign2b, GetSpecification(DoubleEq(-10.0))).WillByDefault(Return(specificationSign2b));
    OWL::Interfaces::TrafficSigns signsLane2{ {&fakeSign2a, &fakeSign2b} };
    ON_CALL(*lane2, GetTrafficSigns()).WillByDefault(ReturnRef(signsLane2));

    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node1);
    Fakes::TrafficSign fakeSign3a;
    ON_CALL(fakeSign3a, GetS()).WillByDefault(Return(10.0));
    Fakes::TrafficSign fakeSign3b;
    ON_CALL(fakeSign3b, GetS()).WillByDefault(Return(60.0));
    OWL::Interfaces::TrafficSigns signsLane3{ {&fakeSign3a, &fakeSign3b} };
    ON_CALL(*lane3, GetTrafficSigns()).WillByDefault(ReturnRef(signsLane3));

    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetTrafficSignsInRange(laneMultiStream.Get(), 150.0, -100.0);

    ASSERT_THAT(result, SizeIs(3));
    auto& result1 = result[node1->roadGraphVertex];
    ASSERT_THAT(result1, SizeIs(1));
    ASSERT_THAT(result1.at(0).type, Eq(specificationSign1b.type));
    ASSERT_THAT(result1.at(0).distanceToStartOfRoad, Eq(specificationSign1b.distanceToStartOfRoad));
    ASSERT_THAT(result1.at(0).relativeDistance, Eq(specificationSign1b.relativeDistance));

    auto& result2 = result[node2->roadGraphVertex];
    ASSERT_THAT(result2, SizeIs(2));
    ASSERT_THAT(result2.at(0).type, Eq(specificationSign1b.type));
    ASSERT_THAT(result2.at(0).distanceToStartOfRoad, Eq(specificationSign1b.distanceToStartOfRoad));
    ASSERT_THAT(result2.at(0).relativeDistance, Eq(specificationSign1b.relativeDistance));
    ASSERT_THAT(result2.at(1).type, Eq(specificationSign2b.type));
    ASSERT_THAT(result2.at(1).distanceToStartOfRoad, Eq(specificationSign2b.distanceToStartOfRoad));
    ASSERT_THAT(result2.at(1).relativeDistance, Eq(specificationSign2b.relativeDistance));
}

TEST(GetRoadMarkingInRange, ReturnsCorrectRoadMarkings)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    Fakes::RoadMarking fakeMarking1a;
    ON_CALL(fakeMarking1a, GetS()).WillByDefault(Return(10.0));
    Fakes::RoadMarking fakeMarking1b;
    CommonTrafficSign::Entity specificationMarking1b{CommonTrafficSign::Type::Stop, CommonTrafficSign::Unit::None, 60.0, 10.0, 0.0, "", {}};
    ON_CALL(fakeMarking1b, GetS()).WillByDefault(Return(60.0));
    ON_CALL(fakeMarking1b, GetSpecification(DoubleEq(10.0))).WillByDefault(Return(specificationMarking1b));
    OWL::Interfaces::RoadMarkings markingsLane1{ {&fakeMarking1a, &fakeMarking1b} };
    ON_CALL(*lane1, GetRoadMarkings()).WillByDefault(ReturnRef(markingsLane1));

    auto [node2, lane2] = laneMultiStream.AddLane(200, false, *node1);
    Fakes::RoadMarking fakeMarking2a;
    ON_CALL(fakeMarking2a, GetS()).WillByDefault(Return(110.0));
    Fakes::RoadMarking fakeMarking2b;
    CommonTrafficSign::Entity specificationMarking2b{CommonTrafficSign::Type::MaximumSpeedLimit, CommonTrafficSign::Unit::MeterPerSecond, 160.0, 90.0, 10.0, "", {}};
    ON_CALL(fakeMarking2b, GetS()).WillByDefault(Return(160.0));
    ON_CALL(fakeMarking2b, GetSpecification(DoubleEq(90.0))).WillByDefault(Return(specificationMarking2b));
    OWL::Interfaces::RoadMarkings markingsLane2{ {&fakeMarking2a, &fakeMarking2b} };
    ON_CALL(*lane2, GetRoadMarkings()).WillByDefault(ReturnRef(markingsLane2));

    auto [node3, lane3] = laneMultiStream.AddLane(300, true, *node1);
    Fakes::RoadMarking fakeMarking3a;
    CommonTrafficSign::Entity specificationMarking3a{CommonTrafficSign::Type::TownBegin, CommonTrafficSign::Unit::None, 10.0, 60.0, 0.0, "Town", {}};
    ON_CALL(fakeMarking3a, GetS()).WillByDefault(Return(10.0));
    ON_CALL(fakeMarking3a, GetSpecification(DoubleEq(60.0))).WillByDefault(Return(specificationMarking3a));
    Fakes::RoadMarking fakeMarking3b;
    ON_CALL(fakeMarking3b, GetS()).WillByDefault(Return(60.0));
    OWL::Interfaces::RoadMarkings markingsLane3{ {&fakeMarking3a, &fakeMarking3b} };
    ON_CALL(*lane3, GetRoadMarkings()).WillByDefault(ReturnRef(markingsLane3));

    Fakes::WorldData worldData;
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetRoadMarkingsInRange(laneMultiStream.Get(), 50.0, 100.0);

    ASSERT_THAT(result, SizeIs(3));
    auto& result1 = result[node1->roadGraphVertex];
    ASSERT_THAT(result1, SizeIs(1));
    ASSERT_THAT(result1.at(0).type, Eq(specificationMarking1b.type));
    ASSERT_THAT(result1.at(0).distanceToStartOfRoad, Eq(specificationMarking1b.distanceToStartOfRoad));
    ASSERT_THAT(result1.at(0).relativeDistance, Eq(specificationMarking1b.relativeDistance));

    auto& result2 = result[node2->roadGraphVertex];
    ASSERT_THAT(result2, SizeIs(2));
    ASSERT_THAT(result2.at(0).type, Eq(specificationMarking1b.type));
    ASSERT_THAT(result2.at(0).distanceToStartOfRoad, Eq(specificationMarking1b.distanceToStartOfRoad));
    ASSERT_THAT(result2.at(0).relativeDistance, Eq(specificationMarking1b.relativeDistance));
    ASSERT_THAT(result2.at(1).type, Eq(specificationMarking2b.type));
    ASSERT_THAT(result2.at(1).distanceToStartOfRoad, Eq(specificationMarking2b.distanceToStartOfRoad));
    ASSERT_THAT(result2.at(1).relativeDistance, Eq(specificationMarking2b.relativeDistance));

    auto& result3 = result[node3->roadGraphVertex];
    ASSERT_THAT(result3, SizeIs(2));
    ASSERT_THAT(result3.at(0).type, Eq(specificationMarking1b.type));
    ASSERT_THAT(result3.at(0).distanceToStartOfRoad, Eq(specificationMarking1b.distanceToStartOfRoad));
    ASSERT_THAT(result3.at(0).relativeDistance, Eq(specificationMarking1b.relativeDistance));
    ASSERT_THAT(result3.at(1).type, Eq(specificationMarking3a.type));
    ASSERT_THAT(result3.at(1).distanceToStartOfRoad, Eq(specificationMarking3a.distanceToStartOfRoad));
    ASSERT_THAT(result3.at(1).relativeDistance, Eq(specificationMarking3a.relativeDistance));
}

TEST(GetLaneMarkings, OneLaneWithOnlyOneBoundaryPerSide)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries {4};
    std::vector<OWL::Id> rightLaneBoundaries {5};
    ON_CALL(*lane1, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));
    ON_CALL(*lane1, GetRightLaneBoundaries()).WillByDefault(Return(rightLaneBoundaries));

    Fakes::LaneBoundary fakeLeftLaneBoundary{};
    ON_CALL(fakeLeftLaneBoundary, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(fakeLeftLaneBoundary, GetSStart()).WillByDefault(Return(0));
    ON_CALL(fakeLeftLaneBoundary, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(fakeLeftLaneBoundary, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(fakeLeftLaneBoundary, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(fakeLeftLaneBoundary, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeRightLaneBoundary{};
    ON_CALL(fakeRightLaneBoundary, GetWidth()).WillByDefault(Return(0.6));
    ON_CALL(fakeRightLaneBoundary, GetSStart()).WillByDefault(Return(0));
    ON_CALL(fakeRightLaneBoundary, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(fakeRightLaneBoundary, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(fakeRightLaneBoundary, GetColor()).WillByDefault(Return(LaneMarking::Color::Blue));
    ON_CALL(fakeRightLaneBoundary, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    ON_CALL(worldData, GetLaneBoundary(4)).WillByDefault(ReturnRef(fakeLeftLaneBoundary));
    ON_CALL(worldData, GetLaneBoundary(5)).WillByDefault(ReturnRef(fakeRightLaneBoundary));

    WorldDataQuery wdQuery(worldData);

    auto resultLeft = wdQuery.GetLaneMarkings(laneMultiStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(resultLeft.at(node1->roadGraphVertex), SizeIs(1));
    auto& resultLeftLaneMarking = resultLeft.at(node1->roadGraphVertex).at(0);
    ASSERT_THAT(resultLeftLaneMarking.type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(resultLeftLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLeftLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLeftLaneMarking.width, DoubleEq(0.5));

    auto resultRight = wdQuery.GetLaneMarkings(laneMultiStream.Get(), 0, 100, Side::Right);
    ASSERT_THAT(resultRight.at(node1->roadGraphVertex), SizeIs(1));
    auto& resultRightLaneMarking = resultRight.at(node1->roadGraphVertex).at(0);
    ASSERT_THAT(resultRightLaneMarking.type, Eq(LaneMarking::Type::Broken));
    ASSERT_THAT(resultRightLaneMarking.color, Eq(LaneMarking::Color::Blue));
    ASSERT_THAT(resultRightLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultRightLaneMarking.width, DoubleEq(0.6));
}

TEST(GetLaneMarkings, OneLaneWithSolidBrokenBoundary)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4, 5} };
    ON_CALL(*lane1, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));

    Fakes::LaneBoundary leftLine{};
    ON_CALL(leftLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(leftLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(leftLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(leftLine, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(leftLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(leftLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Left));

    Fakes::LaneBoundary rightLine{};
    ON_CALL(rightLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(rightLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(rightLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(rightLine, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(rightLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(rightLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Right));

    ON_CALL(worldData, GetLaneBoundary(4)).WillByDefault(ReturnRef(leftLine));
    ON_CALL(worldData, GetLaneBoundary(5)).WillByDefault(ReturnRef(rightLine));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneMultiStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(result.at(node1->roadGraphVertex), SizeIs(1));
    auto& resultLaneMarking = result.at(node1->roadGraphVertex).at(0);
    ASSERT_THAT(resultLaneMarking.type, Eq(LaneMarking::Type::Solid_Broken));
    ASSERT_THAT(resultLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLaneMarking.width, DoubleEq(0.5));
}

TEST(GetLaneMarkings, OneLaneWithBrokenSolidBoundary)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4, 5} };
    ON_CALL(*lane1, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));

    Fakes::LaneBoundary leftLine{};
    ON_CALL(leftLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(leftLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(leftLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(leftLine, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(leftLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(leftLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Left));

    Fakes::LaneBoundary rightLine{};
    ON_CALL(rightLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(rightLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(rightLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(rightLine, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(rightLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(rightLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Right));

    ON_CALL(worldData, GetLaneBoundary(4)).WillByDefault(ReturnRef(leftLine));
    ON_CALL(worldData, GetLaneBoundary(5)).WillByDefault(ReturnRef(rightLine));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneMultiStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(result.at(node1->roadGraphVertex), SizeIs(1));
    auto& resultLaneMarking = result.at(node1->roadGraphVertex).at(0);
    ASSERT_THAT(resultLaneMarking.type, Eq(LaneMarking::Type::Broken_Solid));
    ASSERT_THAT(resultLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLaneMarking.width, DoubleEq(0.5));
}

TEST(GetLaneMarkings, OneLaneWithSolidSolidBoundary)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4, 5} };
    ON_CALL(*lane1, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));

    Fakes::LaneBoundary leftLine{};
    ON_CALL(leftLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(leftLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(leftLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(leftLine, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(leftLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(leftLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Left));

    Fakes::LaneBoundary rightLine{};
    ON_CALL(rightLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(rightLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(rightLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(rightLine, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(rightLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(rightLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Right));

    ON_CALL(worldData, GetLaneBoundary(4)).WillByDefault(ReturnRef(leftLine));
    ON_CALL(worldData, GetLaneBoundary(5)).WillByDefault(ReturnRef(rightLine));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneMultiStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(result.at(node1->roadGraphVertex), SizeIs(1));
    auto& resultLaneMarking = result.at(node1->roadGraphVertex).at(0);
    ASSERT_THAT(resultLaneMarking.type, Eq(LaneMarking::Type::Solid_Solid));
    ASSERT_THAT(resultLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLaneMarking.width, DoubleEq(0.5));
}

TEST(GetLaneMarkings, OneLaneWithBrokenBrokenBoundary)
{
    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLaneBoundaries { {4, 5} };
    ON_CALL(*lane1, GetLeftLaneBoundaries()).WillByDefault(Return(leftLaneBoundaries));

    Fakes::LaneBoundary leftLine{};
    ON_CALL(leftLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(leftLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(leftLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(leftLine, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(leftLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(leftLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Left));

    Fakes::LaneBoundary rightLine{};
    ON_CALL(rightLine, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(rightLine, GetSStart()).WillByDefault(Return(0));
    ON_CALL(rightLine, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(rightLine, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(rightLine, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(rightLine, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Right));

    ON_CALL(worldData, GetLaneBoundary(4)).WillByDefault(ReturnRef(leftLine));
    ON_CALL(worldData, GetLaneBoundary(5)).WillByDefault(ReturnRef(rightLine));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneMultiStream.Get(), 0, 100, Side::Left);
    ASSERT_THAT(result.at(node1->roadGraphVertex), SizeIs(1));
    auto& resultLaneMarking = result.at(node1->roadGraphVertex).at(0);
    ASSERT_THAT(resultLaneMarking.type, Eq(LaneMarking::Type::Broken_Broken));
    ASSERT_THAT(resultLaneMarking.color, Eq(LaneMarking::Color::Yellow));
    ASSERT_THAT(resultLaneMarking.relativeStartDistance, DoubleEq(0.0));
    ASSERT_THAT(resultLaneMarking.width, DoubleEq(0.5));
}

TEST(GetLaneMarkings, TwoLanesWithMultipleBoundaries)
{

    FakeLaneMultiStream laneMultiStream;
    auto [node1, lane1] = laneMultiStream.AddRoot(100, true);
    auto [node2, lane2] = laneMultiStream.AddLane(200, true, *node1);
    Fakes::WorldData worldData;

    std::vector<OWL::Id> leftLane1Boundaries { {4, 5, 6} };
    ON_CALL(*lane1, GetLeftLaneBoundaries()).WillByDefault(Return(leftLane1Boundaries));
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.0));
    std::vector<OWL::Id> leftLane2Boundaries { {10, 11} };
    ON_CALL(*lane2, GetLeftLaneBoundaries()).WillByDefault(Return(leftLane2Boundaries));
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(100.0));

    Fakes::LaneBoundary fakeLaneBoundary4{};
    ON_CALL(fakeLaneBoundary4, GetWidth()).WillByDefault(Return(0.5));
    ON_CALL(fakeLaneBoundary4, GetSStart()).WillByDefault(Return(0));
    ON_CALL(fakeLaneBoundary4, GetSEnd()).WillByDefault(Return(20));
    ON_CALL(fakeLaneBoundary4, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(fakeLaneBoundary4, GetColor()).WillByDefault(Return(LaneMarking::Color::Yellow));
    ON_CALL(fakeLaneBoundary4, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeLaneBoundary5{};
    ON_CALL(fakeLaneBoundary5, GetWidth()).WillByDefault(Return(0.6));
    ON_CALL(fakeLaneBoundary5, GetSStart()).WillByDefault(Return(20));
    ON_CALL(fakeLaneBoundary5, GetSEnd()).WillByDefault(Return(60));
    ON_CALL(fakeLaneBoundary5, GetType()).WillByDefault(Return(LaneMarking::Type::Broken));
    ON_CALL(fakeLaneBoundary5, GetColor()).WillByDefault(Return(LaneMarking::Color::Blue));
    ON_CALL(fakeLaneBoundary5, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeLaneBoundary6{};
    ON_CALL(fakeLaneBoundary6, GetWidth()).WillByDefault(Return(0.7));
    ON_CALL(fakeLaneBoundary6, GetSStart()).WillByDefault(Return(60));
    ON_CALL(fakeLaneBoundary6, GetSEnd()).WillByDefault(Return(100));
    ON_CALL(fakeLaneBoundary6, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(fakeLaneBoundary6, GetColor()).WillByDefault(Return(LaneMarking::Color::White));
    ON_CALL(fakeLaneBoundary6, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeLaneBoundary10{};
    ON_CALL(fakeLaneBoundary10, GetWidth()).WillByDefault(Return(0.8));
    ON_CALL(fakeLaneBoundary10, GetSStart()).WillByDefault(Return(100));
    ON_CALL(fakeLaneBoundary10, GetSEnd()).WillByDefault(Return(150));
    ON_CALL(fakeLaneBoundary10, GetType()).WillByDefault(Return(LaneMarking::Type::None));
    ON_CALL(fakeLaneBoundary10, GetColor()).WillByDefault(Return(LaneMarking::Color::White));
    ON_CALL(fakeLaneBoundary10, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    Fakes::LaneBoundary fakeLaneBoundary11{};
    ON_CALL(fakeLaneBoundary11, GetWidth()).WillByDefault(Return(0.9));
    ON_CALL(fakeLaneBoundary11, GetSStart()).WillByDefault(Return(150));
    ON_CALL(fakeLaneBoundary11, GetSEnd()).WillByDefault(Return(200));
    ON_CALL(fakeLaneBoundary11, GetType()).WillByDefault(Return(LaneMarking::Type::Solid));
    ON_CALL(fakeLaneBoundary11, GetColor()).WillByDefault(Return(LaneMarking::Color::Green));
    ON_CALL(fakeLaneBoundary11, GetSide()).WillByDefault(Return(OWL::LaneMarkingSide::Single));

    ON_CALL(worldData, GetLaneBoundary(4)).WillByDefault(ReturnRef(fakeLaneBoundary4));
    ON_CALL(worldData, GetLaneBoundary(5)).WillByDefault(ReturnRef(fakeLaneBoundary5));
    ON_CALL(worldData, GetLaneBoundary(6)).WillByDefault(ReturnRef(fakeLaneBoundary6));
    ON_CALL(worldData, GetLaneBoundary(10)).WillByDefault(ReturnRef(fakeLaneBoundary10));
    ON_CALL(worldData, GetLaneBoundary(11)).WillByDefault(ReturnRef(fakeLaneBoundary11));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetLaneMarkings(laneMultiStream.Get(), 50, 90, Side::Left);
    const auto& result1 = result.at(node1->roadGraphVertex);
    ASSERT_THAT(result1, SizeIs(2));
    auto& result1LaneMarking0 = result1.at(0);
    ASSERT_THAT(result1LaneMarking0.type, Eq(LaneMarking::Type::Broken));
    ASSERT_THAT(result1LaneMarking0.color, Eq(LaneMarking::Color::Blue));
    ASSERT_THAT(result1LaneMarking0.relativeStartDistance, DoubleEq(-30.0));
    ASSERT_THAT(result1LaneMarking0.width, DoubleEq(0.6));
    auto& result1LaneMarking1 = result1.at(1);
    ASSERT_THAT(result1LaneMarking1.type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(result1LaneMarking1.color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(result1LaneMarking1.relativeStartDistance, DoubleEq(10.0));
    ASSERT_THAT(result1LaneMarking1.width, DoubleEq(0.7));

    const auto& result2 = result.at(node2->roadGraphVertex);
    ASSERT_THAT(result2, SizeIs(3));
    auto& result2LaneMarking0 = result2.at(0);
    ASSERT_THAT(result2LaneMarking0.type, Eq(LaneMarking::Type::Broken));
    ASSERT_THAT(result2LaneMarking0.color, Eq(LaneMarking::Color::Blue));
    ASSERT_THAT(result2LaneMarking0.relativeStartDistance, DoubleEq(-30.0));
    ASSERT_THAT(result2LaneMarking0.width, DoubleEq(0.6));
    auto& result2LaneMarking1 = result2.at(1);
    ASSERT_THAT(result2LaneMarking1.type, Eq(LaneMarking::Type::Solid));
    ASSERT_THAT(result2LaneMarking1.color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(result2LaneMarking1.relativeStartDistance, DoubleEq(10.0));
    ASSERT_THAT(result2LaneMarking1.width, DoubleEq(0.7));
    auto& result2LaneMarking2 = result2.at(2);
    ASSERT_THAT(result2LaneMarking2.type, Eq(LaneMarking::Type::None));
    ASSERT_THAT(result2LaneMarking2.color, Eq(LaneMarking::Color::White));
    ASSERT_THAT(result2LaneMarking2.relativeStartDistance, DoubleEq(50.0));
    ASSERT_THAT(result2LaneMarking2.width, DoubleEq(0.8));
}
/*
TEST(CreateLaneStream, OnlyUniqueConnectionsInStreamDirection)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, "Road1"};
    Fakes::Lane& startLane = fakelm1.GetLane(0, 0);
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto& road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, "Road2"};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto& road2Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(0, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes2_1));
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_2));

    std::vector<RouteElement> route {{"Road1", true}, {"Road2", true}};
    std::unordered_map<std::string, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 0.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&startLane));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(21.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(33.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(true));
}

TEST(CreateLaneStream, SecondRoadAgainstStreamDirection)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, "Road1"};
    Fakes::Lane& startLane = fakelm1.GetLane(0, 0);
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, "Road2"};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_2 {fakelm1.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes2_2));
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes2_1));

    std::vector<RouteElement> route {{"Road1", true}, {"Road2", false}};
    std::unordered_map<std::string, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 0.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&startLane));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(34.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(false));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(46.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(false));
}

TEST(CreateLaneStream, MultipleConnections)
{
    FakeLaneManager fakelm1{1, 2, 3.0, {10.0}, "Road1"};
    Fakes::Lane& startLane = fakelm1.GetLane(0, 0);
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{1, 2, 3.0, {11.0}, "Road2"};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    FakeLaneManager fakelm3{1, 2, 3.0, {12.0}, "Road3"};
    auto road3 = fakelm3.GetRoads().cbegin()->second;
    auto road3Id = fakelm3.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm2.GetLane(0, 0).GetId(), fakelm3.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_1));
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes3_1 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm3.GetLane(0, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes3_1));

    std::vector<RouteElement> route {{"Road1", true}, {"Road3", true}};
    std::unordered_map<std::string, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}, {road3Id, road3}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 0.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(2));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&startLane));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm3.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
}

TEST(CreateLaneStream, StartOnSecondLane)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, "Road1"};
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, "Road2"};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(0, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes2_1));
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_2));

    std::vector<RouteElement> route {{"Road1", true}, {"Road2", true}};
    std::unordered_map<std::string, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{ {fakelm1.GetLane(0, 0).GetId(), &fakelm1.GetLane(0, 0)} };
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 15.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&fakelm1.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(21.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(33.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(true));
}

TEST(CreateLaneStream, StartOnSecondRoad)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, "Road1"};
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, "Road2"};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    FakeLaneManager fakelm3{2, 2, 3.0, {14.0, 15.0}, "Road3"};
    auto road3 = fakelm2.GetRoads().cbegin()->second;
    auto road3Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(0, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes2_1));
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_2));

    std::vector<RouteElement> route {{"Road1", true}, {"Road2", true}};
    std::unordered_map<std::string, OWL::Road*> roads {{road1Id, road1}, {road2Id, road2}, {road3Id, road3}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::unordered_map<OWL::Id, OWL::Interfaces::Lane*> lanes{ {fakelm1.GetLane(0, 0).GetId(), &fakelm1.GetLane(0, 0)},
                                                               {fakelm1.GetLane(1, 0).GetId(), &fakelm1.GetLane(1, 0)},
                                                               {fakelm3.GetLane(1, 0).GetId(), &fakelm3.GetLane(1, 0)}};
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road2", -1, 1.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&fakelm1.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(21.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(33.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(true));
}

TEST(CreateLaneStream, ExcessRoudsInRoute)
{
    FakeLaneManager fakelm1{2, 2, 3.0, {10.0, 11.0}, "Road1"};
    Fakes::Lane& startLane = fakelm1.GetLane(0, 0);
    auto road1 = fakelm1.GetRoads().cbegin()->second;
    auto road1Id = fakelm1.GetRoads().cbegin()->first;
    FakeLaneManager fakelm2{2, 2, 3.0, {12.0, 13.0}, "Road2"};
    auto road2 = fakelm2.GetRoads().cbegin()->second;
    auto road2Id = fakelm2.GetRoads().cbegin()->first;
    std::vector<OWL::Id> successorsLanes1_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm1.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_1));
    std::vector<OWL::Id> successorsLanes1_2 {fakelm2.GetLane(0, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes1_2 {fakelm1.GetLane(0, 0).GetId()};
    ON_CALL(fakelm1.GetLane(1, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes1_2));
    ON_CALL(fakelm1.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes1_2));
    std::vector<OWL::Id> successorsLanes2_1 {fakelm2.GetLane(1, 0).GetId()};
    std::vector<OWL::Id> predecessorsLanes2_1 {fakelm1.GetLane(1, 0).GetId()};
    ON_CALL(fakelm2.GetLane(0, 0), GetNext()).WillByDefault(ReturnRef(successorsLanes2_1));
    ON_CALL(fakelm2.GetLane(0, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_1));
    std::vector<OWL::Id> predecessorsLanes2_2 {fakelm2.GetLane(0, 0).GetId()};
    ON_CALL(fakelm2.GetLane(1, 0), GetPrevious()).WillByDefault(ReturnRef(predecessorsLanes2_2));
    Fakes::Road road0, road3;
    std::string road0Id = "Road0", road3Id = "Road3";
    ON_CALL(road3, GetId()).WillByDefault(ReturnRef(road3Id));
    std::vector<const OWL::Interfaces::Section*> emptySections{};
    ON_CALL(road3, GetSections()).WillByDefault(ReturnRef(emptySections));

    std::vector<RouteElement> route {{"Road0", true}, {"Road1", true}, {"Road2", true}, {"Road3", true}};
    std::unordered_map<std::string, OWL::Road*> roads {{road0Id, &road0}, {road1Id, road1}, {road2Id, road2}, {road3Id, &road3}};
    Fakes::WorldData worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    WorldDataQuery wdQuery(worldData);

    auto laneStream = wdQuery.CreateLaneStream(route, "Road1", -1, 0.0);
    auto lanesInStream = laneStream->GetElements();
    ASSERT_THAT(lanesInStream.size(), Eq(4));
    ASSERT_THAT(lanesInStream.at(0).element, Eq(&startLane));
    ASSERT_THAT(lanesInStream.at(0).sOffset, DoubleEq(0.0));
    ASSERT_THAT(lanesInStream.at(0).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(1).element, Eq(&fakelm1.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(1).sOffset, DoubleEq(10.0));
    ASSERT_THAT(lanesInStream.at(1).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(2).element, Eq(&fakelm2.GetLane(0,0)));
    ASSERT_THAT(lanesInStream.at(2).sOffset, DoubleEq(21.0));
    ASSERT_THAT(lanesInStream.at(2).inStreamDirection, Eq(true));
    ASSERT_THAT(lanesInStream.at(3).element, Eq(&fakelm2.GetLane(1,0)));
    ASSERT_THAT(lanesInStream.at(3).sOffset, DoubleEq(33.0));
    ASSERT_THAT(lanesInStream.at(3).inStreamDirection, Eq(true));
}
*/
TEST(CreateLaneMultiStream, LinearGraphOneLanePerRoad)
{
    Fakes::WorldData worldData;
    RoadGraph roadGraph;
    auto vertexA = add_vertex(RouteElement{"RoadA", true}, roadGraph);
    auto vertexB = add_vertex(RouteElement{"RoadB", false}, roadGraph);
    auto vertexC = add_vertex(RouteElement{"RoadC", true}, roadGraph);
    add_edge(vertexA, vertexB, roadGraph);
    add_edge(vertexB, vertexC, roadGraph);

    OWL::Id idLaneA = 2;
    OWL::Id idLaneB = 4;
    OWL::Id idLaneC = 6;

    Fakes::Road roadA;
    std::string idRoadA = "RoadA";
    Fakes::Section sectionA;
    Fakes::Lane laneA;
    ON_CALL(roadA, GetId()).WillByDefault(ReturnRef(idRoadA));
    ON_CALL(laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(laneA, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneA, GetRoad()).WillByDefault(ReturnRef(roadA));
    ON_CALL(laneA, GetLength()).WillByDefault(Return(100));
    ON_CALL(laneA, Exists()).WillByDefault(Return(true));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(roadA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{&laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    Fakes::Road roadB;
    Fakes::Lane laneB;
    std::string idRoadB = "RoadB";
    ON_CALL(roadB, GetId()).WillByDefault(ReturnRef(idRoadB));
    ON_CALL(laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(laneB, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneB, GetRoad()).WillByDefault(ReturnRef(roadB));
    ON_CALL(laneB, GetLength()).WillByDefault(Return(150));
    Fakes::Road roadC;
    Fakes::Lane laneC;
    std::string idRoadC = "RoadC";
    ON_CALL(roadC, GetId()).WillByDefault(ReturnRef(idRoadC));
    ON_CALL(laneC, GetId()).WillByDefault(Return(idLaneC));
    ON_CALL(laneC, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneC, GetRoad()).WillByDefault(ReturnRef(roadC));
    ON_CALL(laneC, GetLength()).WillByDefault(Return(200));

    std::vector<OWL::Id> successorsLanesA {idLaneB};
    ON_CALL(laneA, GetNext()).WillByDefault(ReturnRef(successorsLanesA));
    std::vector<OWL::Id> predecessorsLanesB {idLaneC};
    ON_CALL(laneB, GetPrevious()).WillByDefault(ReturnRef(predecessorsLanesB));
    std::vector<OWL::Id> successorsLanesC {};
    ON_CALL(laneC, GetNext()).WillByDefault(ReturnRef(successorsLanesC));

    std::unordered_map<std::string, OWL::Road*> roads {{idRoadA, &roadA}, {idRoadB, &roadB}, {idRoadC, &roadC}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    ON_CALL(worldData, GetLane(idLaneA)).WillByDefault(ReturnRef(laneA));
    ON_CALL(worldData, GetLane(idLaneB)).WillByDefault(ReturnRef(laneB));
    ON_CALL(worldData, GetLane(idLaneC)).WillByDefault(ReturnRef(laneC));
    RoadGraphVertexMapping vertexMapping{{{"RoadA", true}, vertexA}};
    ON_CALL(worldData, GetRoadGraphVertexMapping()).WillByDefault(ReturnRef(vertexMapping));
    WorldDataQuery wdQuery(worldData);

    auto laneMultiStream = wdQuery.CreateLaneMultiStream(roadGraph, vertexA, -1, 0.0);
    const auto& root = laneMultiStream->GetRoot();
    ASSERT_THAT(root.roadGraphVertex, Eq(vertexA));
    ASSERT_THAT(root.element->element, Eq(&laneA));
    ASSERT_THAT(root.element->sOffset, DoubleEq(0.));
    ASSERT_THAT(root.element->inStreamDirection, Eq(true));
    ASSERT_THAT(root.next, SizeIs(1));
    const auto& node1 = root.next.front();
    ASSERT_THAT(node1.roadGraphVertex, Eq(vertexB));
    ASSERT_THAT(node1.element->element, Eq(&laneB));
    ASSERT_THAT(node1.element->sOffset, DoubleEq(250.));
    ASSERT_THAT(node1.element->inStreamDirection, Eq(false));
    ASSERT_THAT(node1.next, SizeIs(1));
    const auto& node2 = node1.next.front();
    ASSERT_THAT(node2.roadGraphVertex, Eq(vertexC));
    ASSERT_THAT(node2.element->element, Eq(&laneC));
    ASSERT_THAT(node2.element->sOffset, DoubleEq(250.));
    ASSERT_THAT(node2.element->inStreamDirection, Eq(true));
    ASSERT_THAT(node2.next, IsEmpty());
}

TEST(CreateLaneMultiStream, LinearGraphOneRoadWithThreeLanes)
{
    Fakes::WorldData worldData;
    RoadGraph roadGraph;
    auto vertexA = add_vertex(RouteElement{"RoadA", true}, roadGraph);

    OWL::Id idLaneA = 2;
    OWL::Id idLaneB = 4;
    OWL::Id idLaneC = 6;

    Fakes::Road roadA;
    Fakes::Section sectionA;
    Fakes::Lane laneA;
    std::string idRoadA = "RoadA";
    ON_CALL(roadA, GetId()).WillByDefault(ReturnRef(idRoadA));
    ON_CALL(laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(laneA, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneA, GetRoad()).WillByDefault(ReturnRef(roadA));
    ON_CALL(laneA, GetLength()).WillByDefault(Return(100));
    ON_CALL(laneA, Exists()).WillByDefault(Return(true));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(roadA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{&laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    Fakes::Lane laneB;
    ON_CALL(laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(laneB, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneB, GetRoad()).WillByDefault(ReturnRef(roadA));
    ON_CALL(laneB, GetLength()).WillByDefault(Return(150));
    Fakes::Lane laneC;
    ON_CALL(laneC, GetId()).WillByDefault(Return(idLaneC));
    ON_CALL(laneC, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneC, GetRoad()).WillByDefault(ReturnRef(roadA));
    ON_CALL(laneC, GetLength()).WillByDefault(Return(200));

    std::vector<OWL::Id> successorsLanesA {idLaneB};
    ON_CALL(laneA, GetNext()).WillByDefault(ReturnRef(successorsLanesA));
    std::vector<OWL::Id> successorsLanesB {idLaneC};
    ON_CALL(laneB, GetNext()).WillByDefault(ReturnRef(successorsLanesB));
    std::vector<OWL::Id> successorsLanesC {};
    ON_CALL(laneC, GetNext()).WillByDefault(ReturnRef(successorsLanesC));

    std::unordered_map<std::string, OWL::Road*> roads {{idRoadA, &roadA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    ON_CALL(worldData, GetLane(idLaneA)).WillByDefault(ReturnRef(laneA));
    ON_CALL(worldData, GetLane(idLaneB)).WillByDefault(ReturnRef(laneB));
    ON_CALL(worldData, GetLane(idLaneC)).WillByDefault(ReturnRef(laneC));
    RoadGraphVertexMapping vertexMapping{{{"RoadA", true}, vertexA}};
    ON_CALL(worldData, GetRoadGraphVertexMapping()).WillByDefault(ReturnRef(vertexMapping));
    WorldDataQuery wdQuery(worldData);

    auto laneMultiStream = wdQuery.CreateLaneMultiStream(roadGraph, vertexA, -1, 0.0);
    const auto& root = laneMultiStream->GetRoot();
    ASSERT_THAT(root.roadGraphVertex, Eq(vertexA));
    ASSERT_THAT(root.element->element, Eq(&laneA));
    ASSERT_THAT(root.element->sOffset, DoubleEq(0.));
    ASSERT_THAT(root.element->inStreamDirection, Eq(true));
    ASSERT_THAT(root.next, SizeIs(1));
    const auto& node1 = root.next.front();
    ASSERT_THAT(node1.roadGraphVertex, Eq(vertexA));
    ASSERT_THAT(node1.element->element, Eq(&laneB));
    ASSERT_THAT(node1.element->sOffset, DoubleEq(100.));
    ASSERT_THAT(node1.element->inStreamDirection, Eq(true));
    ASSERT_THAT(node1.next, SizeIs(1));
    const auto& node2 = node1.next.front();
    ASSERT_THAT(node2.roadGraphVertex, Eq(vertexA));
    ASSERT_THAT(node2.element->element, Eq(&laneC));
    ASSERT_THAT(node2.element->sOffset, DoubleEq(250.));
    ASSERT_THAT(node2.element->inStreamDirection, Eq(true));
    ASSERT_THAT(node2.next, IsEmpty());
}

TEST(CreateLaneMultiStream, LinearGraphLanesDontContinue)
{
    Fakes::WorldData worldData;
    RoadGraph roadGraph;
    auto vertexA = add_vertex(RouteElement{"RoadA", true}, roadGraph);
    auto vertexB = add_vertex(RouteElement{"RoadB", false}, roadGraph);
    auto vertexC = add_vertex(RouteElement{"RoadC", true}, roadGraph);
    add_edge(vertexA, vertexB, roadGraph);
    add_edge(vertexB, vertexC, roadGraph);

    OWL::Id idLaneA = 2;

    Fakes::Road roadA;
    Fakes::Section sectionA;
    Fakes::Lane laneA;
    std::string idRoadA = "RoadA";
    ON_CALL(roadA, GetId()).WillByDefault(ReturnRef(idRoadA));
    ON_CALL(laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(laneA, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneA, GetRoad()).WillByDefault(ReturnRef(roadA));
    ON_CALL(laneA, GetLength()).WillByDefault(Return(100));
    ON_CALL(laneA, Exists()).WillByDefault(Return(true));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(roadA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{&laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));

    std::vector<OWL::Id> successorsLanesA {};
    ON_CALL(laneA, GetNext()).WillByDefault(ReturnRef(successorsLanesA));

    std::unordered_map<std::string, OWL::Road*> roads {{idRoadA, &roadA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    ON_CALL(worldData, GetLane(idLaneA)).WillByDefault(ReturnRef(laneA));
    RoadGraphVertexMapping vertexMapping{{{"RoadA", true}, vertexA}};
    ON_CALL(worldData, GetRoadGraphVertexMapping()).WillByDefault(ReturnRef(vertexMapping));
    WorldDataQuery wdQuery(worldData);

    auto laneMultiStream = wdQuery.CreateLaneMultiStream(roadGraph, vertexA, -1, 0.0);
    const auto& root = laneMultiStream->GetRoot();
    ASSERT_THAT(root.roadGraphVertex, Eq(vertexA));
    ASSERT_THAT(root.element->element, Eq(&laneA));
    ASSERT_THAT(root.element->sOffset, DoubleEq(0.));
    ASSERT_THAT(root.element->inStreamDirection, Eq(true));
    ASSERT_THAT(root.next, SizeIs(1));
    const auto& node1 = root.next.front();
    ASSERT_THAT(node1.roadGraphVertex, Eq(vertexB));
    ASSERT_THAT(node1.element, Eq(std::nullopt));
    ASSERT_THAT(node1.next, SizeIs(1));
    const auto& node2 = node1.next.front();
    ASSERT_THAT(node2.roadGraphVertex, Eq(vertexC));
    ASSERT_THAT(node2.element, Eq(std::nullopt));
    ASSERT_THAT(node2.next, IsEmpty());
}

TEST(CreateLaneMultiStream, BranchingGraphOneLanePerRoad)
{
    Fakes::WorldData worldData;
    RoadGraph roadGraph;
    auto vertexA = add_vertex(RouteElement{"RoadA", true}, roadGraph);
    auto vertexB = add_vertex(RouteElement{"RoadB", false}, roadGraph);
    auto vertexC = add_vertex(RouteElement{"RoadC", true}, roadGraph);
    add_edge(vertexA, vertexB, roadGraph);
    add_edge(vertexA, vertexC, roadGraph);

    OWL::Id idLaneA = 2;
    OWL::Id idLaneB = 4;
    OWL::Id idLaneC = 6;

    Fakes::Road roadA;
    Fakes::Section sectionA;
    Fakes::Lane laneA;
    std::string idRoadA = "RoadA";
    ON_CALL(roadA, GetId()).WillByDefault(ReturnRef(idRoadA));
    ON_CALL(laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(laneA, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneA, GetRoad()).WillByDefault(ReturnRef(roadA));
    ON_CALL(laneA, GetLength()).WillByDefault(Return(100));
    ON_CALL(laneA, Exists()).WillByDefault(Return(true));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(roadA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{&laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    Fakes::Road roadB;
    Fakes::Lane laneB;
    std::string idRoadB = "RoadB";
    ON_CALL(roadB, GetId()).WillByDefault(ReturnRef(idRoadB));
    ON_CALL(laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(laneB, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneB, GetRoad()).WillByDefault(ReturnRef(roadB));
    ON_CALL(laneB, GetLength()).WillByDefault(Return(150));
    Fakes::Road roadC;
    Fakes::Lane laneC;
    std::string idRoadC = "RoadC";
    ON_CALL(roadC, GetId()).WillByDefault(ReturnRef(idRoadC));
    ON_CALL(laneC, GetId()).WillByDefault(Return(idLaneC));
    ON_CALL(laneC, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(laneC, GetRoad()).WillByDefault(ReturnRef(roadC));
    ON_CALL(laneC, GetLength()).WillByDefault(Return(200));

    std::vector<OWL::Id> successorsLanesA {idLaneB, idLaneC};
    ON_CALL(laneA, GetNext()).WillByDefault(ReturnRef(successorsLanesA));
    std::vector<OWL::Id> predecessorsLanesB {};
    ON_CALL(laneB, GetPrevious()).WillByDefault(ReturnRef(predecessorsLanesB));
    std::vector<OWL::Id> successorsLanesC {};
    ON_CALL(laneC, GetNext()).WillByDefault(ReturnRef(successorsLanesC));

    std::unordered_map<std::string, OWL::Road*> roads {{idRoadA, &roadA}, {idRoadB, &roadB}, {idRoadC, &roadC}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    ON_CALL(worldData, GetLane(idLaneA)).WillByDefault(ReturnRef(laneA));
    ON_CALL(worldData, GetLane(idLaneB)).WillByDefault(ReturnRef(laneB));
    ON_CALL(worldData, GetLane(idLaneC)).WillByDefault(ReturnRef(laneC));
    RoadGraphVertexMapping vertexMapping{{{"RoadA", true}, vertexA}};
    ON_CALL(worldData, GetRoadGraphVertexMapping()).WillByDefault(ReturnRef(vertexMapping));
    WorldDataQuery wdQuery(worldData);

    auto laneMultiStream = wdQuery.CreateLaneMultiStream(roadGraph, vertexA, -1, 0.0);
    const auto& root = laneMultiStream->GetRoot();
    ASSERT_THAT(root.roadGraphVertex, Eq(vertexA));
    ASSERT_THAT(root.element->element, Eq(&laneA));
    ASSERT_THAT(root.element->sOffset, DoubleEq(0.));
    ASSERT_THAT(root.element->inStreamDirection, Eq(true));
    ASSERT_THAT(root.next, SizeIs(2));
    const auto& node1 = root.next.front();
    ASSERT_THAT(node1.roadGraphVertex, Eq(vertexB));
    ASSERT_THAT(node1.element->element, Eq(&laneB));
    ASSERT_THAT(node1.element->sOffset, DoubleEq(250.));
    ASSERT_THAT(node1.element->inStreamDirection, Eq(false));
    ASSERT_THAT(node1.next, IsEmpty());
    const auto& node2 = root.next.back();
    ASSERT_THAT(node2.roadGraphVertex, Eq(vertexC));
    ASSERT_THAT(node2.element->element, Eq(&laneC));
    ASSERT_THAT(node2.element->sOffset, DoubleEq(100.));
    ASSERT_THAT(node2.element->inStreamDirection, Eq(true));
    ASSERT_THAT(node2.next, IsEmpty());
}

TEST(GetIntersectingConnections, NoIntersectionsOneEntireJunction_ReturnsEmptyVector)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA;
    OWL::Interfaces::Roads connections{&connectionA};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::string idJunction{"Junction"}, idConnectionA{"ConnectionA"};
    ON_CALL(connectionA, GetId()).WillByDefault(ReturnRef(idConnectionA));
    std::map<std::string, std::vector<IntersectionInfo>> intersections{};
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::map<std::string, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<std::string, OWL::Road*> roads{ {idConnectionA, &connectionA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result, IsEmpty());
}

TEST(GetIntersectingConnections, NoIntersectionsForRoad_ReturnsEmptyVector)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA;
    OWL::Interfaces::Roads connections{&connectionA};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::string idJunction{"Junction"}, idConnectionA{"ConnectionA"};
    ON_CALL(connectionA, GetId()).WillByDefault(ReturnRef(idConnectionA));
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectionA", {} } };
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::map<std::string, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<std::string, Road*> roads{ {idConnectionA, &connectionA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result.empty(), Eq(true));
}

TEST(GetIntersectingConnections, OneIntersectionWithLowerPriority_ReturnsIntersectionWithCorrectPriority)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA, connectionB;
    OWL::Interfaces::Roads connections{&connectionA, &connectionB};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::string idJunction{"Junction"}, idConnectionA{"ConnectionA"}, idConnectionB{"ConnectionB"};
    ON_CALL(connectionA, GetId()).WillByDefault(ReturnRef(idConnectionA));
    ON_CALL(connectionB, GetId()).WillByDefault(ReturnRef(idConnectionB));
    IntersectionInfo intersection{idConnectionB, IntersectingConnectionRank::Lower, {} };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectionA", {intersection} } };
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::map<std::string, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<std::string, Road*> roads{ {idConnectionA, &connectionA}, {idConnectionB, &connectionB} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result.size(), Eq(1));
    ASSERT_THAT(result.at(0).id, Eq("ConnectionB"));
    ASSERT_THAT(result.at(0).rank, Eq(IntersectingConnectionRank::Lower));
}

TEST(GetIntersectingConnections, OneIntersectionWithHigherPriority_ReturnsIntersectionWithCorrectPriority)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA, connectionB;
    OWL::Interfaces::Roads connections{&connectionA, &connectionB};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::string idJunction{"Junction"}, idConnectionA{"ConnectionA"}, idConnectionB{"ConnectionB"};
    ON_CALL(connectionA, GetId()).WillByDefault(ReturnRef(idConnectionA));
    ON_CALL(connectionB, GetId()).WillByDefault(ReturnRef(idConnectionB));
    IntersectionInfo intersection{idConnectionB, IntersectingConnectionRank::Higher, {} };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectionA", {intersection} } };
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::map<std::string, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<std::string, Road*> roads{ {idConnectionA, &connectionA}, {idConnectionB, &connectionB} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0).id, Eq("ConnectionB"));
    ASSERT_THAT(result.at(0).rank, Eq(IntersectingConnectionRank::Higher));
}

TEST(GetIntersectingConnections, TwoIntersections_ReturnsBothIntersections)
{
    Fakes::WorldData worldData;
    Fakes::Junction fakeJunction;
    Fakes::Road connectionA, connectionB, connectionC;
    OWL::Interfaces::Roads connections{&connectionA, &connectionB, &connectionC};
    ON_CALL(fakeJunction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::string idJunction{"Junction"}, idConnectionA{"ConnectionA"}, idConnectionB{"ConnectionB"}, idConnectionC{"ConnectionC"};
    ON_CALL(connectionA, GetId()).WillByDefault(ReturnRef(idConnectionA));
    ON_CALL(connectionB, GetId()).WillByDefault(ReturnRef(idConnectionB));
    ON_CALL(connectionC, GetId()).WillByDefault(ReturnRef(idConnectionC));
    IntersectionInfo intersection1{idConnectionB, IntersectingConnectionRank::Lower, {} };
    IntersectionInfo intersection2{idConnectionC, IntersectingConnectionRank::Higher, {} };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectionA", {intersection1, intersection2} } };
    ON_CALL(fakeJunction, GetIntersections()).WillByDefault(ReturnRef(intersections));
    std::map<std::string, Junction*> junctions{ {idJunction, &fakeJunction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::unordered_map<std::string, Road*> roads{ {idConnectionA, &connectionA}, {idConnectionB, &connectionB}, {idConnectionC, &connectionC} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetIntersectingConnections("ConnectionA");
    ASSERT_THAT(result, SizeIs(2));
    ASSERT_THAT(result, UnorderedElementsAre(IntersectingConnection{"ConnectionB", IntersectingConnectionRank::Lower},
                                             IntersectingConnection{"ConnectionC", IntersectingConnectionRank::Higher}));
}

class GetMovingObjectsInRangeOfJunctionConnection : public ::testing::Test
{
public:
    GetMovingObjectsInRangeOfJunctionConnection()
    {
        ON_CALL(junction, GetId()).WillByDefault(ReturnRef(idJunction));
        ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
        ON_CALL(connectingRoad, GetId()).WillByDefault(ReturnRef(idConnectingRoad));
        ON_CALL(incomingRoad, GetId()).WillByDefault(ReturnRef(idIncomingRoad));
        ON_CALL(connectingRoad, IsInStreamDirection()).WillByDefault(Return(true));
        ON_CALL(connectingRoad, GetPredecessor()).WillByDefault(ReturnRef(idIncomingRoad));
        ON_CALL(connectingRoad, GetSections()).WillByDefault(ReturnRef(connectingSections));
        ON_CALL(connectingRoad, GetLength()).WillByDefault(Return(10.0));
        ON_CALL(connectingSection, GetLanes()).WillByDefault(ReturnRef(connectingLanes));
        ON_CALL(connectingSection, Covers(_)).WillByDefault(Return(true));
        ON_CALL(connectingLane1, GetId()).WillByDefault(Return(idConnectingLane1));
        ON_CALL(connectingLane1, GetOdId()).WillByDefault(Return(-1));
        ON_CALL(connectingLane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
        ON_CALL(connectingLane1, GetRoad()).WillByDefault(ReturnRef(connectingRoad));
        ON_CALL(connectingLane1, GetLength()).WillByDefault(Return(10.0));
        ON_CALL(connectingLane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
        ON_CALL(connectingLane1, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(10.));
        ON_CALL(connectingLane1, Exists()).WillByDefault(Return(true));
        ON_CALL(connectingLane2, GetId()).WillByDefault(Return(idConnectingLane2));
        ON_CALL(connectingLane2, GetOdId()).WillByDefault(Return(-2));
        ON_CALL(connectingLane2, GetLaneType()).WillByDefault(Return(LaneType::Driving));
        ON_CALL(connectingLane2, GetRoad()).WillByDefault(ReturnRef(connectingRoad));
        ON_CALL(connectingLane2, GetLength()).WillByDefault(Return(10.0));
        ON_CALL(connectingLane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
        ON_CALL(connectingLane2, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(10.));
        ON_CALL(connectingLane2, Exists()).WillByDefault(Return(true));
        ON_CALL(incomingRoad, GetSections()).WillByDefault(ReturnRef(incomingSections));
        ON_CALL(incomingRoad, IsInStreamDirection()).WillByDefault(Return(true));
        ON_CALL(incomingRoad, GetSuccessor()).WillByDefault(ReturnRef(idJunction));
        ON_CALL(incomingRoad, GetPredecessor()).WillByDefault(ReturnRef(idNoRoad));
        ON_CALL(incomingSection, GetLanes()).WillByDefault(ReturnRef(incomingLanes));
        ON_CALL(incomingSection, Covers(_)).WillByDefault(Return(true));
        ON_CALL(incomingLane1, GetId()).WillByDefault(Return(idIncomingLane1));
        ON_CALL(connectingLane1, GetNext()).WillByDefault(ReturnRef(emptyIds));
        ON_CALL(connectingLane1, GetPrevious()).WillByDefault(ReturnRef(predecessorsConnectingLane1));
        ON_CALL(incomingLane1, GetNext()).WillByDefault(ReturnRef(successorsIncomingLane1));
        ON_CALL(incomingLane1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
        ON_CALL(incomingLane1, GetRoad()).WillByDefault(ReturnRef(incomingRoad));
        ON_CALL(incomingLane1, GetLength()).WillByDefault(Return(1000.0));
        ON_CALL(incomingLane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
        ON_CALL(incomingLane1, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(1000.));
        ON_CALL(incomingLane1, GetOdId()).WillByDefault(Return(-1));
        ON_CALL(incomingLane1, Exists()).WillByDefault(Return(true));
        ON_CALL(incomingLane2, GetId()).WillByDefault(Return(idIncomingLane2));
        ON_CALL(connectingLane2, GetNext()).WillByDefault(ReturnRef(emptyIds));
        ON_CALL(connectingLane2, GetPrevious()).WillByDefault(ReturnRef(predecessorsConnectingLane2));
        ON_CALL(incomingLane2, GetNext()).WillByDefault(ReturnRef(successorsIncomingLane2));
        ON_CALL(incomingLane2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
        ON_CALL(incomingLane2, GetRoad()).WillByDefault(ReturnRef(incomingRoad));
        ON_CALL(incomingLane2, GetLength()).WillByDefault(Return(1000.0));
        ON_CALL(incomingLane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(0.));
        ON_CALL(incomingLane2, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(1000.));
        ON_CALL(incomingLane2, GetOdId()).WillByDefault(Return(-2));
        ON_CALL(incomingLane2, Exists()).WillByDefault(Return(true));

        ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
        ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
        ON_CALL(worldData, GetLane(idConnectingLane1)).WillByDefault(ReturnRef(connectingLane1));
        ON_CALL(worldData, GetLane(idConnectingLane2)).WillByDefault(ReturnRef(connectingLane2));
        ON_CALL(worldData, GetLane(idIncomingLane1)).WillByDefault(ReturnRef(incomingLane1));
        ON_CALL(worldData, GetLane(idIncomingLane2)).WillByDefault(ReturnRef(incomingLane2));
    }

    Fakes::WorldData worldData;
    Fakes::Road connectingRoad, incomingRoad;
    Fakes::Section connectingSection, incomingSection;
    OWL::Id idConnectingLane1 = 11, idConnectingLane2 = 12, idIncomingLane1 = 13, idIncomingLane2 = 14;
    OWL::Fakes::Lane connectingLane1;
    OWL::Fakes::Lane connectingLane2;
    OWL::Fakes::Lane incomingLane1;
    OWL::Fakes::Lane incomingLane2;
    Fakes::Junction junction;
    std::string idConnectingRoad = "Connection", idIncomingRoad = "IncomingRoad", idJunction = "Junction", idNoRoad="";
    std::vector<const OWL::Interfaces::Road*> connectingRoads {&connectingRoad};
    std::vector<const OWL::Interfaces::Section*> connectingSections {&connectingSection};
    std::vector<const OWL::Interfaces::Lane*> connectingLanes { {&connectingLane1, &connectingLane2} };
    std::vector<const OWL::Interfaces::Section*> incomingSections {&incomingSection};
    std::vector<const OWL::Interfaces::Lane*> incomingLanes { {&incomingLane1, &incomingLane2} };
    std::unordered_map<std::string, Road*> roads{ {idConnectingRoad, &connectingRoad}, {idIncomingRoad, &incomingRoad} };
    std::map<std::string, Junction*> junctions { {idJunction, &junction} };
    std::vector<OWL::Id> predecessorsConnectingLane1{idIncomingLane1};
    std::vector<OWL::Id> successorsIncomingLane1{idConnectingLane1};
    std::vector<OWL::Id> predecessorsConnectingLane2{idIncomingLane2};
    std::vector<OWL::Id> successorsIncomingLane2{idConnectingLane2};
    std::vector<OWL::Id> emptyIds{};
};

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, NoAgents_ReturnsEmptyVector)
{
    OWL::Interfaces::LaneAssignments emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(connectingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result.empty(), Eq(true));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnConnector_ReturnsThisAgent)
{
    OWL::Fakes::MovingObject object;
    OWL::LaneOverlap overlap{GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0}};
    OWL::Interfaces::LaneAssignments worldObjectsOnConnector{{overlap,&object}};
    OWL::Interfaces::LaneAssignments emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(connectingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(incomingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&object));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, TwoAgentsOnInComingRoad_ReturnsAgentInRange)
{
    OWL::Fakes::MovingObject objectInRange, objectOutsideRange;
    OWL::LaneOverlap overlap1{GlobalRoadPosition{"",0,950,0,0},
                              GlobalRoadPosition{"",0,950,0,0},
                              GlobalRoadPosition{"",0,950,0,0},
                              GlobalRoadPosition{"",0,950,0,0}};
    OWL::LaneOverlap overlap2{GlobalRoadPosition{"",0,850,0,0},
                              GlobalRoadPosition{"",0,850,0,0},
                              GlobalRoadPosition{"",0,850,0,0},
                              GlobalRoadPosition{"",0,850,0,0}};
    OWL::Interfaces::LaneAssignments worldObjectsOnInComingRoad{{overlap1, &objectInRange}, {overlap2, &objectOutsideRange}};
    OWL::Interfaces::LaneAssignments emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(connectingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnInComingRoad));
    ON_CALL(incomingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&objectInRange));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnBothConnectors_ReturnsThisAgentOnlyOnce)
{
    OWL::Fakes::MovingObject object;
    OWL::LaneOverlap overlap{GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0}};
    OWL::Interfaces::LaneAssignments worldObjectsOnConnector{{overlap, &object}};
    OWL::Interfaces::LaneAssignments emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(connectingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(incomingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&object));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnBothIncomingRoads_ReturnsThisAgentOnlyOnce)
{
    OWL::Fakes::MovingObject object;
    OWL::LaneOverlap overlap{GlobalRoadPosition{"",0,950,0,0},
                             GlobalRoadPosition{"",0,950,0,0},
                             GlobalRoadPosition{"",0,950,0,0},
                             GlobalRoadPosition{"",0,950,0,0}};
    OWL::Interfaces::LaneAssignments worldObjectsOnInComingRoad{{overlap, &object}};
    OWL::Interfaces::LaneAssignments emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(connectingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnInComingRoad));
    ON_CALL(incomingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnInComingRoad));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&object));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnConnectorAndIncomingRoads_ReturnsThisAgentOnlyOnce)
{
    OWL::Fakes::MovingObject object;
    OWL::LaneOverlap overlap{GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0}};
    OWL::Interfaces::LaneAssignments worldObjectsOnConnector{{overlap, &object}};
    OWL::Interfaces::LaneAssignments emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(connectingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnConnector));
    ON_CALL(incomingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(1));
    ASSERT_THAT(result.at(0), Eq(&object));
}

TEST_F(GetMovingObjectsInRangeOfJunctionConnection, OneAgentsOnEachConnector_ReturnsBoth)
{
    OWL::Fakes::MovingObject object1, object2;
    OWL::LaneOverlap overlap{GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0},
                             GlobalRoadPosition{"",0,5,0,0}};
    OWL::Interfaces::LaneAssignments worldObjectsOnConnector1{{overlap, &object1}};
    OWL::Interfaces::LaneAssignments worldObjectsOnConnector2{{overlap, &object2}};
    OWL::Interfaces::LaneAssignments emptyWorldObjects;
    ON_CALL(connectingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnConnector1));
    ON_CALL(connectingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(worldObjectsOnConnector2));
    ON_CALL(incomingLane1, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));
    ON_CALL(incomingLane2, GetWorldObjects(true)).WillByDefault(ReturnRef(emptyWorldObjects));

    WorldDataQuery wdQuery(worldData);

    auto result = wdQuery.GetMovingObjectsInRangeOfJunctionConnection("Connection", 100.0);

    ASSERT_THAT(result, SizeIs(2));
    ASSERT_THAT(result, UnorderedElementsAre(&object1, &object2));
}

TEST(GetDistanceUntilMovingObjectEntersConnector, DISABLED_ObjectOnConnector_ReturnsCorrectDistance)
{
    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Junction junction;
    OWL::Fakes::Road connectorA, connectorB, incomingRoad;
    OWL::Fakes::Section sectionA, sectionB;
    std::string idJunction = "Junction", idConnectorA = "ConnectorA", idConnectorB = "ConnectorB", idIncomingRoad = "IncomingRoad";
    OWL::Id idLaneA = 11, idLaneB = 12;
    IdMapping<OWL::Interfaces::Lane> lanes{};
    lanes.emplace(idLaneA, std::make_unique<OWL::Fakes::Lane>());
    lanes.emplace(idLaneB, std::make_unique<OWL::Fakes::Lane>());
    OWL::Fakes::Lane* laneA = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idLaneA).get());
    OWL::Fakes::Lane* laneB = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idLaneB).get());
    ON_CALL(junction, GetId()).WillByDefault(ReturnRef(idJunction));
    OWL::Interfaces::Roads connectingRoads{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
    ON_CALL(connectorA, GetId()).WillByDefault(ReturnRef(idConnectorA));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(connectorA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(*laneA, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*laneA, GetRoad()).WillByDefault(ReturnRef(connectorA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(ReturnRef(idIncomingRoad));
    ON_CALL(connectorB, GetId()).WillByDefault(ReturnRef(idConnectorB));
    OWL::Interfaces::Sections sectionsB{&sectionB};
    ON_CALL(connectorB, GetSections()).WillByDefault(ReturnRef(sectionsB));
    OWL::Interfaces::Lanes lanesB{laneB};
    ON_CALL(sectionB, GetLanes()).WillByDefault(ReturnRef(lanesB));
    ON_CALL(sectionB, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(*laneB, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*laneB, GetRoad()).WillByDefault(ReturnRef(connectorB));
    OWL::Interfaces::Roads connections{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::unordered_map<std::string, OWL::Interfaces::Road*> roads{ {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idIncomingRoad, &incomingRoad} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::map<std::string, OWL::Interfaces::Junction*> junctions{ {idJunction, &junction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    IntersectionInfo intersection{idConnectorB, IntersectingConnectionRank::Undefined, { { {idLaneA, idLaneB}, {10.0, 15.0} } } };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectorA", {intersection} } };
    ON_CALL(junction, GetIntersections()).WillByDefault(ReturnRef(intersections));

    WorldDataQuery wdQuery(worldData);

    auto distance = wdQuery.GetDistanceUntilObjectEntersConnector("ConnectorA", -1, "ConnectorB");
    ASSERT_THAT(distance, Eq(3.0));
}

TEST(GetDistanceUntilMovingObjectEntersConnector, DISABLED_ObjectOnIncomingRoad_ReturnsCorrectDistance)
{
    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Junction junction;
    OWL::Fakes::Road connectorA, connectorB, incomingRoad;
    OWL::Fakes::Section sectionA, sectionB, incomingSection;
    std::string idJunction = "Junction", idConnectorA = "ConnectorA", idConnectorB = "ConnectorB", idIncomingRoad = "IncomingRoad";
    OWL::Id idLaneA = 11, idLaneB = 12, idIncomingLane = 13;
    IdMapping<OWL::Interfaces::Lane> lanes{};
    lanes.emplace(idLaneA, std::make_unique<OWL::Fakes::Lane>());
    lanes.emplace(idLaneB, std::make_unique<OWL::Fakes::Lane>());
    lanes.emplace(idIncomingLane, std::make_unique<OWL::Fakes::Lane>());
    OWL::Fakes::Lane* laneA = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idLaneA).get());
    OWL::Fakes::Lane* laneB = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idLaneB).get());
    OWL::Fakes::Lane* incomingLane = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idIncomingLane).get());
    ON_CALL(junction, GetId()).WillByDefault(ReturnRef(idJunction));
    OWL::Interfaces::Roads connectingRoads{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
    ON_CALL(connectorA, GetId()).WillByDefault(ReturnRef(idConnectorA));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(connectorA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(*laneA, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*laneA, GetRoad()).WillByDefault(ReturnRef(connectorA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(ReturnRef(idIncomingRoad));
    ON_CALL(connectorB, GetId()).WillByDefault(ReturnRef(idConnectorB));
    OWL::Interfaces::Sections sectionsB{&sectionB};
    ON_CALL(connectorB, GetSections()).WillByDefault(ReturnRef(sectionsB));
    OWL::Interfaces::Lanes lanesB{laneB};
    ON_CALL(sectionB, GetLanes()).WillByDefault(ReturnRef(lanesB));
    ON_CALL(sectionB, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(*laneB, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*laneB, GetRoad()).WillByDefault(ReturnRef(connectorB));
    std::vector<OWL::Id> predecessorsLaneB{idIncomingLane};
    ON_CALL(*laneB, GetPrevious()).WillByDefault(ReturnRef(predecessorsLaneB));
    ON_CALL(incomingRoad, GetId()).WillByDefault(ReturnRef(idIncomingRoad));
    OWL::Interfaces::Sections incomingSections{&incomingSection};
    ON_CALL(incomingRoad, GetSections()).WillByDefault(ReturnRef(incomingSections));
    OWL::Interfaces::Lanes incomingLanes{incomingLane};
    ON_CALL(incomingSection, GetLanes()).WillByDefault(ReturnRef(incomingLanes));
    ON_CALL(incomingSection, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*incomingLane, GetId()).WillByDefault(Return(idIncomingLane));
    ON_CALL(*incomingLane, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*incomingLane, GetLength()).WillByDefault(Return(100.0));
    std::vector<OWL::Id> successorsIncomingLane{idLaneB};
    ON_CALL(*incomingLane, GetNext()).WillByDefault(ReturnRef(successorsIncomingLane));
    ON_CALL(*incomingLane, GetRoad()).WillByDefault(ReturnRef(incomingRoad));
    ON_CALL(incomingRoad, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(incomingRoad, GetSuccessor()).WillByDefault(ReturnRef(idJunction));
    OWL::Interfaces::Roads connections{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::unordered_map<std::string, OWL::Interfaces::Road*> roads{ {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idIncomingRoad, &incomingRoad} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::map<std::string, OWL::Interfaces::Junction*> junctions{ {idJunction, &junction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    IntersectionInfo intersection{idConnectorB, IntersectingConnectionRank::Undefined, { { {idLaneA, idLaneB}, {10.0, 15.0} } } };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectorA", {intersection} } };
    ON_CALL(junction, GetIntersections()).WillByDefault(ReturnRef(intersections));

    WorldDataQuery wdQuery(worldData);

    auto distance = wdQuery.GetDistanceUntilObjectEntersConnector("ConnectorA", -1, "ConnectorB");
    ASSERT_THAT(distance, Eq(103.0));
}

TEST(GetDistanceUntilMovingObjectLeavesConnector, DISABLED_ObjectOnConnector_ReturnsCorrectDistance)
{
    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Junction junction;
    OWL::Fakes::Road connectorA, connectorB, incomingRoad;
    OWL::Fakes::Section sectionA, sectionB;
    std::string idJunction = "Junction", idConnectorA = "ConnectorA", idConnectorB = "ConnectorB", idIncomingRoad = "IncomingRoad";
    OWL::Id idLaneA = 11, idLaneB = 12;
    IdMapping<OWL::Interfaces::Lane> lanes{};
    lanes.emplace(idLaneA, std::make_unique<OWL::Fakes::Lane>());
    lanes.emplace(idLaneB, std::make_unique<OWL::Fakes::Lane>());
    OWL::Fakes::Lane* laneA = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idLaneA).get());
    OWL::Fakes::Lane* laneB = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idLaneB).get());
    ON_CALL(junction, GetId()).WillByDefault(ReturnRef(idJunction));
    OWL::Interfaces::Roads connectingRoads{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
    ON_CALL(connectorA, GetId()).WillByDefault(ReturnRef(idConnectorA));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(connectorA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(*laneA, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*laneA, GetRoad()).WillByDefault(ReturnRef(connectorA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(ReturnRef(idIncomingRoad));
    ON_CALL(connectorB, GetId()).WillByDefault(ReturnRef(idConnectorB));
    OWL::Interfaces::Sections sectionsB{&sectionB};
    ON_CALL(connectorB, GetSections()).WillByDefault(ReturnRef(sectionsB));
    OWL::Interfaces::Lanes lanesB{laneB};
    ON_CALL(sectionB, GetLanes()).WillByDefault(ReturnRef(lanesB));
    ON_CALL(sectionB, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(*laneB, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*laneB, GetRoad()).WillByDefault(ReturnRef(connectorB));
    OWL::Interfaces::Roads connections{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::unordered_map<std::string, OWL::Interfaces::Road*> roads{ {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idIncomingRoad, &incomingRoad} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::map<std::string, OWL::Interfaces::Junction*> junctions{ {idJunction, &junction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    IntersectionInfo intersection{idConnectorB, IntersectingConnectionRank::Undefined, { { {idLaneA, idLaneB}, {10.0, 15.0} } } };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectorA", {intersection} } };
    ON_CALL(junction, GetIntersections()).WillByDefault(ReturnRef(intersections));

    WorldDataQuery wdQuery(worldData);

    auto distance = wdQuery.GetDistanceUntilObjectLeavesConnector("ConnectorA", -1, "ConnectorB");
    ASSERT_THAT(distance, Eq(11.0));
}

TEST(GetDistanceUntilMovingObjectLeavesConnector, DISABLED_ObjectOnIncomingRoad_ReturnsCorrectDistance)
{
    OWL::Fakes::WorldData worldData;
    OWL::Fakes::Junction junction;
    OWL::Fakes::Road connectorA, connectorB, incomingRoad;
    OWL::Fakes::Section sectionA, sectionB, incomingSection;
    std::string idJunction = "Junction", idConnectorA = "ConnectorA", idConnectorB = "ConnectorB", idIncomingRoad = "IncomingRoad";
    OWL::Id idLaneA = 11, idLaneB = 12, idIncomingLane = 13;
    IdMapping<OWL::Interfaces::Lane> lanes{};
    lanes.emplace(idLaneA, std::make_unique<OWL::Fakes::Lane>());
    lanes.emplace(idLaneB, std::make_unique<OWL::Fakes::Lane>());
    lanes.emplace(idIncomingLane, std::make_unique<OWL::Fakes::Lane>());
    OWL::Fakes::Lane* laneA = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idLaneA).get());
    OWL::Fakes::Lane* laneB = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idLaneB).get());
    OWL::Fakes::Lane* incomingLane = dynamic_cast<OWL::Fakes::Lane*>(lanes.at(idIncomingLane).get());
    ON_CALL(junction, GetId()).WillByDefault(ReturnRef(idJunction));
    OWL::Interfaces::Roads connectingRoads{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connectingRoads));
    ON_CALL(connectorA, GetId()).WillByDefault(ReturnRef(idConnectorA));
    OWL::Interfaces::Sections sectionsA{&sectionA};
    ON_CALL(connectorA, GetSections()).WillByDefault(ReturnRef(sectionsA));
    OWL::Interfaces::Lanes lanesA{laneA};
    ON_CALL(sectionA, GetLanes()).WillByDefault(ReturnRef(lanesA));
    ON_CALL(sectionA, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*laneA, GetId()).WillByDefault(Return(idLaneA));
    ON_CALL(*laneA, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*laneA, GetRoad()).WillByDefault(ReturnRef(connectorA));
    ON_CALL(connectorB, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(connectorB, GetPredecessor()).WillByDefault(ReturnRef(idIncomingRoad));
    ON_CALL(connectorB, GetId()).WillByDefault(ReturnRef(idConnectorB));
    OWL::Interfaces::Sections sectionsB{&sectionB};
    ON_CALL(connectorB, GetSections()).WillByDefault(ReturnRef(sectionsB));
    OWL::Interfaces::Lanes lanesB{laneB};
    ON_CALL(sectionB, GetLanes()).WillByDefault(ReturnRef(lanesB));
    ON_CALL(sectionB, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*laneB, GetId()).WillByDefault(Return(idLaneB));
    ON_CALL(*laneB, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*laneB, GetRoad()).WillByDefault(ReturnRef(connectorB));
    std::vector<OWL::Id> predecessorsLaneB{idIncomingLane};
    ON_CALL(*laneB, GetPrevious()).WillByDefault(ReturnRef(predecessorsLaneB));
    ON_CALL(incomingRoad, GetId()).WillByDefault(ReturnRef(idIncomingRoad));
    OWL::Interfaces::Sections incomingSections{&incomingSection};
    ON_CALL(incomingRoad, GetSections()).WillByDefault(ReturnRef(incomingSections));
    OWL::Interfaces::Lanes incomingLanes{incomingLane};
    ON_CALL(incomingSection, GetLanes()).WillByDefault(ReturnRef(incomingLanes));
    ON_CALL(incomingSection, Covers(_)).WillByDefault(Return(true));
    ON_CALL(*incomingLane, GetId()).WillByDefault(Return(idIncomingLane));
    ON_CALL(*incomingLane, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(*incomingLane, GetLength()).WillByDefault(Return(100.0));
    std::vector<OWL::Id> successorsInComingLane{idLaneB};
    ON_CALL(*incomingLane, GetNext()).WillByDefault(ReturnRef(successorsInComingLane));
    ON_CALL(*incomingLane, GetRoad()).WillByDefault(ReturnRef(incomingRoad));
    ON_CALL(incomingRoad, IsInStreamDirection()).WillByDefault(Return(true));
    ON_CALL(incomingRoad, GetSuccessor()).WillByDefault(ReturnRef(idJunction));
    OWL::Interfaces::Roads connections{&connectorA, &connectorB};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(connections));
    std::unordered_map<std::string, OWL::Interfaces::Road*> roads{ {idConnectorA, &connectorA}, {idConnectorB, &connectorB}, {idIncomingRoad, &incomingRoad} };
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::map<std::string, OWL::Interfaces::Junction*> junctions{ {idJunction, &junction} };
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    ON_CALL(worldData, GetLanes()).WillByDefault(ReturnRef(lanes));
    IntersectionInfo intersection{idConnectorB, IntersectingConnectionRank::Undefined, { { {idLaneA, idLaneB}, {10.0, 15.0} } } };
    std::map<std::string, std::vector<IntersectionInfo>> intersections{ {"ConnectorA", {intersection} } };
    ON_CALL(junction, GetIntersections()).WillByDefault(ReturnRef(intersections));


    WorldDataQuery wdQuery(worldData);

    auto distance = wdQuery.GetDistanceUntilObjectLeavesConnector("ConnectorA", -1, "ConnectorB");
    ASSERT_THAT(distance, Eq(111.0));
}

TEST(GetDistanceBetweenObjects, LinearStreamObjectOnSameRoad_ReturnsDistanceOnAllNode)
{
    FakeRoadMultiStream roadStream;
    auto [node1, road1] = roadStream.AddRoot(100, true);
    auto [node2, road2] = roadStream.AddRoad(200, false, *node1);
    auto [node3, road3] = roadStream.AddRoad(300, true, *node2);
    Fakes::WorldData worldData;

    std::string idRoad1 = "Road1";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    std::string idRoad2 = "Road2";
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    std::string idRoad3 = "Road3";
    ON_CALL(*road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    GlobalRoadPositions target{{"Road1", {"Road1", 0, 20, 0, 0}}};

    WorldDataQuery wdQuery(worldData);

    const auto distance = wdQuery.GetDistanceBetweenObjects(roadStream.Get(), 11.0, target);

    ASSERT_THAT(distance, ElementsAre(std::make_pair(node1->roadGraphVertex, std::optional<double>{9}),
                                      std::make_pair(node2->roadGraphVertex, std::optional<double>{9}),
                                      std::make_pair(node3->roadGraphVertex, std::optional<double>{9})));
};

TEST(GetDistanceBetweenObjects, LinearStreamObjectOnNextRoad_ReturnsDistanceOnThisAndFollowingNodes)
{
    FakeRoadMultiStream roadStream;
    auto [node1, road1] = roadStream.AddRoot(100, true);
    auto [node2, road2] = roadStream.AddRoad(200, false, *node1);
    auto [node3, road3] = roadStream.AddRoad(300, true, *node2);
    Fakes::WorldData worldData;

    std::string idRoad1 = "Road1";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    std::string idRoad2 = "Road2";
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    std::string idRoad3 = "Road3";
    ON_CALL(*road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    GlobalRoadPositions target{{"Road2", {"Road2", 0, 20, 0, 0}}};

    WorldDataQuery wdQuery(worldData);

    const auto distance = wdQuery.GetDistanceBetweenObjects(roadStream.Get(), 11.0, target);

    ASSERT_THAT(distance, ElementsAre(std::make_pair(node1->roadGraphVertex, std::nullopt),
                                      std::make_pair(node2->roadGraphVertex, std::optional<double>{269}),
                                      std::make_pair(node3->roadGraphVertex, std::optional<double>{269})));
};

TEST(GetDistanceBetweenObjects, LinearStreamObjectBehind_ReturnsNegativeDistance)
{
    FakeRoadMultiStream roadStream;
    auto [node1, road1] = roadStream.AddRoot(100, true);
    auto [node2, road2] = roadStream.AddRoad(200, false, *node1);
    auto [node3, road3] = roadStream.AddRoad(300, true, *node2);
    Fakes::WorldData worldData;

    std::string idRoad1 = "Road1";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    std::string idRoad2 = "Road2";
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    std::string idRoad3 = "Road3";
    ON_CALL(*road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    GlobalRoadPositions target{{"Road1", {"Road1", 0, 20, 0, 0}}};

    WorldDataQuery wdQuery(worldData);

    const auto distance = wdQuery.GetDistanceBetweenObjects(roadStream.Get(), 51.0, target);

    ASSERT_THAT(distance, ElementsAre(std::make_pair(node1->roadGraphVertex, std::optional<double>{-31}),
                                      std::make_pair(node2->roadGraphVertex, std::optional<double>{-31}),
                                      std::make_pair(node3->roadGraphVertex, std::optional<double>{-31})));
};

TEST(GetDistanceBetweenObjects, BranchingStreamObjectOneLeaf_ReturnsDistanceOnThisNode)
{
    FakeRoadMultiStream roadStream;
    auto [node1, road1] = roadStream.AddRoot(100, true);
    auto [node2, road2] = roadStream.AddRoad(200, false, *node1);
    auto [node3, road3] = roadStream.AddRoad(300, true, *node1);
    Fakes::WorldData worldData;

    std::string idRoad1 = "Road1";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    std::string idRoad2 = "Road2";
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    std::string idRoad3 = "Road3";
    ON_CALL(*road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    GlobalRoadPositions target{{"Road2", {"Road2", 0, 20, 0, 0}}};

    WorldDataQuery wdQuery(worldData);

    const auto distance = wdQuery.GetDistanceBetweenObjects(roadStream.Get(), 11.0, target);

    ASSERT_THAT(distance, ElementsAre(std::make_pair(node1->roadGraphVertex, std::nullopt),
                                      std::make_pair(node2->roadGraphVertex, std::optional<double>{269}),
                                      std::make_pair(node3->roadGraphVertex, std::nullopt)));
};

TEST(GetDistanceBetweenObjects, BranchingStreamObjectTwoLeaf_ReturnsDistanceForBoth)
{
    FakeRoadMultiStream roadStream;
    auto [node1, road1] = roadStream.AddRoot(100, true);
    auto [node2, road2] = roadStream.AddRoad(200, false, *node1);
    auto [node3, road3] = roadStream.AddRoad(300, true, *node1);
    Fakes::WorldData worldData;

    std::string idRoad1 = "Road1";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    std::string idRoad2 = "Road2";
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    std::string idRoad3 = "Road3";
    ON_CALL(*road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    GlobalRoadPositions target{{"Road2", {"Road2", 0, 20, 0, 0}}, {"Road3", {"Road3", 0, 40, 0, 0}}};

    WorldDataQuery wdQuery(worldData);

    const auto distance = wdQuery.GetDistanceBetweenObjects(roadStream.Get(), 11.0, target);

    ASSERT_THAT(distance, ElementsAre(std::make_pair(node1->roadGraphVertex, std::nullopt),
                                      std::make_pair(node2->roadGraphVertex, std::optional<double>{269}),
                                      std::make_pair(node3->roadGraphVertex, std::optional<double>{129})));
};

TEST(GetObstruction, ObjectOnSameLane)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneMultiStream laneStream;
    auto [node, lane] = laneStream.AddRoot(1000.0, true);
    OWL::Fakes::Road fakeRoad;
    std::string roadId{"Road"};
    ON_CALL(fakeRoad, GetId()).WillByDefault(ReturnRef(roadId));
    ON_CALL(*lane, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(500.0));
    ON_CALL(*lane, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(1500.0));
    ON_CALL(*lane, GetRoad()).WillByDefault(ReturnRef(fakeRoad));
    Primitive::LaneGeometryJoint::Points sStartPoints{{},{2000.,2002.},{}};
    ON_CALL(*lane, GetInterpolatedPointsAtDistance(510)).WillByDefault(Return(sStartPoints));
    Primitive::LaneGeometryJoint::Points sEndPoints{{},{2005.,2002.},{}};
    ON_CALL(*lane, GetInterpolatedPointsAtDistance(515)).WillByDefault(Return(sEndPoints));

    double tCoordinate{0.0};
    RoadInterval roadInterval{{},
                              GlobalRoadPosition{"",0,510,0,0},
                              GlobalRoadPosition{"",0,515,0,0},
                              GlobalRoadPosition{"",0,510,0,0},
                              GlobalRoadPosition{"",0,510,0,0}};
    RoadIntervals touchedRoads{{"Road", roadInterval}};
    std::map<ObjectPoint, Common::Vector2d> points{{ObjectPointRelative::Leftmost, {2000., 2005.}},
                                                   {ObjectPointRelative::Rightmost, {2000., 2000.}},
                                                   {ObjectPointPredefined::FrontCenter, {2005., 2002.5}}};

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetObstruction(laneStream.Get(), tCoordinate, points, touchedRoads).at(node->roadGraphVertex);
    EXPECT_THAT(result.lateralDistances.at(ObjectPointRelative::Leftmost), Eq(3));
    EXPECT_THAT(result.lateralDistances.at(ObjectPointRelative::Rightmost), Eq(-2));
    EXPECT_THAT(result.lateralDistances.at(ObjectPointPredefined::FrontCenter), Eq(0.5));
}

TEST(GetObstruction, ObjectOnNextLane)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneMultiStream laneStream;
    auto [node1, lane1] = laneStream.AddRoot(1000.0, true);
    auto [node2, lane2] = laneStream.AddLane(1000.0, true, *node1);
    OWL::Fakes::Road fakeRoad;
    std::string roadId{"Road"};
    ON_CALL(fakeRoad, GetId()).WillByDefault(ReturnRef(roadId));
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(500.0));
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(1500.0));
    ON_CALL(*lane1, GetRoad()).WillByDefault(ReturnRef(fakeRoad));
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(1500.0));
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(2500.0));
    ON_CALL(*lane2, GetRoad()).WillByDefault(ReturnRef(fakeRoad));
    Primitive::LaneGeometryJoint::Points sStartPoints{{},{2000.,2002.},{}};
    ON_CALL(*lane2, GetInterpolatedPointsAtDistance(1510)).WillByDefault(Return(sStartPoints));
    Primitive::LaneGeometryJoint::Points sEndPoints{{},{2005.,2002.},{}};
    ON_CALL(*lane2, GetInterpolatedPointsAtDistance(1515)).WillByDefault(Return(sEndPoints));

    double tCoordinate{0.0};
    RoadInterval roadInterval{{},
                              GlobalRoadPosition{"",0,1510,0,0},
                              GlobalRoadPosition{"",0,1515,0,0},
                              GlobalRoadPosition{"",0,1510,0,0},
                              GlobalRoadPosition{"",0,1510,0,0}};
    RoadIntervals touchedRoads{{"Road", roadInterval}};
    std::map<ObjectPoint, Common::Vector2d> points{{ObjectPointRelative::Leftmost, {2000., 2005.}},
                                                   {ObjectPointRelative::Rightmost, {2000., 2000.}},
                                                   {ObjectPointPredefined::FrontCenter, {2005., 2002.5}}};

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetObstruction(laneStream.Get(), tCoordinate, points, touchedRoads).at(node2->roadGraphVertex);
    EXPECT_THAT(result.lateralDistances.at(ObjectPointRelative::Leftmost), Eq(3));
    EXPECT_THAT(result.lateralDistances.at(ObjectPointRelative::Rightmost), Eq(-2));
    EXPECT_THAT(result.lateralDistances.at(ObjectPointPredefined::FrontCenter), Eq(0.5));
}

TEST(GetObstruction, ObjectOnTwoLanes)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneMultiStream laneStream;
    auto [node1, lane1] = laneStream.AddRoot(1000.0, true);
    auto [node2, lane2] = laneStream.AddLane(1000.0, true, *node1);
    OWL::Fakes::Road fakeRoad;
    std::string roadId{"Road"};
    ON_CALL(fakeRoad, GetId()).WillByDefault(ReturnRef(roadId));
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(500.0));
    ON_CALL(*lane1, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(1500.0));
    ON_CALL(*lane1, GetRoad()).WillByDefault(ReturnRef(fakeRoad));
    Primitive::LaneGeometryJoint::Points sStartPoints{{},{2000.,2000.},{}};
    ON_CALL(*lane1, GetInterpolatedPointsAtDistance(1490)).WillByDefault(Return(sStartPoints));
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadStart)).WillByDefault(Return(1500.0));
    ON_CALL(*lane2, GetDistance(OWL::MeasurementPoint::RoadEnd)).WillByDefault(Return(2500.0));
    ON_CALL(*lane2, GetRoad()).WillByDefault(ReturnRef(fakeRoad));
    Primitive::LaneGeometryJoint::Points sEndPoints{{},{2020.,2000.},{}};
    ON_CALL(*lane2, GetInterpolatedPointsAtDistance(1510)).WillByDefault(Return(sEndPoints));

    double tCoordinate{0.0};

    RoadInterval roadInterval{{},
                              GlobalRoadPosition{"",0,1490,0,0},
                              GlobalRoadPosition{"",0,1510,0,0},
                              GlobalRoadPosition{"",0,1500,0,0},
                              GlobalRoadPosition{"",0,1500,0,0}};
    RoadIntervals touchedRoads{{"Road", roadInterval}};
    std::map<ObjectPoint, Common::Vector2d> points{{ObjectPointRelative::Leftmost, {2010., 2010.}},
                                                   {ObjectPointRelative::Rightmost, {2010., 1990.}},
                                                   {ObjectPointPredefined::FrontCenter, {2005., 2005.}}};

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetObstruction(laneStream.Get(), tCoordinate, points, touchedRoads).at(node2->roadGraphVertex);
    EXPECT_THAT(result.lateralDistances.at(ObjectPointRelative::Leftmost), Eq(10));
    EXPECT_THAT(result.lateralDistances.at(ObjectPointRelative::Rightmost), Eq(-10));
    EXPECT_THAT(result.lateralDistances.at(ObjectPointPredefined::FrontCenter), Eq(5));
}

TEST(GetRelativeRoads, OnlyOneRouteNotInJunction_ReturnsOneElement)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, roadA] = roadStream.AddRoot(100.0, true);
    std::string idRoadA = "RoadA";
    ON_CALL(*roadA, GetId()).WillByDefault(ReturnRef(idRoadA));
    std::unordered_map<std::string, OWL::Road*> roads{{idRoadA, roadA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::map<std::string, OWL::Junction*> junctions{};
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeRoads(roadStream.Get(), 0.0, 100.0).at(node->roadGraphVertex);

    EXPECT_THAT(result, SizeIs(1));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    EXPECT_THAT(result.at(0).roadId, Eq("RoadA"));
    EXPECT_THAT(result.at(0).junction, Eq(false));
    EXPECT_THAT(result.at(0).inOdDirection, Eq(true));
}

TEST(GetRelativeRoads, OnlyOneRouteInJunction_ReturnsOneElement)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, roadA] = roadStream.AddRoot(100.0, true);
    std::string idRoadA = "RoadA";

    OWL::Fakes::Junction junction;
    std::string idJunction = "Junction";

    ON_CALL(*roadA, GetId()).WillByDefault(ReturnRef(idRoadA));
    std::unordered_map<std::string, OWL::Road*> roads{{idRoadA, roadA}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::map<std::string, OWL::Junction*> junctions{{idJunction, &junction}};
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::vector<const Road*> roadsOnJunction{roadA};
    ON_CALL(junction, GetConnectingRoads()).WillByDefault(ReturnRef(roadsOnJunction));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeRoads(roadStream.Get(), 0.0, 100.0).at(node->roadGraphVertex);

    EXPECT_THAT(result, SizeIs(1));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    EXPECT_THAT(result.at(0).roadId, Eq("RoadA"));
    EXPECT_THAT(result.at(0).junction, Eq(true));
    EXPECT_THAT(result.at(0).inOdDirection, Eq(true));
}

TEST(GetRelativeRoads, ThreeRoadsInJunction_ReturnsThreeElements)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [nodeA, roadA] = roadStream.AddRoot(100.0, true);
    std::string idRoadA = "RoadA";
    auto [nodeB, roadB] = roadStream.AddRoad(150.0, true, *nodeA);
    std::string idRoadB = "RoadB";
    auto [nodeC, roadC] = roadStream.AddRoad(200.0, false, *nodeB);
    std::string idRoadC = "RoadC";
    auto [nodeD, roadD] = roadStream.AddRoad(250.0, true, *nodeC);
    std::string idRoadD = "RoadD";
    auto [nodeE, roadE] = roadStream.AddRoad(300.0, true, *nodeD);
    std::string idRoadE = "RoadE";

    OWL::Fakes::Junction junctionA;
    std::string idJunctionA = "JunctionA";
    OWL::Fakes::Junction junctionB;
    std::string idJunctionB = "JunctionB";

    ON_CALL(*roadA, GetId()).WillByDefault(ReturnRef(idRoadA));
    ON_CALL(*roadB, GetId()).WillByDefault(ReturnRef(idRoadB));
    ON_CALL(*roadC, GetId()).WillByDefault(ReturnRef(idRoadC));
    ON_CALL(*roadD, GetId()).WillByDefault(ReturnRef(idRoadD));
    std::unordered_map<std::string, OWL::Road*> roads{{idRoadA, roadA}, {idRoadB, roadB}, {idRoadC, roadC}, {idRoadD, roadD}};
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(roads));
    std::map<std::string, OWL::Junction*> junctions{{idJunctionA, &junctionA}, {idJunctionB, &junctionB}};
    ON_CALL(worldData, GetJunctions()).WillByDefault(ReturnRef(junctions));
    std::vector<const Road*> roadsOnJunctionA{roadB};
    ON_CALL(junctionA, GetConnectingRoads()).WillByDefault(ReturnRef(roadsOnJunctionA));
    std::vector<const Road*> roadsOnJunctionB{roadD};
    ON_CALL(junctionB, GetConnectingRoads()).WillByDefault(ReturnRef(roadsOnJunctionB));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeRoads(roadStream.Get(), 110.0, 500.0).at(nodeD->roadGraphVertex);

    EXPECT_THAT(result, SizeIs(3));
    EXPECT_THAT(result.at(0).startS, Eq(-10));
    EXPECT_THAT(result.at(0).endS, Eq(140));
    EXPECT_THAT(result.at(0).roadId, Eq("RoadB"));
    EXPECT_THAT(result.at(0).junction, Eq(true));
    EXPECT_THAT(result.at(0).inOdDirection, Eq(true));
    EXPECT_THAT(result.at(1).startS, Eq(140));
    EXPECT_THAT(result.at(1).endS, Eq(340));
    EXPECT_THAT(result.at(1).roadId, Eq("RoadC"));
    EXPECT_THAT(result.at(1).junction, Eq(false));
    EXPECT_THAT(result.at(1).inOdDirection, Eq(false));
    EXPECT_THAT(result.at(2).startS, Eq(340));
    EXPECT_THAT(result.at(2).endS, Eq(590));
    EXPECT_THAT(result.at(2).roadId, Eq("RoadD"));
    EXPECT_THAT(result.at(2).junction, Eq(true));
    EXPECT_THAT(result.at(2).inOdDirection, Eq(true));
}

TEST(GetRelativeLanes, OnlySectionInDrivingDirection_ReturnsLanesOfThisSection)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, road] = roadStream.AddRoot(100.0, true);
    OWL::Fakes::Lane lane1;
    OWL::Fakes::Lane lane2;
    OWL::Fakes::Lane lane3;
    OWL::Id idLane1 = 1, idLane2 = 2, idLane3 = 3;
    ON_CALL(lane1, GetId()).WillByDefault(Return(idLane1));
    ON_CALL(lane2, GetId()).WillByDefault(Return(idLane2));
    ON_CALL(lane3, GetId()).WillByDefault(Return(idLane3));
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane3, GetOdId()).WillByDefault(Return(1));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane3, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    ON_CALL(lane3, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    OWL::Fakes::Section section;
    OWL::Interfaces::Lanes lanes{{&lane1, &lane2, &lane3}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));
    ON_CALL(section, GetLength()).WillByDefault(Return(100.0));
    std::vector<const OWL::Interfaces::Section*> sections{&section};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 10.0, -1, 300.0, true).at(node->roadGraphVertex);

    ASSERT_THAT(result, SizeIs(1));
    EXPECT_THAT(result.at(0).startS, Eq(-10));
    EXPECT_THAT(result.at(0).endS, Eq(90));
    ASSERT_THAT(result.at(0).lanes, SizeIs(3));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(2).relativeId, Eq(1));
    EXPECT_THAT(result.at(0).lanes.at(2).inDrivingDirection, Eq(false));
    EXPECT_THAT(result.at(0).lanes.at(2).type, Eq(LaneType::Stop));
    EXPECT_THAT(result.at(0).lanes.at(2).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(2).successor, Eq(std::nullopt));
}

TEST(GetRelativeLanes, IncludeOncomingFalse_IgnoresOncomingLanes)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, road] = roadStream.AddRoot(100.0, true);
    OWL::Fakes::Lane lane1;
    OWL::Fakes::Lane lane2;
    OWL::Fakes::Lane lane3;
    OWL::Id idLane1 = 1, idLane2 = 2, idLane3 = 3;
    ON_CALL(lane1, GetId()).WillByDefault(Return(idLane1));
    ON_CALL(lane2, GetId()).WillByDefault(Return(idLane2));
    ON_CALL(lane3, GetId()).WillByDefault(Return(idLane3));
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane3, GetOdId()).WillByDefault(Return(1));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane3, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    ON_CALL(lane3, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    OWL::Fakes::Section section;
    OWL::Interfaces::Lanes lanes{{&lane1, &lane2, &lane3}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));
    ON_CALL(section, GetLength()).WillByDefault(Return(100.0));
    std::vector<const OWL::Interfaces::Section*> sections{&section};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 10.0, -1, 300.0, false).at(node->roadGraphVertex);

    ASSERT_THAT(result, SizeIs(1));
    EXPECT_THAT(result.at(0).startS, Eq(-10));
    EXPECT_THAT(result.at(0).endS, Eq(90));
    ASSERT_THAT(result.at(0).lanes, SizeIs(2));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor, Eq(std::nullopt));
}

TEST(GetRelativeLanes, OnlySectionAgainstDrivingDirection_ReturnsLanesOfThisSection)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, road] = roadStream.AddRoot(100.0, false);
    OWL::Fakes::Lane lane1;
    OWL::Fakes::Lane lane2;
    OWL::Fakes::Lane lane3;
    OWL::Id idLane1 = 1, idLane2 = 2, idLane3 = 3;
    ON_CALL(lane1, GetId()).WillByDefault(Return(idLane1));
    ON_CALL(lane2, GetId()).WillByDefault(Return(idLane2));
    ON_CALL(lane3, GetId()).WillByDefault(Return(idLane3));
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane3, GetOdId()).WillByDefault(Return(1));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane3, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1, GetNext()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2, GetNext()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane3, GetNext()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    ON_CALL(lane3, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    OWL::Fakes::Section section;
    OWL::Interfaces::Lanes lanes{{&lane1, &lane2, &lane3}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));
    ON_CALL(section, GetLength()).WillByDefault(Return(100.0));
    std::vector<const OWL::Interfaces::Section*> sections{&section};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 0.0, -1, 300.0, true).at(node->roadGraphVertex);

    ASSERT_THAT(result, SizeIs(1));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    ASSERT_THAT(result.at(0).lanes, SizeIs(3));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(false));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(false));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(2).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(2).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(2).type, Eq(LaneType::Stop));
    EXPECT_THAT(result.at(0).lanes.at(2).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(2).successor, Eq(std::nullopt));
}

TEST(GetRelativeLanes, TwoSectionsOnSameRoad_ReturnsLanesOfBothSections)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, road] = roadStream.AddRoot(100.0, true);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    ON_CALL(lane1_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane1_2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Fakes::Lane lane2_3;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4, idLane2_3 = 5;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_3, GetId()).WillByDefault(Return(idLane2_3));
    ON_CALL(lane2_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2_2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2_3, GetOdId()).WillByDefault(Return(-3));
    std::vector<OWL::Id> predecessors2_1{idLane1_1};
    std::vector<OWL::Id> predecessors2_2{idLane1_2};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(ReturnRef(predecessors2_1));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(ReturnRef(predecessors2_2));
    ON_CALL(lane2_3, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2_1, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    ON_CALL(lane2_2, GetLaneType()).WillByDefault(Return(LaneType::Entry));
    ON_CALL(lane2_3, GetLaneType()).WillByDefault(Return(LaneType::Shoulder));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2, &lane2_3}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(100.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(200.0));
    std::vector<const OWL::Interfaces::Section*> sections{&section1, &section2};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 0.0, -1, 300.0, true).at(node->roadGraphVertex);

    ASSERT_THAT(result, SizeIs(2));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    ASSERT_THAT(result.at(0).lanes, SizeIs(2));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor.value(), Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor.value(), Eq(-1));
    EXPECT_THAT(result.at(1).startS, Eq(100));
    EXPECT_THAT(result.at(1).endS, Eq(300));
    ASSERT_THAT(result.at(1).lanes, SizeIs(3));
    EXPECT_THAT(result.at(1).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(1).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(0).type, Eq(LaneType::Stop));
    EXPECT_THAT(result.at(1).lanes.at(0).predecessor.value(), Eq(0));
    EXPECT_THAT(result.at(1).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(1).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(1).type, Eq(LaneType::Entry));
    EXPECT_THAT(result.at(1).lanes.at(1).predecessor.value(), Eq(-1));
    EXPECT_THAT(result.at(1).lanes.at(1).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(2).relativeId, Eq(-2));
    EXPECT_THAT(result.at(1).lanes.at(2).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(2).type, Eq(LaneType::Shoulder));
    EXPECT_THAT(result.at(1).lanes.at(2).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(2).successor, Eq(std::nullopt));
}

TEST(GetRelativeLanes, IdOfEgoLaneChanges_ReturnsLanesWithCorrectRelativeId)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, road] = roadStream.AddRoot(100.0, true);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    ON_CALL(lane1_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane1_2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Fakes::Lane lane2_3;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4, idLane2_3 = 5;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_3, GetId()).WillByDefault(Return(idLane2_3));
    ON_CALL(lane2_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2_2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2_3, GetOdId()).WillByDefault(Return(-3));
    std::vector<OWL::Id> predecessors2_2{idLane1_1};
    std::vector<OWL::Id> predecessors2_3{idLane1_2};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(ReturnRef(predecessors2_2));
    ON_CALL(lane2_3, GetPrevious()).WillByDefault(ReturnRef(predecessors2_3));
    ON_CALL(lane2_1, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    ON_CALL(lane2_2, GetLaneType()).WillByDefault(Return(LaneType::Entry));
    ON_CALL(lane2_3, GetLaneType()).WillByDefault(Return(LaneType::Shoulder));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2, &lane2_3}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(100.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(200.0));
    std::vector<const OWL::Interfaces::Section*> sections{&section1, &section2};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 0.0, -1, 300.0, true).at(node->roadGraphVertex);

    ASSERT_THAT(result, SizeIs(2));
    EXPECT_THAT(result.at(0).startS, Eq(0));
    EXPECT_THAT(result.at(0).endS, Eq(100));
    ASSERT_THAT(result.at(0).lanes, SizeIs(2));
    EXPECT_THAT(result.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(0).successor.value(), Eq(0));
    EXPECT_THAT(result.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(0).lanes.at(1).successor.value(), Eq(-1));
    EXPECT_THAT(result.at(1).startS, Eq(100));
    EXPECT_THAT(result.at(1).endS, Eq(300));
    ASSERT_THAT(result.at(1).lanes, SizeIs(3));
    EXPECT_THAT(result.at(1).lanes.at(0).relativeId, Eq(1));
    EXPECT_THAT(result.at(1).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(0).type, Eq(LaneType::Stop));
    EXPECT_THAT(result.at(1).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(1).relativeId, Eq(0));
    EXPECT_THAT(result.at(1).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(1).type, Eq(LaneType::Entry));
    EXPECT_THAT(result.at(1).lanes.at(1).predecessor.value(), Eq(0));
    EXPECT_THAT(result.at(1).lanes.at(1).successor, Eq(std::nullopt));
    EXPECT_THAT(result.at(1).lanes.at(2).relativeId, Eq(-1));
    EXPECT_THAT(result.at(1).lanes.at(2).inDrivingDirection, Eq(true));
    EXPECT_THAT(result.at(1).lanes.at(2).type, Eq(LaneType::Shoulder));
    EXPECT_THAT(result.at(1).lanes.at(2).predecessor.value(), Eq(-1));
    EXPECT_THAT(result.at(1).lanes.at(2).successor, Eq(std::nullopt));
}

TEST(GetRelativeLane, BranchingTree_ReturnsLanesOfWayToNode)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node1, road1] = roadStream.AddRoot(100.0, true);
    auto [node2, road2] = roadStream.AddRoad(150.0, true, *node1);
    auto [node3, road3] = roadStream.AddRoad(200.0, false, *node1);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    ON_CALL(lane1_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane1_2, GetLaneType()).WillByDefault(Return(LaneType::Exit));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> predecessors2_1{idLane1_1};
    std::vector<OWL::Id> predecessors2_2{idLane1_2};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(ReturnRef(predecessors2_1));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(ReturnRef(predecessors2_2));
    ON_CALL(lane2_1, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    ON_CALL(lane2_2, GetLaneType()).WillByDefault(Return(LaneType::Entry));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(0.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(150.0));
    OWL::Fakes::Lane lane3_1;
    OWL::Fakes::Lane lane3_2;
    OWL::Id idLane3_1 = 5, idLane3_2 = 6;
    ON_CALL(lane3_1, GetId()).WillByDefault(Return(idLane3_1));
    ON_CALL(lane3_2, GetId()).WillByDefault(Return(idLane3_2));
    ON_CALL(lane3_1, GetOdId()).WillByDefault(Return(1));
    ON_CALL(lane3_2, GetOdId()).WillByDefault(Return(2));
    std::vector<OWL::Id> predecessors3_1{idLane1_1};
    std::vector<OWL::Id> predecessors3_2{idLane1_2};
    ON_CALL(lane3_1, GetNext()).WillByDefault(ReturnRef(predecessors3_1));
    ON_CALL(lane3_2, GetNext()).WillByDefault(ReturnRef(predecessors3_2));
    ON_CALL(lane3_1, GetLaneType()).WillByDefault(Return(LaneType::Driving));
    ON_CALL(lane3_2, GetLaneType()).WillByDefault(Return(LaneType::Stop));
    OWL::Fakes::Section section3;
    OWL::Interfaces::Lanes lanes3{{&lane3_1, &lane3_2}};
    ON_CALL(section3, GetLanes()).WillByDefault(ReturnRef(lanes3));
    ON_CALL(section3, GetSOffset()).WillByDefault(Return(0.0));
    ON_CALL(section3, GetLength()).WillByDefault(Return(200.0));
    std::vector<const OWL::Interfaces::Section*> sections1{&section1};
    ON_CALL(*road1, GetSections()).WillByDefault(ReturnRef(sections1));
    std::vector<const OWL::Interfaces::Section*> sections2{&section2};
    ON_CALL(*road2, GetSections()).WillByDefault(ReturnRef(sections2));
    std::vector<const OWL::Interfaces::Section*> sections3{&section3};
    ON_CALL(*road3, GetSections()).WillByDefault(ReturnRef(sections3));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLanes(roadStream.Get(), 0.0, -1, 300.0, true);
    auto result2 = result.at(node2->roadGraphVertex);
    auto result3 = result.at(node3->roadGraphVertex);

    ASSERT_THAT(result2, SizeIs(2));
    EXPECT_THAT(result2.at(0).startS, Eq(0));
    EXPECT_THAT(result2.at(0).endS, Eq(100));
    ASSERT_THAT(result2.at(0).lanes, SizeIs(2));
    EXPECT_THAT(result2.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result2.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result2.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result2.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result2.at(0).lanes.at(0).successor.value(), Eq(0));
    EXPECT_THAT(result2.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result2.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result2.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result2.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result2.at(0).lanes.at(1).successor.value(), Eq(-1));
    EXPECT_THAT(result2.at(1).startS, Eq(100));
    EXPECT_THAT(result2.at(1).endS, Eq(250));
    ASSERT_THAT(result2.at(1).lanes, SizeIs(2));
    EXPECT_THAT(result2.at(1).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result2.at(1).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result2.at(1).lanes.at(0).type, Eq(LaneType::Stop));
    EXPECT_THAT(result2.at(1).lanes.at(0).predecessor.value(), Eq(0));
    EXPECT_THAT(result2.at(1).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result2.at(1).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result2.at(1).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result2.at(1).lanes.at(1).type, Eq(LaneType::Entry));
    EXPECT_THAT(result2.at(1).lanes.at(1).predecessor.value(), Eq(-1));
    EXPECT_THAT(result2.at(1).lanes.at(1).successor, Eq(std::nullopt));

    ASSERT_THAT(result3, SizeIs(2));
    EXPECT_THAT(result3.at(0).startS, Eq(0));
    EXPECT_THAT(result3.at(0).endS, Eq(100));
    ASSERT_THAT(result3.at(0).lanes, SizeIs(2));
    EXPECT_THAT(result3.at(0).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result3.at(0).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result3.at(0).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result3.at(0).lanes.at(0).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result3.at(0).lanes.at(0).successor.value(), Eq(0));
    EXPECT_THAT(result3.at(0).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result3.at(0).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result3.at(0).lanes.at(1).type, Eq(LaneType::Exit));
    EXPECT_THAT(result3.at(0).lanes.at(1).predecessor, Eq(std::nullopt));
    EXPECT_THAT(result3.at(0).lanes.at(1).successor.value(), Eq(-1));
    EXPECT_THAT(result3.at(1).startS, Eq(100));
    EXPECT_THAT(result3.at(1).endS, Eq(300));
    ASSERT_THAT(result3.at(1).lanes, SizeIs(2));
    EXPECT_THAT(result3.at(1).lanes.at(0).relativeId, Eq(0));
    EXPECT_THAT(result3.at(1).lanes.at(0).inDrivingDirection, Eq(true));
    EXPECT_THAT(result3.at(1).lanes.at(0).type, Eq(LaneType::Driving));
    EXPECT_THAT(result3.at(1).lanes.at(0).predecessor.value(), Eq(0));
    EXPECT_THAT(result3.at(1).lanes.at(0).successor, Eq(std::nullopt));
    EXPECT_THAT(result3.at(1).lanes.at(1).relativeId, Eq(-1));
    EXPECT_THAT(result3.at(1).lanes.at(1).inDrivingDirection, Eq(true));
    EXPECT_THAT(result3.at(1).lanes.at(1).type, Eq(LaneType::Stop));
    EXPECT_THAT(result3.at(1).lanes.at(1).predecessor.value(), Eq(-1));
    EXPECT_THAT(result3.at(1).lanes.at(1).successor, Eq(std::nullopt));
}

TEST(GetRelativeLaneId, OnlySectionInDrivingDirection_ReturnsCorrectRelativeId)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, road] = roadStream.AddRoot(100.0, true);
    OWL::Fakes::Lane lane1;
    OWL::Fakes::Lane lane2;
    OWL::Fakes::Lane lane3;
    OWL::Id idLane1 = 1, idLane2 = 2, idLane3 = 3;
    ON_CALL(lane1, GetId()).WillByDefault(Return(idLane1));
    ON_CALL(lane2, GetId()).WillByDefault(Return(idLane2));
    ON_CALL(lane3, GetId()).WillByDefault(Return(idLane3));
    ON_CALL(lane1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane3, GetOdId()).WillByDefault(Return(1));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane3, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    OWL::Fakes::Section section;
    OWL::Interfaces::Lanes lanes{{&lane1, &lane2, &lane3}};
    ON_CALL(section, GetLanes()).WillByDefault(ReturnRef(lanes));
    ON_CALL(section, GetLength()).WillByDefault(Return(100.0));
    std::vector<const OWL::Interfaces::Section*> sections{&section};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));
    std::string idRoad = "RoadA";
    ON_CALL(*road, GetId()).WillByDefault(ReturnRef(idRoad));

    GlobalRoadPositions targetPosition1{{idRoad, {idRoad, 1, 15., 0, 0}}};
    GlobalRoadPositions targetPosition2{{idRoad, {idRoad, -2, 15., 0, 0}}};

    WorldDataQuery wdQuery{worldData};
    auto result1 = wdQuery.GetRelativeLaneId(roadStream.Get(), 10.0, -1, targetPosition1).at(node->roadGraphVertex);
    auto result2 = wdQuery.GetRelativeLaneId(roadStream.Get(), 10.0, -1, targetPosition2).at(node->roadGraphVertex);

    EXPECT_THAT(result1, Eq(1));
    EXPECT_THAT(result2, Eq(-1));
}

TEST(GetRelativeLaneId, IdOfEgoLaneChanges_ReturnsCorrectRelativeId)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, road] = roadStream.AddRoot(100.0, true);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    ON_CALL(lane1_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Fakes::Lane lane2_3;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4, idLane2_3 = 5;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_3, GetId()).WillByDefault(Return(idLane2_3));
    ON_CALL(lane2_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2_2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2_3, GetOdId()).WillByDefault(Return(-3));
    std::vector<OWL::Id> predecessors2_2{idLane1_1};
    std::vector<OWL::Id> predecessors2_3{idLane1_2};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(ReturnRef(predecessors2_2));
    ON_CALL(lane2_3, GetPrevious()).WillByDefault(ReturnRef(predecessors2_3));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2, &lane2_3}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(100.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(200.0));
    std::vector<const OWL::Interfaces::Section*> sections{&section1, &section2};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));
    std::string idRoad = "RoadA";
    ON_CALL(*road, GetId()).WillByDefault(ReturnRef(idRoad));

    GlobalRoadPositions targetPosition1{{idRoad, {idRoad, -1, 115., 0, 0}}};
    GlobalRoadPositions targetPosition2{{idRoad, {idRoad, -2, 115., 0, 0}}};

    WorldDataQuery wdQuery{worldData};
    auto result1 = wdQuery.GetRelativeLaneId(roadStream.Get(), 10.0, -1, targetPosition1).at(node->roadGraphVertex);
    auto result2 = wdQuery.GetRelativeLaneId(roadStream.Get(), 10.0, -1, targetPosition2).at(node->roadGraphVertex);

    EXPECT_THAT(result1, Eq(1));
    EXPECT_THAT(result2, Eq(0));
}

TEST(GetRelativeLaneId, BranchingTree_ReturnsCorrectRelativeId)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node1, road1] = roadStream.AddRoot(100.0, true);
    auto [node2, road2] = roadStream.AddRoad(150.0, true, *node1);
    auto [node3, road3] = roadStream.AddRoad(200.0, false, *node1);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    ON_CALL(lane1_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> predecessors2_1{idLane1_1};
    std::vector<OWL::Id> predecessors2_2{idLane1_2};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(ReturnRef(predecessors2_1));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(ReturnRef(predecessors2_2));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(0.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(150.0));
    OWL::Fakes::Lane lane3_1;
    OWL::Fakes::Lane lane3_2;
    OWL::Id idLane3_1 = 5, idLane3_2 = 6;
    ON_CALL(lane3_1, GetId()).WillByDefault(Return(idLane3_1));
    ON_CALL(lane3_2, GetId()).WillByDefault(Return(idLane3_2));
    ON_CALL(lane3_1, GetOdId()).WillByDefault(Return(1));
    ON_CALL(lane3_2, GetOdId()).WillByDefault(Return(2));
    std::vector<OWL::Id> predecessors3_1{idLane1_1};
    std::vector<OWL::Id> predecessors3_2{idLane1_2};
    ON_CALL(lane3_1, GetNext()).WillByDefault(ReturnRef(predecessors3_1));
    ON_CALL(lane3_2, GetNext()).WillByDefault(ReturnRef(predecessors3_2));
    OWL::Fakes::Section section3;
    OWL::Interfaces::Lanes lanes3{{&lane3_1, &lane3_2}};
    ON_CALL(section3, GetLanes()).WillByDefault(ReturnRef(lanes3));
    ON_CALL(section3, GetSOffset()).WillByDefault(Return(0.0));
    ON_CALL(section3, GetLength()).WillByDefault(Return(200.0));
    std::vector<const OWL::Interfaces::Section*> sections1{&section1};
    ON_CALL(*road1, GetSections()).WillByDefault(ReturnRef(sections1));
    std::vector<const OWL::Interfaces::Section*> sections2{&section2};
    ON_CALL(*road2, GetSections()).WillByDefault(ReturnRef(sections2));
    std::vector<const OWL::Interfaces::Section*> sections3{&section3};
    ON_CALL(*road3, GetSections()).WillByDefault(ReturnRef(sections3));

    std::string idRoad1 = "RoadA";
    std::string idRoad2 = "RoadB";
    std::string idRoad3 = "RoadC";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    ON_CALL(*road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    GlobalRoadPositions targetPosition1{{idRoad2, {idRoad2, -1, 115., 0, 0}}, {idRoad3, {idRoad3, 1, 115., 0, 0}}};
    GlobalRoadPositions targetPosition2{{idRoad2, {idRoad2, -2, 115., 0, 0}}, {idRoad3, {idRoad3, 2, 115., 0, 0}}};

    WorldDataQuery wdQuery{worldData};
    auto result1 = wdQuery.GetRelativeLaneId(roadStream.Get(), 10.0, -1, targetPosition1).at(node3->roadGraphVertex);
    auto result2 = wdQuery.GetRelativeLaneId(roadStream.Get(), 10.0, -1, targetPosition2).at(node3->roadGraphVertex);

    EXPECT_THAT(result1, Eq(0));
    EXPECT_THAT(result2, Eq(-1));
}

TEST(GetRelativeLaneId, TargetOnPreviousSection_ReturnsCorrectRelativeId)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node, road] = roadStream.AddRoot(100.0, true);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    ON_CALL(lane1_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Fakes::Lane lane2_3;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4, idLane2_3 = 5;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_3, GetId()).WillByDefault(Return(idLane2_3));
    ON_CALL(lane2_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2_2, GetOdId()).WillByDefault(Return(-2));
    ON_CALL(lane2_3, GetOdId()).WillByDefault(Return(-3));
    std::vector<OWL::Id> predecessors2_2{idLane1_1};
    std::vector<OWL::Id> predecessors2_3{idLane1_2};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(ReturnRef(predecessors2_2));
    ON_CALL(lane2_3, GetPrevious()).WillByDefault(ReturnRef(predecessors2_3));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2, &lane2_3}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(100.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(200.0));
    std::vector<const OWL::Interfaces::Section*> sections{&section1, &section2};
    ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sections));
    std::string idRoad = "RoadA";
    ON_CALL(*road, GetId()).WillByDefault(ReturnRef(idRoad));

    GlobalRoadPositions targetPosition1{{idRoad, {idRoad, -1, 15., 0, 0}}};
    GlobalRoadPositions targetPosition2{{idRoad, {idRoad, -2, 15., 0, 0}}};

    WorldDataQuery wdQuery{worldData};
    auto result1 = wdQuery.GetRelativeLaneId(roadStream.Get(), 110.0, -2, targetPosition1).at(node->roadGraphVertex);
    auto result2 = wdQuery.GetRelativeLaneId(roadStream.Get(), 110.0, -2, targetPosition2).at(node->roadGraphVertex);

    EXPECT_THAT(result1, Eq(0));
    EXPECT_THAT(result2, Eq(-1));
}


TEST(GetRelativeLaneId, TargetOnPreviousRoad_ReturnsCorrectRelativeId)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;

    auto [node1, road1] = roadStream.AddRoot(100.0, true);
    auto [node2, road2] = roadStream.AddRoad(150.0, true, *node1);
    OWL::Fakes::Lane lane1_1;
    OWL::Fakes::Lane lane1_2;
    OWL::Id idLane1_1 = 1, idLane1_2 = 2;
    ON_CALL(lane1_1, GetId()).WillByDefault(Return(idLane1_1));
    ON_CALL(lane1_2, GetId()).WillByDefault(Return(idLane1_2));
    ON_CALL(lane1_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane1_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> emptyIds{};
    ON_CALL(lane1_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane1_2, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    OWL::Fakes::Section section1;
    OWL::Interfaces::Lanes lanes1{{&lane1_1, &lane1_2}};
    ON_CALL(section1, GetLanes()).WillByDefault(ReturnRef(lanes1));
    ON_CALL(section1, GetLength()).WillByDefault(Return(100.0));
    OWL::Fakes::Lane lane2_1;
    OWL::Fakes::Lane lane2_2;
    OWL::Id idLane2_1 = 3, idLane2_2 = 4;
    ON_CALL(lane2_1, GetId()).WillByDefault(Return(idLane2_1));
    ON_CALL(lane2_2, GetId()).WillByDefault(Return(idLane2_2));
    ON_CALL(lane2_1, GetOdId()).WillByDefault(Return(-1));
    ON_CALL(lane2_2, GetOdId()).WillByDefault(Return(-2));
    std::vector<OWL::Id> predecessors2_2{idLane1_1};
    ON_CALL(lane2_1, GetPrevious()).WillByDefault(ReturnRef(emptyIds));
    ON_CALL(lane2_2, GetPrevious()).WillByDefault(ReturnRef(predecessors2_2));
    OWL::Fakes::Section section2;
    OWL::Interfaces::Lanes lanes2{{&lane2_1, &lane2_2}};
    ON_CALL(section2, GetLanes()).WillByDefault(ReturnRef(lanes2));
    ON_CALL(section2, GetSOffset()).WillByDefault(Return(0.0));
    ON_CALL(section2, GetLength()).WillByDefault(Return(150.0));
    std::vector<const OWL::Interfaces::Section*> sections1{&section1};
    ON_CALL(*road1, GetSections()).WillByDefault(ReturnRef(sections1));
    std::vector<const OWL::Interfaces::Section*> sections2{&section2};
    ON_CALL(*road2, GetSections()).WillByDefault(ReturnRef(sections2));

    std::string idRoad1 = "RoadA";
    std::string idRoad2 = "RoadB";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));

    GlobalRoadPositions targetPosition{{idRoad1, {idRoad1, -1, 15., 0, 0}}};

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetRelativeLaneId(roadStream.Get(), 110.0, -2, targetPosition).at(node2->roadGraphVertex);

    EXPECT_THAT(result, Eq(0));
}

TEST(GetLaneCurvature, OnLaneStreamReturnsCorrectCurvature)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneMultiStream laneStream;
    auto [node1, lane1] = laneStream.AddRoot(100.0, true);
    auto [node2, lane2] = laneStream.AddLane(150.0, true, *node1);
    auto [node3, lane3] = laneStream.AddLane(200.0, false, *node1);

    ON_CALL(*lane2, GetCurvature(DoubleEq(50.0))).WillByDefault(Return(0.1));
    ON_CALL(*lane3, GetCurvature(DoubleEq(150.0))).WillByDefault(Return(0.2));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetLaneCurvature(laneStream.Get(), 150.0);

    EXPECT_THAT(result.at(node2->roadGraphVertex), Eq(0.1));
    EXPECT_THAT(result.at(node3->roadGraphVertex), Eq(0.2));
}

TEST(GetLaneWidth, OnLaneStreamReturnsCorrectCurvature)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneMultiStream laneStream;
    auto [node1, lane1] = laneStream.AddRoot(100.0, true);
    auto [node2, lane2] = laneStream.AddLane(150.0, true, *node1);
    auto [node3, lane3] = laneStream.AddLane(200.0, false, *node1);

    ON_CALL(*lane2, GetWidth(DoubleEq(50.0))).WillByDefault(Return(1.1));
    ON_CALL(*lane3, GetWidth(DoubleEq(150.0))).WillByDefault(Return(2.2));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetLaneWidth(laneStream.Get(), 150.0);

    EXPECT_THAT(result.at(node2->roadGraphVertex), Eq(1.1));
    EXPECT_THAT(result.at(node3->roadGraphVertex), Eq(2.2));
}

TEST(GetLaneDirection, OnLaneStreamReturnsCorrectCurvature)
{
    OWL::Fakes::WorldData worldData;
    FakeLaneMultiStream laneStream;
    auto [node1, lane1] = laneStream.AddRoot(100.0, true);
    auto [node2, lane2] = laneStream.AddLane(150.0, true, *node1);
    auto [node3, lane3] = laneStream.AddLane(200.0, false, *node1);

    ON_CALL(*lane2, GetDirection(DoubleEq(50.0))).WillByDefault(Return(0.1));
    ON_CALL(*lane3, GetDirection(DoubleEq(150.0))).WillByDefault(Return(0.2));

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetLaneDirection(laneStream.Get(), 150.0);

    EXPECT_THAT(result.at(node2->roadGraphVertex), Eq(0.1));
    EXPECT_THAT(result.at(node3->roadGraphVertex), Eq(0.2));
}

TEST(GetEdgeWeights, ReturnsMapWithCorrectRates)
{
    OWL::Fakes::WorldData worldData;

    TurningRates turningRates {{"RoadA", "RoadB", 2.},
                               {"RoadA", "RoadC", 3.},
                               {"RoadC", "RoadD", 4.}};
    ON_CALL(worldData, GetTurningRates()).WillByDefault(ReturnRef(turningRates));

    RoadGraph roadGraph;
    auto nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraph);
    auto nodeB = add_vertex(RouteElement{"RoadB", true}, roadGraph);
    auto nodeC = add_vertex(RouteElement{"RoadC", true}, roadGraph);
    auto nodeD = add_vertex(RouteElement{"RoadD", true}, roadGraph);
    auto edgeAB = add_edge(nodeA, nodeB, roadGraph).first;
    auto edgeAC = add_edge(nodeA, nodeC, roadGraph).first;
    auto edgeAD = add_edge(nodeA, nodeD, roadGraph).first;

    WorldDataQuery wdQuery{worldData};
    auto result = wdQuery.GetEdgeWeights(roadGraph);

    EXPECT_THAT(result, SizeIs(3));
    EXPECT_THAT(result.at(edgeAB), Eq(2.));
    EXPECT_THAT(result.at(edgeAC), Eq(3.));
    EXPECT_THAT(result.at(edgeAD), Eq(1.));
}

TEST(ResolveRelativePoint, ObjectOnFirstRoad)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;
    auto [node1, road1] = roadStream.AddRoot(100, true);
    auto [node2, road2] = roadStream.AddRoad(200, false, *node1);
    auto [node3, road3] = roadStream.AddRoad(300, true, *node1);
    std::string idRoad1 = "Road1";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    std::string idRoad2 = "Road2";
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    std::string idRoad3 = "Road3";
    ON_CALL(*road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    RoadInterval roadInterval{{},
                              GlobalRoadPosition{"Road1",0,10,0,0},
                              GlobalRoadPosition{"Road1",0,15,0,0},
                              GlobalRoadPosition{"Road1",0,12,-1,0},
                              GlobalRoadPosition{"Road1",0,12,2,0}};
    RoadIntervals touchedRoads{{"Road1", roadInterval}};

    WorldDataQuery wdQuery{worldData};
    auto rearmost = wdQuery.ResolveRelativePoint(roadStream.Get(), ObjectPointRelative::Rearmost, touchedRoads);
    auto frontmost = wdQuery.ResolveRelativePoint(roadStream.Get(), ObjectPointRelative::Frontmost, touchedRoads);
    auto rightmost = wdQuery.ResolveRelativePoint(roadStream.Get(), ObjectPointRelative::Rightmost, touchedRoads);
    auto leftmost = wdQuery.ResolveRelativePoint(roadStream.Get(), ObjectPointRelative::Leftmost, touchedRoads);

    ASSERT_THAT(rearmost.at(node1->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,10,0,0}));
    ASSERT_THAT(rearmost.at(node2->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,10,0,0}));
    ASSERT_THAT(rearmost.at(node3->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,10,0,0}));

    ASSERT_THAT(frontmost.at(node1->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,15,0,0}));
    ASSERT_THAT(frontmost.at(node2->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,15,0,0}));
    ASSERT_THAT(frontmost.at(node3->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,15,0,0}));

    ASSERT_THAT(rightmost.at(node1->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,12,-1,0}));
    ASSERT_THAT(rightmost.at(node2->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,12,-1,0}));
    ASSERT_THAT(rightmost.at(node3->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,12,-1,0}));

    ASSERT_THAT(leftmost.at(node1->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,12,2,0}));
    ASSERT_THAT(leftmost.at(node2->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,12,2,0}));
    ASSERT_THAT(leftmost.at(node3->roadGraphVertex), Eq(GlobalRoadPosition{"Road1",0,12,2,0}));
}

TEST(ResolveRelativePoint, ObjectOnSecondRoads)
{
    OWL::Fakes::WorldData worldData;
    FakeRoadMultiStream roadStream;
    auto [node1, road1] = roadStream.AddRoot(100, true);
    auto [node2, road2] = roadStream.AddRoad(200, false, *node1);
    auto [node3, road3] = roadStream.AddRoad(300, true, *node1);
    std::string idRoad1 = "Road1";
    ON_CALL(*road1, GetId()).WillByDefault(ReturnRef(idRoad1));
    std::string idRoad2 = "Road2";
    ON_CALL(*road2, GetId()).WillByDefault(ReturnRef(idRoad2));
    std::string idRoad3 = "Road3";
    ON_CALL(*road3, GetId()).WillByDefault(ReturnRef(idRoad3));

    RoadInterval roadInterval2{{},
                              GlobalRoadPosition{"Road2",0,110,0,0},
                              GlobalRoadPosition{"Road2",0,115,0,0},
                              GlobalRoadPosition{"Road2",0,112,-1,0},
                              GlobalRoadPosition{"Road2",0,112,2,0}};
    RoadInterval roadInterval3{{},
                              GlobalRoadPosition{"Road3",0,210,0,0},
                              GlobalRoadPosition{"Road3",0,215,0,0},
                              GlobalRoadPosition{"Road3",0,212,-1,0},
                              GlobalRoadPosition{"Road3",0,212,2,0}};
    RoadIntervals touchedRoads{{"Road2", roadInterval2}, {"Road3", roadInterval3}};

    WorldDataQuery wdQuery{worldData};
    auto rearmost = wdQuery.ResolveRelativePoint(roadStream.Get(), ObjectPointRelative::Rearmost, touchedRoads);
    auto frontmost = wdQuery.ResolveRelativePoint(roadStream.Get(), ObjectPointRelative::Frontmost, touchedRoads);
    auto rightmost = wdQuery.ResolveRelativePoint(roadStream.Get(), ObjectPointRelative::Rightmost, touchedRoads);
    auto leftmost = wdQuery.ResolveRelativePoint(roadStream.Get(), ObjectPointRelative::Leftmost, touchedRoads);

    ASSERT_THAT(rearmost.at(node1->roadGraphVertex), std::nullopt);
    ASSERT_THAT(rearmost.at(node2->roadGraphVertex), Eq(GlobalRoadPosition{"Road2",0,115,0,0}));
    ASSERT_THAT(rearmost.at(node3->roadGraphVertex), Eq(GlobalRoadPosition{"Road3",0,210,0,0}));

    ASSERT_THAT(frontmost.at(node1->roadGraphVertex), std::nullopt);
    ASSERT_THAT(frontmost.at(node2->roadGraphVertex), Eq(GlobalRoadPosition{"Road2",0,110,0,0}));
    ASSERT_THAT(frontmost.at(node3->roadGraphVertex), Eq(GlobalRoadPosition{"Road3",0,215,0,0}));

    ASSERT_THAT(rightmost.at(node1->roadGraphVertex), std::nullopt);
    ASSERT_THAT(rightmost.at(node2->roadGraphVertex), Eq(GlobalRoadPosition{"Road2",0,112,2,0}));
    ASSERT_THAT(rightmost.at(node3->roadGraphVertex), Eq(GlobalRoadPosition{"Road3",0,212,-1,0}));

    ASSERT_THAT(leftmost.at(node1->roadGraphVertex), std::nullopt);
    ASSERT_THAT(leftmost.at(node2->roadGraphVertex), Eq(GlobalRoadPosition{"Road2",0,112,-1,0}));
    ASSERT_THAT(leftmost.at(node3->roadGraphVertex), Eq(GlobalRoadPosition{"Road3",0,212,2,0}));
}
