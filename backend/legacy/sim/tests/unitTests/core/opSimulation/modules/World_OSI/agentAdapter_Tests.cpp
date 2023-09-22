/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "AgentAdapter.h"
#include "fakeMovingObject.h"
#include "fakeWorld.h"
#include "fakeWorldData.h"
#include "fakeCallback.h"
#include "fakeAgentBlueprint.h"
#include "fakeLocalizer.h"
#include "Localization.h"
#include <optional>

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsNull;
using ::testing::_;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Lt;
using ::testing::AllOf;
using ::testing::DoubleEq;
using ::testing::ElementsAreArray;

class AgentAdapterTest
{
public:
explicit AgentAdapterTest(const AgentVehicleType type, OWL::Implementation::MovingObject& obj) :
        fakeLocalizer(fakeWorldData), fakeWorld(), fakeCallbacks()
{
    RoadGraph graph = RoadGraph(2);
    graph.added_vertex(0);

    Route route ;
    route.root = 0;
    route.target = 0;
    route.roadGraph = graph;

    spawnParameter.positionX = 0;
    spawnParameter.positionY = 0;
    spawnParameter.yawAngle = 0;
    spawnParameter.velocity = 0;
    spawnParameter.acceleration = 0;
    spawnParameter.gear = 1;
    spawnParameter.route = route;

    ON_CALL(fakeWorldData, AddMovingObject(321)).WillByDefault(ReturnRef(obj));
    ON_CALL(fakeAgentBlueprint, GetSpawnParameter()).WillByDefault(ReturnRef(spawnParameter));
    ON_CALL(fakeAgentBlueprint, GetSensorParameters()).WillByDefault(Return(sensorParameter));
    World::Localization::Result localizerResult;
    ON_CALL(fakeLocalizer, Locate(_,_)).WillByDefault(Return(localizerResult));

    agentAdapter = std::make_shared<AgentAdapter>(obj, fakeWorld, fakeCallbacks, fakeLocalizer);

    switch (type)
    {
    case AgentVehicleType::Pedestrian:
        ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(returnPedestrian()));
        break;
    case AgentVehicleType::Car:
        ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(returnCar()));
        break;
    case AgentVehicleType::Motorbike:
        ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(returnMotorbike()));
        break;
    case AgentVehicleType::NONE:
        ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(returnNONE()));
        break;
    case AgentVehicleType::Undefined:
        ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(returnUndefined()));
        break;
    case AgentVehicleType::Bicycle:
        ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(returnBicycle()));
        break;
    case AgentVehicleType::Truck:
        ON_CALL(fakeAgentBlueprint, GetVehicleModelParameters()).WillByDefault(Return(returnTruck()));
        break;
    }


    agentAdapter->InitParameter(fakeAgentBlueprint);

}

    static VehicleModelParameters returnNONE()
    {
        VehicleModelParameters params;
        params.vehicleType = AgentVehicleType::NONE;
        return params;
    }

    static VehicleModelParameters returnUndefined()
    {
        VehicleModelParameters params;
        params.vehicleType = AgentVehicleType::Undefined;
        return params;
    }

    static VehicleModelParameters returnBicycle()
    {
        VehicleModelParameters params;
        params = returnMotorbike();
        params.vehicleType = AgentVehicleType::Bicycle;
        return params;
    }

    static VehicleModelParameters returnTruck()
    {
        VehicleModelParameters params;
        params = returnCar();
        params.vehicleType = AgentVehicleType::Truck;
        return params;
    }

    static VehicleModelParameters returnPedestrian()
    {
        VehicleModelParameters defaultPedestrian;
        defaultPedestrian.vehicleType = AgentVehicleType::Pedestrian;
        return defaultPedestrian;
    }

    static VehicleModelParameters returnCar()
    {
        VehicleModelParameters defaultCar;
        defaultCar.vehicleType = AgentVehicleType::Car;
        defaultCar.frontAxle.positionX = 0.3;
        defaultCar.frontAxle.positionZ = 0.1;
        defaultCar.frontAxle.wheelDiameter = 2.0;
        defaultCar.frontAxle.trackWidth = 2.0;
        defaultCar.rearAxle.trackWidth = 2.0;
        defaultCar.rearAxle.wheelDiameter = 2.0;
        defaultCar.rearAxle.positionX = -0.3;
        defaultCar.rearAxle.positionZ = 0.1;
        defaultCar.boundingBoxCenter.x = defaultCar.boundingBoxCenter.y = 0;
        defaultCar.boundingBoxCenter.z = 0.2;
        defaultCar.boundingBoxDimensions.height = 0.5;
        defaultCar.boundingBoxDimensions.width = 0.5;
        defaultCar.boundingBoxDimensions.length = 0.5;
        defaultCar.properties.emplace("SteeringRatio",10.0);

        return defaultCar;
    }

    static VehicleModelParameters returnMotorbike()
    {
        VehicleModelParameters defaultBicycle;
        defaultBicycle.vehicleType = AgentVehicleType::Motorbike;
        defaultBicycle.frontAxle.positionX = 0.3;
        defaultBicycle.frontAxle.positionZ = 0.1;
        defaultBicycle.rearAxle.positionX = -0.3;
        defaultBicycle.rearAxle.positionZ = 0.1;
        defaultBicycle.frontAxle.wheelDiameter = 1.5;
        defaultBicycle.rearAxle.wheelDiameter = 1.5;
        defaultBicycle.frontAxle.trackWidth = 2.0;
        defaultBicycle.rearAxle.trackWidth = 2.0;
        defaultBicycle.boundingBoxCenter.x = defaultBicycle.boundingBoxCenter.y = 0;
        defaultBicycle.boundingBoxCenter.z = 0.2;
        defaultBicycle.boundingBoxDimensions.height = 0.5;
        defaultBicycle.boundingBoxDimensions.width = 0.25;
        defaultBicycle.boundingBoxDimensions.length = 0.5;
        defaultBicycle.properties.emplace("SteeringRatio",5.0);

        return defaultBicycle;
    }

    void SetWheelRotation(double angle, double velocity, double acceleration)
    {
        agentAdapter->SetWheelsRotationRateAndOrientation(angle, velocity, acceleration);
    }

    std::shared_ptr<AgentAdapter> agentAdapter;

private:
    testing::NiceMock<OWL::Fakes::WorldData> fakeWorldData;
    WorldInterface *fakeWorld;
    const CallbackInterface *fakeCallbacks;

    const testing::NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    const testing::NiceMock<World::Localization::FakeLocalizer> fakeLocalizer;
    SpawnParameter spawnParameter;
    openpass::sensors::Parameters sensorParameter;

};

TEST(AgentAdapter_Test, GenerateCarWheels)
{
    osi3::MovingObject osi_car;
    osi3::MovingObject* osi_p = &osi_car;
    OWL::Implementation::MovingObject movingObject(&osi_car);
    AgentAdapterTest cartest (AgentVehicleType::Car, movingObject);

    VehicleModelParameters input_car = AgentAdapterTest::returnCar();

    ASSERT_FLOAT_EQ(input_car.frontAxle.positionX - input_car.boundingBoxCenter.x , osi_car.vehicle_attributes().bbcenter_to_front().x());
    ASSERT_FLOAT_EQ(0, osi_car.vehicle_attributes().bbcenter_to_front().y());
    ASSERT_FLOAT_EQ(input_car.frontAxle.positionZ - input_car.boundingBoxCenter.z , osi_car.vehicle_attributes().bbcenter_to_front().z());

    ASSERT_EQ(osi_car.type(), osi3::MovingObject_Type_TYPE_VEHICLE);
    ASSERT_EQ(osi_car.vehicle_attributes().wheel_data_size(), 4);
    ASSERT_EQ(osi_car.vehicle_attributes().number_wheels(), 4);

    OWL::WheelData w = movingObject.GetWheelData(0,0).value();

    ASSERT_EQ(w.index, 0);
    ASSERT_EQ(movingObject.GetWheelData(0,0)->axle, 0);
    ASSERT_EQ(movingObject.GetWheelData(0,1)->index, 1);
    ASSERT_EQ(movingObject.GetWheelData(0,1)->axle, 0);
    ASSERT_EQ(movingObject.GetWheelData(1,0)->index, 0);
    ASSERT_EQ(movingObject.GetWheelData(1,0)->axle, 1);
    ASSERT_EQ(movingObject.GetWheelData(1,1)->index, 1);
    ASSERT_EQ(movingObject.GetWheelData(1,1)->axle, 1);

    ASSERT_EQ(movingObject.GetWheelData(0,2), std::nullopt);
    ASSERT_EQ(movingObject.GetWheelData(0,2), std::nullopt);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->wheelRadius, input_car.frontAxle.wheelDiameter / 2.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,1)->wheelRadius, input_car.frontAxle.wheelDiameter / 2.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->wheelRadius, input_car.rearAxle.wheelDiameter / 2.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,1)->wheelRadius, input_car.rearAxle.wheelDiameter / 2.0);


    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->position.x, input_car.frontAxle.positionX - input_car.boundingBoxCenter.x);
    ASSERT_FLOAT_EQ(osi_car.vehicle_attributes().bbcenter_to_front().x(), movingObject.GetWheelData(0,0)->position.x);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,1)->position.x, input_car.frontAxle.positionX - input_car.boundingBoxCenter.x);
    ASSERT_FLOAT_EQ(osi_car.vehicle_attributes().bbcenter_to_front().x(), movingObject.GetWheelData(0,1)->position.x);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->position.x, input_car.rearAxle.positionX - input_car.boundingBoxCenter.x);
    ASSERT_FLOAT_EQ(osi_car.vehicle_attributes().bbcenter_to_rear().x(), movingObject.GetWheelData(1,0)->position.x);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,1)->position.x, input_car.rearAxle.positionX - input_car.boundingBoxCenter.x);
    ASSERT_FLOAT_EQ(osi_car.vehicle_attributes().bbcenter_to_rear().x(), movingObject.GetWheelData(1,1)->position.x);

    ASSERT_GE(movingObject.GetWheelData(0,0)->position.x, 0);
    ASSERT_GE(movingObject.GetWheelData(0,1)->position.x,0);
    ASSERT_LE(movingObject.GetWheelData(1,0)->position.x,0);
    ASSERT_LE(movingObject.GetWheelData(1,1)->position.x,0);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->position.y, - (input_car.frontAxle.trackWidth / 2.0f - input_car.boundingBoxCenter.y));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,1)->position.y, (input_car.frontAxle.trackWidth / 2.0f - input_car.boundingBoxCenter.y));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->position.y, - (input_car.frontAxle.trackWidth / 2.0f - input_car.boundingBoxCenter.y));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,1)->position.y, (input_car.frontAxle.trackWidth / 2.0f - input_car.boundingBoxCenter.y));

    ASSERT_LE(movingObject.GetWheelData(0,0)->position.y, 0);
    ASSERT_GE(movingObject.GetWheelData(0,1)->position.y,0);
    ASSERT_LE(movingObject.GetWheelData(1,0)->position.y,0);
    ASSERT_GE(movingObject.GetWheelData(1,1)->position.y,0);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->position.z, osi_car.vehicle_attributes().bbcenter_to_front().z());
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,1)->position.z, osi_car.vehicle_attributes().bbcenter_to_front().z());
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->position.z, osi_car.vehicle_attributes().bbcenter_to_rear().z());
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,1)->position.z, osi_car.vehicle_attributes().bbcenter_to_rear().z());

    cartest.SetWheelRotation(0.3, 2.0, 0.41);

    auto rotation_rate_front = 2.0 / (input_car.frontAxle.wheelDiameter / 2.0);
    auto rotation_rate_rear = 2.0 / (input_car.rearAxle.wheelDiameter / 2.0);
    auto dTime = (2.0 - 0.0) / 0.41;

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->rotation_rate, rotation_rate_front);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,1)->rotation_rate, rotation_rate_front);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->rotation_rate, rotation_rate_rear);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,1)->rotation_rate, rotation_rate_rear);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->orientation.pitch, CommonHelper::SetAngleToValidRange(rotation_rate_front * dTime));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,1)->orientation.pitch, CommonHelper::SetAngleToValidRange(rotation_rate_front * dTime));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->orientation.pitch, CommonHelper::SetAngleToValidRange(rotation_rate_rear * dTime));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,1)->orientation.pitch, CommonHelper::SetAngleToValidRange(rotation_rate_rear * dTime));

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->orientation.roll, 0.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,1)->orientation.roll, 0.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->orientation.roll, 0.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,1)->orientation.roll, 0.0);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->orientation.yaw, 0.3 * input_car.properties.at("SteeringRatio"));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,1)->orientation.yaw, 0.3 * input_car.properties.at("SteeringRatio"));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->orientation.yaw, 0.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,1)->orientation.yaw, 0.0);

}

TEST(AgentAdapter_Test, GenerateMotorbikeWheels)
{
    osi3::MovingObject osi_bike;
    OWL::Implementation::MovingObject movingObject(&osi_bike);
    AgentAdapterTest biketest(AgentVehicleType::Motorbike, movingObject);

    VehicleModelParameters input_bike = AgentAdapterTest::returnMotorbike();

    ASSERT_FLOAT_EQ(input_bike.frontAxle.positionX - input_bike.boundingBoxCenter.x , osi_bike.vehicle_attributes().bbcenter_to_front().x());
    ASSERT_FLOAT_EQ(0, osi_bike.vehicle_attributes().bbcenter_to_front().y());
    ASSERT_FLOAT_EQ(input_bike.frontAxle.positionZ - input_bike.boundingBoxCenter.z, osi_bike.vehicle_attributes().bbcenter_to_front().z());

    ASSERT_EQ(osi_bike.type(), osi3::MovingObject_Type_TYPE_VEHICLE);
    ASSERT_EQ(osi_bike.vehicle_attributes().wheel_data_size(), 2);
    ASSERT_EQ(osi_bike.vehicle_attributes().number_wheels(), 2);

    ASSERT_EQ(movingObject.GetWheelData(0,1), std::nullopt);
    ASSERT_EQ(movingObject.GetWheelData(1,1), std::nullopt);

    ASSERT_EQ(movingObject.GetWheelData(0,0)->index, 0);
    ASSERT_EQ(movingObject.GetWheelData(0,0)->axle, 0);
    ASSERT_EQ(movingObject.GetWheelData(1,0)->index, 0);
    ASSERT_EQ(movingObject.GetWheelData(1,0)->axle, 1);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->wheelRadius, input_bike.frontAxle.wheelDiameter / 2.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->wheelRadius, input_bike.rearAxle.wheelDiameter / 2.0);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->position.x, input_bike.frontAxle.positionX - input_bike.boundingBoxCenter.x);
    ASSERT_FLOAT_EQ(osi_bike.vehicle_attributes().bbcenter_to_front().x(), movingObject.GetWheelData(0,0)->position.x);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->position.x, input_bike.rearAxle.positionX - input_bike.boundingBoxCenter.x);
    ASSERT_FLOAT_EQ(osi_bike.vehicle_attributes().bbcenter_to_rear().x(), movingObject.GetWheelData(0,1)->position.x);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->position.y, 0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->position.y, 0);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->position.z, osi_bike.vehicle_attributes().bbcenter_to_front().z());
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->position.z, osi_bike.vehicle_attributes().bbcenter_to_rear().z());

    biketest.SetWheelRotation(0.3, 2.0, 0.41);

    auto rotation_rate_front = 2.0 / (input_bike.frontAxle.wheelDiameter / 2.0);
    auto rotation_rate_rear = 2.0 / (input_bike.rearAxle.wheelDiameter / 2.0);
    auto dTime = (2.0 - 0.0) / 0.41;

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->rotation_rate, rotation_rate_front);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->rotation_rate, rotation_rate_rear);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->orientation.pitch, CommonHelper::SetAngleToValidRange(rotation_rate_front * dTime));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->orientation.pitch, CommonHelper::SetAngleToValidRange(rotation_rate_front * dTime));

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->orientation.roll, 0.0);
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->orientation.roll, 0.0);

    ASSERT_FLOAT_EQ(movingObject.GetWheelData(0,0)->orientation.yaw, 0.3 * input_bike.properties.at("SteeringRatio"));
    ASSERT_FLOAT_EQ(movingObject.GetWheelData(1,0)->orientation.yaw, 0.0);
}

TEST(AgentAdapter_Test, GeneratePedestrianWheels)
{
    osi3::MovingObject osi_person;
    OWL::Implementation::MovingObject movingObject(&osi_person);
    AgentAdapterTest pedestrianTest(AgentVehicleType::Pedestrian, movingObject);

    VehicleModelParameters input_car = AgentAdapterTest::returnCar();

    ASSERT_EQ(osi_person.type(), osi3::MovingObject_Type_TYPE_PEDESTRIAN);
    ASSERT_EQ(osi_person.vehicle_attributes().wheel_data_size(), 0);
    ASSERT_EQ(osi_person.vehicle_attributes().number_wheels(), 0);

    ASSERT_EQ(movingObject.GetWheelData(0,0), std::nullopt);
}

TEST(AgentAdapter_Test, SetWheelData)
{

}

TEST(MovingObject_Tests, SetAndGetReferencePointPosition_ReturnsCorrectPosition)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject);
    movingObject.SetLength(8.0);
    movingObject.SetBoundingBoxCenterToRear(7.0, 0.0, 0.0);
    movingObject.SetYaw(0.5);
    movingObject.SetReferencePointPosition(position);
    OWL::Primitive::AbsPosition resultPosition = movingObject.GetReferencePointPosition();
    ASSERT_THAT(resultPosition.x, DoubleEq(position.x));
    ASSERT_THAT(resultPosition.y, DoubleEq(position.y));
    ASSERT_THAT(resultPosition.z, DoubleEq(position.z));
}

TEST(MovingObject_Tests, SetAndGetReferencePointPositionWithYawChangeInBetween_ReturnsCorrectPosition)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject);
    movingObject.SetLength(8.0);
    movingObject.SetBoundingBoxCenterToRear(7.0, 0.0, 0.0);
    movingObject.SetYaw(0.5);
    movingObject.SetReferencePointPosition(position);
    movingObject.SetYaw(0.7);
    OWL::Primitive::AbsPosition resultPosition = movingObject.GetReferencePointPosition();
    ASSERT_THAT(resultPosition.x, DoubleEq(position.x));
    ASSERT_THAT(resultPosition.y, DoubleEq(position.y));
    ASSERT_THAT(resultPosition.z, DoubleEq(position.z));
}

TEST(MovingObject_Tests, SetReferencePointPosition_SetsCorrectPositionOnOSIObject)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject);
    movingObject.SetLength(8.0);
    movingObject.SetBoundingBoxCenterToRear(-2.0, 0.0, 0.0);
    movingObject.SetYaw(M_PI * 0.25);
    movingObject.SetReferencePointPosition(position);
    auto resultPosition = osiObject.base().position();
    ASSERT_THAT(resultPosition.x(), DoubleEq(position.x + std::sqrt(2)));
    ASSERT_THAT(resultPosition.y(), DoubleEq(position.y + std::sqrt(2)));
    ASSERT_THAT(resultPosition.z(), DoubleEq(position.z));
}

TEST(MovingObject_Tests, SetWheelsRotationRateAndOrientation)
{
    auto wheelDiameter = 1.0;

    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject);
    movingObject.SetType(AgentVehicleType::Car);

    OWL::WheelData wheeldata {};
    wheeldata.wheelRadius = wheelDiameter / 2.0;
    wheeldata.axle = 0;
    wheeldata.index = 0;

    movingObject.AddWheel(wheeldata);
    wheeldata.index = 1;
    movingObject.AddWheel(wheeldata);
    wheeldata.index = 0;
    wheeldata.axle = 1;
    movingObject.AddWheel(wheeldata);
    wheeldata.index = 1;
    wheeldata.axle = 1;
    movingObject.AddWheel(wheeldata);

    movingObject.SetWheelsRotationRateAndOrientation(0.1, wheelDiameter / 2.0, wheelDiameter / 2.0, 2.0);
    movingObject.SetFrontAxleSteeringYaw(0.4);

    auto rotationrate = 0.1 / (wheelDiameter / 2.0);

    std::optional<const OWL::WheelData> FrontWheel = movingObject.GetWheelData(0,0);
    std::optional<const OWL::WheelData> FrontWheel2 = movingObject.GetWheelData(0,1);
    std::optional<const OWL::WheelData> RearWheel = movingObject.GetWheelData(1,0);
    std::optional<const OWL::WheelData> RearWheel2 = movingObject.GetWheelData(1,1);

    ASSERT_FLOAT_EQ(FrontWheel.value().orientation.yaw, 0.4);
    ASSERT_FLOAT_EQ(RearWheel.value().orientation.yaw, 0.0);
    ASSERT_FLOAT_EQ(FrontWheel2.value().orientation.yaw, 0.4);
    ASSERT_FLOAT_EQ(RearWheel2.value().orientation.yaw, 0.0);

    ASSERT_FLOAT_EQ(FrontWheel.value().orientation.pitch, 0.4);
    ASSERT_FLOAT_EQ(RearWheel.value().orientation.pitch, 0.4);
    ASSERT_FLOAT_EQ(FrontWheel2.value().orientation.pitch, 0.4);
    ASSERT_FLOAT_EQ(RearWheel2.value().orientation.pitch, 0.4);

    ASSERT_FLOAT_EQ(FrontWheel.value().orientation.roll, 0.0);
    ASSERT_FLOAT_EQ(RearWheel.value().orientation.roll, 0.0);
    ASSERT_FLOAT_EQ(FrontWheel2.value().orientation.roll, 0.0);
    ASSERT_FLOAT_EQ(RearWheel2.value().orientation.roll, 0.0);

    ASSERT_FLOAT_EQ(FrontWheel.value().rotation_rate, rotationrate);
    ASSERT_FLOAT_EQ(RearWheel.value().rotation_rate, rotationrate);
    ASSERT_FLOAT_EQ(FrontWheel2.value().rotation_rate, rotationrate);
    ASSERT_FLOAT_EQ(RearWheel2.value().rotation_rate, rotationrate);
}

TEST(MovingObject_Tests, SetAgentType_MapsCorrectOSIType)
{
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject);

    const std::vector<std::pair<AgentVehicleType, osi3::MovingObject_VehicleClassification_Type>> expectedVehicleTypes = 
    {{AgentVehicleType::Car, osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_MEDIUM_CAR},
     {AgentVehicleType::Motorbike, osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_MOTORBIKE},
     {AgentVehicleType::Bicycle, osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_BICYCLE},
     {AgentVehicleType::Truck, osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_HEAVY_TRUCK}};

    for (const auto & [agentVehicleType, expectedOsiVehicleType] : expectedVehicleTypes)
    {
        movingObject.SetType(agentVehicleType);

        ASSERT_THAT(osiObject.type(), osi3::MovingObject_Type::MovingObject_Type_TYPE_VEHICLE);
        ASSERT_THAT(osiObject.vehicle_classification().type(), expectedOsiVehicleType);
    }

    movingObject.SetType(AgentVehicleType::Pedestrian);
    ASSERT_THAT(osiObject.type(), osi3::MovingObject_Type::MovingObject_Type_TYPE_PEDESTRIAN);
}

struct CalculateBoundingBoxData
{
    double yaw;
    double roll;
    std::vector<std::pair<double,double>> expectedResult;
};

class CalculateBoundingBox_Tests : public testing::Test,
        public ::testing::WithParamInterface<CalculateBoundingBoxData>
{
};

class TestWorldObject : public WorldObjectAdapter
{
public:
    TestWorldObject (OWL::Interfaces::WorldObject& baseTrafficObject) :
        WorldObjectAdapter(baseTrafficObject)
    {}

    virtual double GetLaneRemainder(const std::string& roadId, Side) const{};
    virtual ObjectTypeOSI GetType() const {}
    virtual const RoadIntervals &GetTouchedRoads() const
    {
    }
    virtual Common::Vector2d GetAbsolutePosition(const ObjectPoint& objectPoint) const {return {0,0};}
    virtual const GlobalRoadPositions& GetRoadPosition(const ObjectPoint& point) const {return {};}
    virtual Common::Vector2d GetVelocity(ObjectPoint point) const {return {0,0};}
    virtual Common::Vector2d GetAcceleration(ObjectPoint point) const {return {0,0};}
    virtual bool Locate() {return false;}
    virtual void Unlocate() {};
};

TEST_P(CalculateBoundingBox_Tests, CalculateBoundingBox_ReturnCorrectPoints)
{
    const auto& data = GetParam();
    OWL::Fakes::MovingObject movingObject;
    OWL::Primitive::AbsPosition position{10, 20, 0};
    ON_CALL(movingObject, GetReferencePointPosition()).WillByDefault(Return(position));
    OWL::Primitive::Dimension dimension{6.0, 2.0, 1.6};
    ON_CALL(movingObject, GetDimension()).WillByDefault(Return(dimension));
    OWL::Primitive::AbsOrientation orientation{data.yaw, 0, data.roll};
    ON_CALL(movingObject, GetAbsOrientation()).WillByDefault(Return(orientation));

    TestWorldObject object(movingObject);

    auto result = object.GetBoundingBox2D();

    std::vector<std::pair<double,double>> resultPoints;
    for (const point_t point : result.outer())
    {
        resultPoints.emplace_back(bg::get<0>(point), bg::get<1>(point));
    }
    resultPoints.pop_back(); //in boost the last point is equal to the first

    ASSERT_THAT(resultPoints, ElementsAreArray(data.expectedResult));
}

INSTANTIATE_TEST_SUITE_P(BoundingBoxTest, CalculateBoundingBox_Tests,
                        testing::Values(
//!                      yaw     roll   expectedResult
CalculateBoundingBoxData{0.0,     0.0, {{7.0, 19.0},{7.0, 21.0}, {13.0, 21.0}, {13.0, 19.0}}},
CalculateBoundingBoxData{M_PI_2,  0.0, {{11.0, 17.0},{9.0, 17.0}, {9.0, 23.0}, {11.0, 23.0}}},
CalculateBoundingBoxData{0.0,  M_PI_4, {{7.0, 20 - 2.6*M_SQRT1_2},{7.0, 20.0 + M_SQRT1_2}, {13.0, 20.0  + M_SQRT1_2}, {13.0, 20.0 - 2.6*M_SQRT1_2}}},
CalculateBoundingBoxData{0.0, -M_PI_4, {{7.0, 20 - M_SQRT1_2},{7.0, 20.0 + 2.6*M_SQRT1_2}, {13.0, 20.0  + 2.6*M_SQRT1_2}, {13.0, 20.0 - M_SQRT1_2}}}
));
