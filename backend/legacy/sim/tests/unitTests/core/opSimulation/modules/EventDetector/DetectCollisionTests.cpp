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

#include "fakeAgent.h"
#include "fakeTrafficObject.h"
#include "fakeWorld.h"
#include "fakeEventNetwork.h"

#include "CollisionDetector.h"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class CollisionDetectorTest : public ::testing::Test
{
public:
    CollisionDetectorTest()
    {
        objects = {&objectAsCollisionParter};
        agents = {{0, &agentUnderTest}, {9, &agentAsCollisionPartner}};

        ON_CALL(mockWorld, GetTrafficObjects()).WillByDefault(ReturnRef(objects));
        ON_CALL(mockWorld, GetAgents()).WillByDefault(Return(agents));
    }
protected:
    NiceMock<FakeTrafficObject> objectAsCollisionParter;
    NiceMock<FakeAgent> agentUnderTest;
    NiceMock<FakeAgent> agentAsCollisionPartner;
    NiceMock<FakeWorld> mockWorld;
    NiceMock<FakeEventNetwork> mockEventNetwork;

    std::vector<const TrafficObjectInterface *> objects;
    std::map<int, AgentInterface *> agents;

    void UseAgent()
    {
        objects = {};
        agents = {{0, &agentUnderTest}, {1, &agentAsCollisionPartner}};

        ON_CALL(mockWorld, GetTrafficObjects()).WillByDefault(ReturnRef(objects));
        ON_CALL(mockWorld, GetAgents()).WillByDefault(Return(agents));
    }

    void UseObject()
    {
        objects = {&objectAsCollisionParter};
        agents = {{0, &agentUnderTest}};

        ON_CALL(mockWorld, GetTrafficObjects()).WillByDefault(ReturnRef(objects));
        ON_CALL(mockWorld, GetAgents()).WillByDefault(Return(agents));
    }
};

TEST_F(CollisionDetectorTest, TestCollisionStandardCase)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,4});
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionStandardCase2)
{
    UseObject();

    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{0,1});
    ownBoundingBox.outer().push_back(point_t{1,2});
    ownBoundingBox.outer().push_back(point_t{2,1});
    ownBoundingBox.outer().push_back(point_t{1,0});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{1,1});
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{3,1});
    otherBoundingBox.outer().push_back(point_t{2,0});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}
TEST_F(CollisionDetectorTest, TestCollisionStandardCase3)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{1,1});
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{3,1});
    otherBoundingBox.outer().push_back(point_t{2,0});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionStandardCase4)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{0,1});
    otherBoundingBox.outer().push_back(point_t{1,2});
    otherBoundingBox.outer().push_back(point_t{2,1});
    otherBoundingBox.outer().push_back(point_t{1,0});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionStandardCase_AgentWithAgent)
{
    UseAgent();

    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,4});
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(agentAsCollisionPartner, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}


TEST_F(CollisionDetectorTest, TestCollisionSpecialCaseCrossedHitboxes)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{-1,3});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{1,-3});
    ownBoundingBox.outer().push_back(point_t{-1,-3});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{-3,1});
    otherBoundingBox.outer().push_back(point_t{3,1});
    otherBoundingBox.outer().push_back(point_t{3,-1});
    otherBoundingBox.outer().push_back(point_t{-3,-1});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}
//TODO handle this case in the algorithm so that this case returns a collision
/*
TEST_F(CollisionDetectorTest, TestCollisionSpecialCaseBothSameHitbox)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{0,0});
    ownBoundingBox.outer().push_back(point_t{0,5});
    ownBoundingBox.outer().push_back(point_t{5,5});
    ownBoundingBox.outer().push_back(point_t{5,0});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{0,0});
    otherBoundingBox.outer().push_back(point_t{0,5});
    otherBoundingBox.outer().push_back(point_t{5,5});
    otherBoundingBox.outer().push_back(point_t{5,0});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}
*/

//TODO handle this case in the algorithm so that this case returns a collision
/*
TEST_F(CollisionDetectorTest, TestCollisionSpecialCaseOneWithinTheOther)
{
    ON_CALL(objectUnderTest, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectUnderTest, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{0,0});
    ownBoundingBox.outer().push_back(point_t{0,5});
    ownBoundingBox.outer().push_back(point_t{5,5});
    ownBoundingBox.outer().push_back(point_t{5,0});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{1,1});
    otherBoundingBox.outer().push_back(point_t{1,3});
    otherBoundingBox.outer().push_back(point_t{3,3});
    otherBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(objectUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}
*/

TEST_F(CollisionDetectorTest, TestCollidableFalseWhenObjectAreColliding)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(false));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(0.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,4});
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestNotCollideDueToZValue)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(10.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,4});
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestNotCollideBecauseNotOverlapping2DHitbox)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(0.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,6});
    otherBoundingBox.outer().push_back(point_t{6,6});
    otherBoundingBox.outer().push_back(point_t{6,4});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestNotCollideBecauseNotOverlapping2DHitbox_AgentWithAgent)
{
    UseAgent();

    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,6});
    otherBoundingBox.outer().push_back(point_t{6,6});
    otherBoundingBox.outer().push_back(point_t{6,4});
    ON_CALL(agentAsCollisionPartner, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionStandardCaseAgentcollidesNotWithOtherAgent)
{
    UseAgent();

    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,5});
    otherBoundingBox.outer().push_back(point_t{5,5});
    otherBoundingBox.outer().push_back(point_t{5,4});
    ON_CALL(agentAsCollisionPartner, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionBorderCaseCollisionInASinglePoint)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(1.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(1.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{3,3});
    otherBoundingBox.outer().push_back(point_t{3,5});
    otherBoundingBox.outer().push_back(point_t{5,5});
    otherBoundingBox.outer().push_back(point_t{5,3});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionBorderCaseCollisionOnSingleEdge)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(0.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{3,3});
    otherBoundingBox.outer().push_back(point_t{3,5});
    otherBoundingBox.outer().push_back(point_t{5,5});
    otherBoundingBox.outer().push_back(point_t{5,3});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionBorderCaseCollisionAreaWithNoCollisionVolume)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(0.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{3,1});
    otherBoundingBox.outer().push_back(point_t{3,3});
    otherBoundingBox.outer().push_back(point_t{5,3});
    otherBoundingBox.outer().push_back(point_t{5,1});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionPartners)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(0.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    std::vector<std::pair<ObjectTypeOSI, int>> collisionPartners;
    collisionPartners.push_back(std::make_pair(objectAsCollisionParter.GetType(), objectAsCollisionParter.GetId()));
    ON_CALL(agentUnderTest, GetCollisionPartners()).WillByDefault(Return(collisionPartners));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,4});
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestAgentWithZeroNoHeight)
{
    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(0.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(0.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,4});
    otherBoundingBox.outer().push_back(point_t{4,4});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}

TEST_F(CollisionDetectorTest, TestCollisionCaseBrokenHitboxes)
{
    //Test for that this hitbox will not cause a runtime exception

    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    //PolygonDefinitionWrongOrder
    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,4});
    otherBoundingBox.outer().push_back(point_t{4,2});
    otherBoundingBox.outer().push_back(point_t{4,4});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    EXPECT_NO_THROW(collisionDetector.Trigger(0));
}


TEST_F(CollisionDetectorTest, TestCollisionCaseBrokenHitboxes2)
{
    //Test for that this hitbox will not cause a runtime exception

    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,1});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    ownBoundingBox.outer().push_back(point_t{0,0});
    ownBoundingBox.outer().push_back(point_t{0,5});
    ownBoundingBox.outer().push_back(point_t{5,5});
    ownBoundingBox.outer().push_back(point_t{5,0});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    EXPECT_NO_THROW(collisionDetector.Trigger(0));
}

TEST_F(CollisionDetectorTest, TestCollisionCaseBrokenHitboxes3)
{
    //Test for that this hitbox will not cause a runtime exception

    UseObject();
    ON_CALL(objectAsCollisionParter, GetIsCollidable()).WillByDefault(Return(true));

    ON_CALL(objectAsCollisionParter, GetZOffset()).WillByDefault(Return(4.0));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.0));

    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{0,3});
    ownBoundingBox.outer().push_back(point_t{0,-3});
    ownBoundingBox.outer().push_back(point_t{0,3});
    ownBoundingBox.outer().push_back(point_t{0,-3});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));

    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{3,0});
    otherBoundingBox.outer().push_back(point_t{-3,0});
    otherBoundingBox.outer().push_back(point_t{3,0});
    otherBoundingBox.outer().push_back(point_t{-3,0});
    ON_CALL(objectAsCollisionParter, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    EXPECT_NO_THROW(collisionDetector.Trigger(0));
}
