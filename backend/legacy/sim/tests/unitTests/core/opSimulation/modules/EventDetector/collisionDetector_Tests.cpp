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

#include "dontCare.h"
#include "fakeAgent.h"
#include "fakeTrafficObject.h"
#include "fakeCallback.h"
#include "fakeEventNetwork.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"

#include "CollisionDetector.h"

using namespace testing;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::NiceMock;
using ::testing::_;

TEST(CollisionDetector, IsCollideableFalse_AgentDoesNotCollide)
{
    NiceMock<FakeTrafficObject> objectUnderTest;
    NiceMock<FakeAgent> agentUnderTest;
    NiceMock<FakeWorld> mockWorld;
    NiceMock<FakeEventNetwork> mockEventNetwork;

    std::vector<const TrafficObjectInterface *> objects = {&objectUnderTest};
    std::map<int, AgentInterface *> agents = {{0, &agentUnderTest}};

    ON_CALL(mockWorld, GetTrafficObjects()).WillByDefault(ReturnRef(objects));
    ON_CALL(mockWorld, GetAgents()).WillByDefault(Return(agents));
    ON_CALL(objectUnderTest, GetIsCollidable()).WillByDefault(Return(false));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.));
    ON_CALL(objectUnderTest, GetZOffset()).WillByDefault(Return(0.));
    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,6});
    otherBoundingBox.outer().push_back(point_t{4,6});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));
    ON_CALL(objectUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);
    
    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

TEST(CollisionDetector, AgentHigherThanGapUnderTrafficObject_AgentDoesCollides)
{
    NiceMock<FakeTrafficObject> objectUnderTest;
    NiceMock<FakeAgent> agentUnderTest;
    NiceMock<FakeWorld> mockWorld;
    NiceMock<FakeEventNetwork> mockEventNetwork;

    std::vector<const TrafficObjectInterface *> objects = {{&objectUnderTest}};
    std::map<int, AgentInterface *> agents = {{0, &agentUnderTest}};

    ON_CALL(mockWorld, GetTrafficObjects()).WillByDefault(ReturnRef(objects));
    ON_CALL(mockWorld, GetAgents()).WillByDefault(Return(agents));
    ON_CALL(objectUnderTest, GetIsCollidable()).WillByDefault(Return(true));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.));
    ON_CALL(objectUnderTest, GetZOffset()).WillByDefault(Return(4.));
    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,6});
    otherBoundingBox.outer().push_back(point_t{4,6});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));
    ON_CALL(objectUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);
    
    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(1);
    collisionDetector.Trigger(0);
}


TEST(CollisionDetector, AgentLowerThanGapUnderTrafficObject_AgentDoesNotCollide)
{
    NiceMock<FakeTrafficObject> objectUnderTest;
    NiceMock<FakeAgent> agentUnderTest;
    NiceMock<FakeWorld> mockWorld;
    NiceMock<FakeEventNetwork> mockEventNetwork;

    std::vector<const TrafficObjectInterface *> objects = {&objectUnderTest};
    std::map<int, AgentInterface *> agents = {{0, &agentUnderTest}};

    ON_CALL(mockWorld, GetTrafficObjects()).WillByDefault(ReturnRef(objects));
    ON_CALL(mockWorld, GetAgents()).WillByDefault(Return(agents));
    ON_CALL(objectUnderTest, GetIsCollidable()).WillByDefault(Return(true));
    ON_CALL(agentUnderTest, GetHeight()).WillByDefault(Return(5.));
    ON_CALL(objectUnderTest, GetZOffset()).WillByDefault(Return(10.));
    polygon_t ownBoundingBox;
    ownBoundingBox.outer().push_back(point_t{1,1});
    ownBoundingBox.outer().push_back(point_t{1,3});
    ownBoundingBox.outer().push_back(point_t{3,3});
    ownBoundingBox.outer().push_back(point_t{3,1});
    polygon_t otherBoundingBox;
    otherBoundingBox.outer().push_back(point_t{2,2});
    otherBoundingBox.outer().push_back(point_t{2,6});
    otherBoundingBox.outer().push_back(point_t{4,6});
    otherBoundingBox.outer().push_back(point_t{4,2});
    ON_CALL(agentUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(ownBoundingBox));
    ON_CALL(objectUnderTest, GetBoundingBox2D()).WillByDefault(ReturnRef(otherBoundingBox));

    CollisionDetector collisionDetector(&mockWorld, &mockEventNetwork, nullptr, nullptr);
    
    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(0);
    collisionDetector.Trigger(0);
}

