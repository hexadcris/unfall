/********************************************************************************
 * Copyright (c) 2019-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
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
#include "fakeWorld.h"

#include "collisionImpl.h"

using ::testing::Return;
using ::testing::ReturnPointee;
using ::testing::NiceMock;
using ::testing::DoubleNear;
using ::testing::_;

class DynamicsCollision_Test : public ::testing::Test
{
public:
    DynamicsCollision_Test()
    {
        heavyVehicle.properties = {{"Mass", 2000.0}};
        lightVehicle.properties = {{"Mass", 1000.0}};
    }

protected:
    VehicleModelParameters heavyVehicle;
    VehicleModelParameters lightVehicle;
};

TEST_F(DynamicsCollision_Test, CollisionOfTwoAgentsOnlyInXDirection)
{
    NiceMock<FakeAgent> agent;
    NiceMock<FakeAgent> opponent;
    std::pair<ObjectTypeOSI,int> pair0 = std::make_pair(ObjectTypeOSI::Vehicle, 0);
    std::pair<ObjectTypeOSI,int> pair1 = std::make_pair(ObjectTypeOSI::Vehicle, 1);

    ON_CALL(agent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{40.0, 0.0}));
    ON_CALL(agent, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(agent, GetVehicleModelParameters()).WillByDefault(Return(heavyVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersAgent {pair1};
    ON_CALL(agent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersAgent));

    ON_CALL(opponent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{10.0, 0.0}));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(opponent, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersOpponent {pair0};
    ON_CALL(opponent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersOpponent));

    NiceMock<FakeWorld> world;
    ON_CALL(world, GetAgent(0)).WillByDefault(Return(&agent));
    ON_CALL(world, GetAgent(1)).WillByDefault(Return(&opponent));

    DynamicsCollisionImplementation dynamicsCollision(
                "",
                false,
                0,
                0,
                0,
                0,
                nullptr,
                &world,
                nullptr,
                nullptr,
                nullptr,
                &agent);

    dynamicsCollision.Trigger(0);

    ASSERT_THAT(dynamicsCollision.GetVelocity(), DoubleNear(30.0, 1e-3));
    ASSERT_THAT(dynamicsCollision.GetMovingDirection(), DoubleNear(0.0, 1e-3));
}

TEST_F(DynamicsCollision_Test, CollisionOfTwoAgentsOrthogonal)
{
    NiceMock<FakeAgent> agent;
    NiceMock<FakeAgent> opponent;
    std::pair<ObjectTypeOSI,int> pair0 = std::make_pair(ObjectTypeOSI::Vehicle, 0);
    std::pair<ObjectTypeOSI,int> pair1 = std::make_pair(ObjectTypeOSI::Vehicle, 1);

    ON_CALL(agent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{40.0, 0.0}));
    ON_CALL(agent, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(agent, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersAgent {pair1};
    ON_CALL(agent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersAgent));

    ON_CALL(opponent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{0.0, 40.0}));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.5 * M_PI));
    ON_CALL(opponent, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersOpponent {pair0};
    ON_CALL(opponent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersOpponent));

    NiceMock<FakeWorld> world;
    ON_CALL(world, GetAgent(0)).WillByDefault(Return(&agent));
    ON_CALL(world, GetAgent(1)).WillByDefault(Return(&opponent));

    DynamicsCollisionImplementation dynamicsCollision(
                "",
                false,
                0,
                0,
                0,
                0,
                nullptr,
                &world,
                nullptr,
                nullptr,
                nullptr,
                &agent);

    dynamicsCollision.Trigger(0);

    double expectedVelocity = 20.0 * std::sqrt(2);
    ASSERT_THAT(dynamicsCollision.GetVelocity(), DoubleNear(expectedVelocity, 1e-3));
    ASSERT_THAT(dynamicsCollision.GetMovingDirection(), DoubleNear(0.25 * M_PI, 1e-3));
}

TEST_F(DynamicsCollision_Test, CollisionOfTwoAgentsOpposingDirections)
{
    NiceMock<FakeAgent> agent;
    NiceMock<FakeAgent> opponent;
    std::pair<ObjectTypeOSI,int> pair0 = std::make_pair(ObjectTypeOSI::Vehicle, 0);
    std::pair<ObjectTypeOSI,int> pair1 = std::make_pair(ObjectTypeOSI::Vehicle, 1);

    ON_CALL(agent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{-10.0 * M_SQRT2, -10.0 * M_SQRT2}));
    ON_CALL(agent, GetYaw()).WillByDefault(Return(1.25 * M_PI));
    ON_CALL(agent, GetVehicleModelParameters()).WillByDefault(Return(heavyVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersAgent {pair1};
    ON_CALL(agent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersAgent));

    ON_CALL(opponent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{20.0 * M_SQRT2, 20.0 * M_SQRT2}));
    ON_CALL(opponent, GetYaw()).WillByDefault(Return(0.25 * M_PI));
    ON_CALL(opponent, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersOpponent {pair0};
    ON_CALL(opponent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersOpponent));

    NiceMock<FakeWorld> world;
    ON_CALL(world, GetAgent(0)).WillByDefault(Return(&agent));
    ON_CALL(world, GetAgent(1)).WillByDefault(Return(&opponent));

    DynamicsCollisionImplementation dynamicsCollision(
                "",
                false,
                0,
                0,
                0,
                0,
                nullptr,
                &world,
                nullptr,
                nullptr,
                nullptr,
                &agent);

    dynamicsCollision.Trigger(0);

    ASSERT_NEAR(dynamicsCollision.GetVelocity(), 0.0, 1e-9);
}

TEST_F(DynamicsCollision_Test, CollisionOfThreeAgentsOnlyInXDirection)
{
    NiceMock<FakeAgent> agent;
    NiceMock<FakeAgent> opponent1;
    NiceMock<FakeAgent> opponent2;
    std::pair<ObjectTypeOSI,int> pair0 = std::make_pair(ObjectTypeOSI::Vehicle, 0);
    std::pair<ObjectTypeOSI,int> pair1 = std::make_pair(ObjectTypeOSI::Vehicle, 1);
    std::pair<ObjectTypeOSI,int> pair2 = std::make_pair(ObjectTypeOSI::Vehicle, 2);

    ON_CALL(agent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{40.0, 0.0}));
    ON_CALL(agent, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(agent, GetVehicleModelParameters()).WillByDefault(Return(heavyVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersAgent {pair1, pair2};
    ON_CALL(agent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersAgent));

    ON_CALL(opponent1, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{20.0, 0.0}));
    ON_CALL(opponent1, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(opponent1, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersOpponent1 {pair0, pair2};
    ON_CALL(opponent1, GetCollisionPartners()).WillByDefault(Return(collisionPartnersOpponent1));

    ON_CALL(opponent2, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{0.0, 0.0}));
    ON_CALL(opponent2, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(opponent2, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersOpponent2 {pair1, pair0};
    ON_CALL(opponent2, GetCollisionPartners()).WillByDefault(Return(collisionPartnersOpponent2));

    NiceMock<FakeWorld> world;
    ON_CALL(world, GetAgent(0)).WillByDefault(Return(&agent));
    ON_CALL(world, GetAgent(1)).WillByDefault(Return(&opponent1));
    ON_CALL(world, GetAgent(2)).WillByDefault(Return(&opponent2));

    DynamicsCollisionImplementation dynamicsCollision(
                "",
                false,
                0,
                0,
                0,
                0,
                nullptr,
                &world,
                nullptr,
                nullptr,
                nullptr,
                &agent);

    dynamicsCollision.Trigger(0);

    ASSERT_THAT(dynamicsCollision.GetVelocity(), DoubleNear(25.0, 1e-3));
    ASSERT_THAT(dynamicsCollision.GetMovingDirection(), DoubleNear(0.0, 1e-3));
}

TEST_F(DynamicsCollision_Test, CollisionOfThreeAgentsInDifferentDirections)
{
    NiceMock<FakeAgent> agent;
    NiceMock<FakeAgent> opponent1;
    NiceMock<FakeAgent> opponent2;
    std::pair<ObjectTypeOSI,int> pair0 = std::make_pair(ObjectTypeOSI::Vehicle, 0);
    std::pair<ObjectTypeOSI,int> pair1 = std::make_pair(ObjectTypeOSI::Vehicle, 1);
    std::pair<ObjectTypeOSI,int> pair2 = std::make_pair(ObjectTypeOSI::Vehicle, 2);

    ON_CALL(agent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{0.0, -30.0}));
    ON_CALL(agent, GetYaw()).WillByDefault(Return(-0.5 * M_PI));
    ON_CALL(agent, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersAgent {pair1, pair2};
    ON_CALL(agent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersAgent));

    ON_CALL(opponent1, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{30.0, -30.0}));
    ON_CALL(opponent1, GetYaw()).WillByDefault(Return(-0.25 * M_PI));
    ON_CALL(opponent1, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersOpponent1 {pair0, pair2};
    ON_CALL(opponent1, GetCollisionPartners()).WillByDefault(Return(collisionPartnersOpponent1));

    ON_CALL(opponent2, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{30.0, 0.0}));
    ON_CALL(opponent2, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(opponent2, GetVehicleModelParameters()).WillByDefault(Return(lightVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersOpponent2 {pair1, pair0};
    ON_CALL(opponent2, GetCollisionPartners()).WillByDefault(Return(collisionPartnersOpponent2));

    NiceMock<FakeWorld> world;
    ON_CALL(world, GetAgent(0)).WillByDefault(Return(&agent));
    ON_CALL(world, GetAgent(1)).WillByDefault(Return(&opponent1));
    ON_CALL(world, GetAgent(2)).WillByDefault(Return(&opponent2));

    DynamicsCollisionImplementation dynamicsCollision(
                "",
                false,
                0,
                0,
                0,
                0,
                nullptr,
                &world,
                nullptr,
                nullptr,
                nullptr,
                &agent);

    dynamicsCollision.Trigger(0);

    double expectedVelocity = 20.0 * std::sqrt(2);
    ASSERT_THAT(dynamicsCollision.GetVelocity(), DoubleNear(expectedVelocity, 1e-3));
    ASSERT_THAT(dynamicsCollision.GetMovingDirection(), DoubleNear(-0.25 * M_PI, 1e-3));
}

TEST_F(DynamicsCollision_Test, CollisionOfAgentWithFixedObject)
{
    NiceMock<FakeAgent> agent;
    std::pair<ObjectTypeOSI,int> pair = std::make_pair(ObjectTypeOSI::Object, 0);

    ON_CALL(agent, GetVelocity(_)).WillByDefault(Return(Common::Vector2d{40.0, 0.0}));
    ON_CALL(agent, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(agent, GetVehicleModelParameters()).WillByDefault(Return(heavyVehicle));
    std::vector<std::pair<ObjectTypeOSI,int>> collisionPartnersAgent {pair};
    ON_CALL(agent, GetCollisionPartners()).WillByDefault(Return(collisionPartnersAgent));

    NiceMock<FakeWorld> world;
    ON_CALL(world, GetAgent(0)).WillByDefault(Return(&agent));

    DynamicsCollisionImplementation dynamicsCollision(
                "",
                false,
                0,
                0,
                0,
                0,
                nullptr,
                &world,
                nullptr,
                nullptr,
                nullptr,
                &agent);

    dynamicsCollision.Trigger(0);

    ASSERT_THAT(dynamicsCollision.GetVelocity(), DoubleNear(0.0, 1e-3));
}


