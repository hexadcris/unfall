/********************************************************************************
 * Copyright (c) 2018-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SceneryConverter.h"
#include "EntityRepository.h"

#include "fakeAgent.h"
#include "fakeLane.h"
#include "fakeLaneManager.h"
#include "fakeMovingObject.h"
#include "fakeOdRoad.h"
#include "fakeRoadObject.h"
#include "fakeRoadLaneSection.h"
#include "fakeSection.h"
#include "fakeWorld.h"
#include "fakeWorldData.h"
#include "fakeConnection.h"
#include "fakeJunction.h"
#include "fakeDataBuffer.h"

#include "Generators/laneGeometryElementGenerator.h"

using namespace OWL;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SetArgReferee;
using ::testing::Invoke;
using ::testing::Const;
using ::testing::_;

class FakeRepository : public openpass::entity::RepositoryInterface
{
public:
    MOCK_METHOD0(Reset, void());
    MOCK_METHOD1(Register, openpass::type::EntityId (openpass::type::EntityInfo));
    MOCK_METHOD2(Register, openpass::type::EntityId (openpass::entity::EntityType entityType, openpass::type::EntityInfo));
};

std::tuple<const OWL::Primitive::LaneGeometryJoint*, const OWL::Primitive::LaneGeometryJoint*> CreateSectionPartJointsRect(double length)
{
    osi3::Lane osiLane;
    OWL::Implementation::Lane lane(&osiLane, nullptr, -1);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    0.0,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    return lane.GetNeighbouringJoints(length * 0.5);
}

class FakeScenery : public SceneryInterface
{
public:
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD1(AddRoad, RoadInterface *(const std::string &id));
    MOCK_METHOD1(AddJunction, JunctionInterface *(const std::string &id));
    MOCK_CONST_METHOD0(GetRoads, std::map<std::string, RoadInterface *> &());
    MOCK_CONST_METHOD1(GetRoad, RoadInterface *(const std::string &id));
    MOCK_CONST_METHOD0(GetJunctions, std::map<std::string, JunctionInterface *> &());
    MOCK_CONST_METHOD1(GetJunction, JunctionInterface *(const std::string &id));
};

class FakeRoadLink : public RoadLinkInterface
{
public:
    MOCK_CONST_METHOD0(GetType, RoadLinkType());
    MOCK_CONST_METHOD0(GetElementType, RoadLinkElementType());
    MOCK_CONST_METHOD0(GetElementId, const std::string &());
    MOCK_CONST_METHOD0(GetContactPoint, ContactPointType());
};

void Connect(const RoadInterface* incomingRoad, const RoadInterface* connectingRoad,
             const RoadInterface* outgoingRoad, ContactPointType incomingContactPoint,
             ContactPointType connectingContactPoint, ContactPointType outgoingContactPoint,
             const std::map<int, int>& laneIdMapping)
{

}


TEST(SceneryConverter, RefactoringSafeguard_DoNotDelete)
{
    FakeScenery stubScenery;
    FakeConnection stubConnection;
    ON_CALL(stubConnection, GetContactPoint()).WillByDefault(Return(ContactPointType::Start));

    FakeOdRoad incomingRoad; 
    std::string incomingRoadId = "incomingRoadId";
    ON_CALL(stubConnection, GetIncommingRoadId()).WillByDefault(ReturnRef(incomingRoadId));
    ON_CALL(stubScenery, GetRoad("incomingRoadId")).WillByDefault(Return(&incomingRoad));

    FakeOdRoad connectingRoad;
    std::string connectingRoadId = "connectingRoadId";
    ON_CALL(stubConnection, GetConnectingRoadId()).WillByDefault(ReturnRef(connectingRoadId));
    ON_CALL(stubScenery, GetRoad("connectingRoadId")).WillByDefault(Return(&connectingRoad));

    FakeRoadLink roadLink;
    std::string outgoingRoadId = "outgoingRoadId";
    ON_CALL(roadLink, GetType()).WillByDefault(Return(RoadLinkType::Successor));
    ON_CALL(roadLink, GetElementId()).WillByDefault(ReturnRef(outgoingRoadId));

    std::vector<RoadLinkInterface*> roadLinks = { {&roadLink} };
    ON_CALL(connectingRoad, GetRoadLinks()).WillByDefault(ReturnRef(roadLinks));

    FakeOdRoad outgoingRoad;
    ON_CALL(stubScenery, GetRoad("outgoingRoadId")).WillByDefault(Return(&outgoingRoad));

    FakeRoadLaneSection stubRoadLaneSection;
    std::vector<RoadLaneSectionInterface *> stubLaneSections = { &stubRoadLaneSection };
    ON_CALL(incomingRoad, GetLaneSections()).WillByDefault(ReturnRef(stubLaneSections));
    ON_CALL(connectingRoad, GetLaneSections()).WillByDefault(ReturnRef(stubLaneSections));

    FakeJunction stubJunction;
    std::map<std::string, ConnectionInterface*> stubConnections = {{"", &stubConnection}};
    ON_CALL(stubJunction, GetConnections()).WillByDefault(Return(stubConnections));

    std::map<int,int> links = {{}};
    ON_CALL(stubConnection, GetLinks()).WillByDefault(ReturnRef(links));

    auto [status, error_message] = Internal::ConnectJunction(
            &stubScenery,
            &stubJunction,

            [&](const JunctionInterface*, const RoadInterface *incomingRoad, const RoadInterface *connectingRoad, const RoadInterface *outgoingRoad,
                ContactPointType incomingContactPoint, ContactPointType connectingContactPoint, ContactPointType outgoingContactPoint,
                const std::map<int, int> &laneIdMapping) {
                Connect(incomingRoad, connectingRoad, outgoingRoad, incomingContactPoint, connectingContactPoint,
                                      outgoingContactPoint, laneIdMapping); });

    ASSERT_THAT(status, true);
}
