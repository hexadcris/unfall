/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2018-2021 in-tech GmbH
 *               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  DataTypes.h
//! @brief This file provides the basic datatypes of the osi world layer (OWL)
//-----------------------------------------------------------------------------

#include <exception>
#include <list>
#include <memory>
#include <string>
#include <tuple>
#include <qglobal.h>

#include "include/roadInterface/roadInterface.h"
#include "include/roadInterface/roadLaneInterface.h"
#include "include/roadInterface/roadLaneSectionInterface.h"

#include "OWL/DataTypes.h"
#include "OWL/LaneGeometryElement.h"
#include "OWL/LaneGeometryJoint.h"
#include "OWL/Primitives.h"

#include "osi3/osi_groundtruth.pb.h"
#include "WorldObjectAdapter.h"
#include "OsiDefaultValues.h"

#include "common/hypot.h"

namespace OWL {

    namespace Implementation {

        Lane::Lane(osi3::Lane *osiLane, const Interfaces::Section *section, OdId odId) :
                osiLane(osiLane),
                odId(odId),
                section(section),
                leftLane(section ? new InvalidLane() : nullptr),
                rightLane(section ? new InvalidLane() : nullptr) {
        }

        Lane::~Lane() {
            for (auto laneGeometryElement: laneGeometryElements) {
                delete laneGeometryElement;
            }
            if (leftLaneIsDummy) {
                delete leftLane;
            }
            if (rightLaneIsDummy) {
                delete rightLane;
            }
        }

        void Lane::CopyToGroundTruth(osi3::GroundTruth &target) const {
            auto newLane = target.add_lane();
            newLane->CopyFrom(*osiLane);
        }

        Id Lane::GetId() const {
            return osiLane->id().value();
        }

        OdId Lane::GetOdId() const {
            return odId;
        }

        bool Lane::Exists() const {
            return osiLane->id().value() != InvalidId;
        }

        const Interfaces::Section &Lane::GetSection() const {
            return *section;
        }

        const Interfaces::Road &Lane::GetRoad() const {
            return section->GetRoad();
        }

        int Lane::GetRightLaneCount() const {
            int count = 0;
            const Interfaces::Lane *currentLane = &(GetRightLane());

            while (currentLane->Exists()) {
                count++;
                currentLane = &(currentLane->GetRightLane());
            }

            return count;
        }

        double Lane::GetLength() const {
            return length;
        }


        std::tuple<const Primitive::LaneGeometryJoint *, const Primitive::LaneGeometryJoint *>
        Lane::GetNeighbouringJoints(
                double distance) const {
            const Primitive::LaneGeometryJoint *nextJoint = nullptr;
            const Primitive::LaneGeometryJoint *prevJoint = nullptr;

            const auto &nextJointIt = std::find_if(laneGeometryJoints.cbegin(),
                                                   laneGeometryJoints.cend(),
                                                   [distance](const Primitive::LaneGeometryJoint &joint) {
                                                       return joint.sOffset > distance;
                                                   });


            if (nextJointIt != laneGeometryJoints.cend()) {
                nextJoint = &(*nextJointIt);
            }

            if (nextJointIt != laneGeometryJoints.cbegin()) {
                const auto &prevJointIt = std::prev(nextJointIt);
                prevJoint = &(*prevJointIt);
            }

            return {prevJoint, nextJoint};
        }

        const Primitive::LaneGeometryJoint::Points Lane::GetInterpolatedPointsAtDistance(double distance) const {
            Primitive::LaneGeometryJoint::Points interpolatedPoints{{0.0, 0.0},
                                                                    {0.0, 0.0},
                                                                    {0.0, 0.0}};
            const Primitive::LaneGeometryJoint *prevJoint;
            const Primitive::LaneGeometryJoint *nextJoint;
            std::tie(prevJoint, nextJoint) = GetNeighbouringJoints(distance);

            if (!prevJoint && !nextJoint) {
                return interpolatedPoints;
            } else if (prevJoint && !nextJoint) {
                return prevJoint->points;
            } else if (nextJoint && !prevJoint) {
                return nextJoint->points;
            }

            double interpolationFactor = (distance - prevJoint->sOffset) / (nextJoint->sOffset -
                                                                            prevJoint->sOffset);

            interpolatedPoints.left = prevJoint->points.left * (1.0 - interpolationFactor) + nextJoint->points.left *
                                                                                             interpolationFactor;
            interpolatedPoints.right = prevJoint->points.right * (1.0 - interpolationFactor) + nextJoint->points.right *
                                                                                               interpolationFactor;
            interpolatedPoints.reference =
                    prevJoint->points.reference * (1.0 - interpolationFactor) + nextJoint->points.reference *
                                                                                interpolationFactor;

            return interpolatedPoints;
        }

        double Lane::GetCurvature(double distance) const {
            const Primitive::LaneGeometryJoint *prevJoint;
            const Primitive::LaneGeometryJoint *nextJoint;
            std::tie(prevJoint, nextJoint) = GetNeighbouringJoints(distance);

            if (!prevJoint && !nextJoint) {
                return 0.0;
            } else if (prevJoint && !nextJoint) {
                return prevJoint->curvature;
            } else if (nextJoint && !prevJoint) {
                return 0.0;
            }

            double interpolationFactor = (distance - prevJoint->sOffset) / (nextJoint->sOffset -
                                                                            prevJoint->sOffset);
            double interpolatedCurvature = (1 - interpolationFactor) * prevJoint->curvature + interpolationFactor *
                                                                                              nextJoint->curvature;

            return interpolatedCurvature;
        }

        double Lane::GetWidth(double distance) const {
            const Primitive::LaneGeometryJoint *prevJoint;
            const Primitive::LaneGeometryJoint *nextJoint;
            std::tie(prevJoint, nextJoint) = GetNeighbouringJoints(distance);

            if (!prevJoint && !nextJoint) {
                return 0.0;
            } else if (prevJoint && !nextJoint) {
                return (prevJoint->points.left - prevJoint->points.right).Length();
            } else if (nextJoint && !prevJoint) {
                return 0.0;
            }

            double interpolationFactor = (distance - prevJoint->sOffset) / (nextJoint->sOffset -
                                                                            prevJoint->sOffset);
            double nextWidth = (nextJoint->points.left - nextJoint->points.right).Length();
            double prevWidth = (prevJoint->points.left - prevJoint->points.right).Length();
            double interpolatedWidth = (1.0 - interpolationFactor) * prevWidth + interpolationFactor * nextWidth;

            return interpolatedWidth;
        }

        double Lane::GetDirection(double distance) const {
            auto[prevJoint, nextJoint] = GetNeighbouringJoints(distance);

            if (!prevJoint) {
                return 0.0;
            }

            return prevJoint->sHdg;
        }

        const Interfaces::Lane &Lane::GetLeftLane() const {
            return *leftLane;
        }

        const Interfaces::Lane &Lane::GetRightLane() const {
            return *rightLane;
        }

        const std::vector<Id> Lane::GetLeftLaneBoundaries() const {
            std::vector<Id> laneBoundaries;
            for (const auto &laneBoundary: osiLane->classification().left_lane_boundary_id()) {
                laneBoundaries.push_back(laneBoundary.value());
            }
            return laneBoundaries;
        }

        const std::vector<Id> Lane::GetRightLaneBoundaries() const {
            std::vector<Id> laneBoundaries;
            for (const auto &laneBoundary: osiLane->classification().right_lane_boundary_id()) {
                laneBoundaries.push_back(laneBoundary.value());
            }
            return laneBoundaries;
        }

        double Lane::GetDistance(MeasurementPoint measurementPoint) const {
            if (laneGeometryElements.empty()) {
                throw std::runtime_error("Unexpected Lane without LaneGeometryElements");
            }

            if (measurementPoint == MeasurementPoint::RoadStart) {
                return laneGeometryElements.front()->joints.current.sOffset;
            }

            if (measurementPoint == MeasurementPoint::RoadEnd) {
                return laneGeometryElements.back()->joints.next.sOffset;
            }

            throw std::invalid_argument("measurement point not within valid bounds");
        }

        LaneType Lane::GetLaneType() const {
            return laneType;
        }

        bool Lane::Covers(double distance) const {
            if (GetDistance(MeasurementPoint::RoadStart) <= distance) {
                return next.empty() ?
                       GetDistance(MeasurementPoint::RoadEnd) > distance :
                       GetDistance(MeasurementPoint::RoadEnd) >= distance;

            }
            return false;
        }

        void Lane::SetLeftLane(const Interfaces::Lane &lane, bool transferLaneBoundary) {
            if (leftLaneIsDummy) {
                leftLaneIsDummy = false;
                delete leftLane;
            }

            leftLane = &lane;
            osiLane->mutable_classification()->add_left_adjacent_lane_id()->set_value(lane.GetId());

            if (transferLaneBoundary) {
                for (const auto &laneBoundary: lane.GetRightLaneBoundaries()) {
                    osiLane->mutable_classification()->add_left_lane_boundary_id()->set_value(laneBoundary);
                }
            }
        }

        void Lane::SetRightLane(const Interfaces::Lane &lane, bool transferLaneBoundary) {
            if (rightLaneIsDummy) {
                rightLaneIsDummy = false;
                delete rightLane;
            }

            rightLane = &lane;
            osiLane->mutable_classification()->add_right_adjacent_lane_id()->set_value(lane.GetId());
            if (transferLaneBoundary) {
                for (const auto &laneBoundary: lane.GetLeftLaneBoundaries()) {
                    osiLane->mutable_classification()->add_right_lane_boundary_id()->set_value(laneBoundary);
                }
            }
        }

        void Lane::SetLeftLaneBoundaries(const std::vector<Id> laneBoundaries) {
            for (const auto &laneBoundary: laneBoundaries) {
                osiLane->mutable_classification()->add_left_lane_boundary_id()->set_value(laneBoundary);
            }
        }

        void Lane::SetRightLaneBoundaries(const std::vector<Id> laneBoundaries) {
            for (const auto &laneBoundary: laneBoundaries) {
                osiLane->mutable_classification()->add_right_lane_boundary_id()->set_value(laneBoundary);
            }
        }

        void Lane::SetLaneType(LaneType specifiedType) {
            laneType = specifiedType;
        }

        const Interfaces::LaneAssignments &Lane::GetWorldObjects(bool direction) const {
            return worldObjects.Get(direction);
        }

        const Interfaces::TrafficSigns &Lane::GetTrafficSigns() const {
            return trafficSigns;
        }

        const Interfaces::RoadMarkings &Lane::GetRoadMarkings() const {
            return roadMarkings;
        }

        const Interfaces::TrafficLights &Lane::GetTrafficLights() const {
            return trafficLights;
        }

        void Lane::AddMovingObject(Interfaces::MovingObject &movingObject, const LaneOverlap &laneOverlap) {
            worldObjects.Insert(laneOverlap, &movingObject);
        }

        void Lane::AddStationaryObject(Interfaces::StationaryObject &stationaryObject, const LaneOverlap &laneOverlap) {
            worldObjects.Insert(laneOverlap, &stationaryObject);
            stationaryObjects.push_back({laneOverlap, &stationaryObject});
        }

        void Lane::AddWorldObject(Interfaces::WorldObject &worldObject, const LaneOverlap &laneOverlap) {
            if (worldObject.Is<MovingObject>()) {
                AddMovingObject(*worldObject.As<MovingObject>(), laneOverlap);
            } else if (worldObject.Is<StationaryObject>()) {
                AddStationaryObject(*worldObject.As<StationaryObject>(), laneOverlap);
            }
        }

        void Lane::AddTrafficSign(Interfaces::TrafficSign &trafficSign) {
            trafficSigns.push_back(&trafficSign);
        }

        void Lane::AddRoadMarking(Interfaces::RoadMarking &roadMarking) {
            roadMarkings.push_back(&roadMarking);
        }

        void Lane::AddTrafficLight(Interfaces::TrafficLight &trafficLight) {
            trafficLights.push_back(&trafficLight);
        }

        void Lane::ClearMovingObjects() {
            worldObjects.Clear();
            for (const auto&[laneOverlap, object]: stationaryObjects) {
                worldObjects.Insert(laneOverlap, object);
            }
        }

        void Lane::AddNext(const Interfaces::Lane *lane) {
            next.push_back(lane->GetId());
        }

        void Lane::AddPrevious(const Interfaces::Lane *lane) {
            previous.push_back(lane->GetId());
        }

        void Lane::SetLanePairings()
        {
            for (const auto predecesser : previous)
            {
                for (const auto successor : next)
                {
                    auto lanePairing = osiLane->mutable_classification()->add_lane_pairing();
                    lanePairing->mutable_antecessor_lane_id()->set_value(predecesser);
                    lanePairing->mutable_successor_lane_id()->set_value(successor);
                }
            }
            if (previous.empty())
            {
                for (const auto successor : next)
                {
                    auto lanePairing = osiLane->mutable_classification()->add_lane_pairing();
                    lanePairing->mutable_antecessor_lane_id()->set_value(-1);
                    lanePairing->mutable_successor_lane_id()->set_value(successor);
                }
            }
            if (next.empty())
            {
                for (const auto predecesser : previous)
                {
                    auto lanePairing = osiLane->mutable_classification()->add_lane_pairing();
                    lanePairing->mutable_antecessor_lane_id()->set_value(predecesser);
                    lanePairing->mutable_successor_lane_id()->set_value(-1);
                }
            }
        }

        const Interfaces::LaneGeometryElements &Lane::GetLaneGeometryElements() const {
            return laneGeometryElements;
        }

        void Lane::AddLaneGeometryJoint(const Common::Vector2d &pointLeft,
                                        const Common::Vector2d &pointCenter,
                                        const Common::Vector2d &pointRight,
                                        double sOffset,
                                        double curvature,
                                        double heading) {
            Primitive::LaneGeometryJoint newJoint;

            newJoint.points.left = pointLeft;
            newJoint.points.reference = pointCenter;
            newJoint.points.right = pointRight;
            newJoint.curvature = curvature;
            newJoint.sHdg = heading;
            newJoint.sOffset = sOffset;

            if (laneGeometryJoints.empty()) {
                laneGeometryJoints.push_back(newJoint);
                auto osiCenterpoint = osiLane->mutable_classification()->add_centerline();
                osiCenterpoint->set_x(pointCenter.x);
                osiCenterpoint->set_y(pointCenter.y);
                return;
            }

            const Primitive::LaneGeometryJoint &previousJoint = laneGeometryJoints.back();

            if (previousJoint.sOffset >= sOffset) {
                return; //Do not add the same point twice
            }

            length = sOffset - laneGeometryJoints.front().sOffset;
            Primitive::LaneGeometryElement *newElement = new Primitive::LaneGeometryElement(previousJoint, newJoint,
                                                                                            this);
            laneGeometryElements.push_back(newElement);
            laneGeometryJoints.push_back(newJoint);
            auto osiCenterpoint = osiLane->mutable_classification()->add_centerline();
            osiCenterpoint->set_x(pointCenter.x);
            osiCenterpoint->set_y(pointCenter.y);
        }

        Section::Section(double sOffset) :
                sOffset(sOffset) {}

        void Section::AddNext(const Interfaces::Section &section) {
            nextSections.push_back(&section);
        }

        void Section::AddPrevious(const Interfaces::Section &section) {
            previousSections.push_back(&section);
        }

        void Section::AddLane(const Interfaces::Lane &lane) {
            lanes.push_back(&lane);
        }

        void Section::SetRoad(Interfaces::Road *road) {
            this->road = road;
        }

        const Interfaces::Road &Section::GetRoad() const {
            return *road;
        }

        double Section::GetDistance(MeasurementPoint measurementPoint) const {
            if (measurementPoint == MeasurementPoint::RoadStart) {
                return GetSOffset();
            }

            if (measurementPoint == MeasurementPoint::RoadEnd) {
                return GetSOffset() + GetLength();
            }

            throw std::invalid_argument("measurement point not within valid bounds");

        }

        bool Section::Covers(double distance) const {
            if (GetDistance(MeasurementPoint::RoadStart) <= distance) {
                return nextSections.empty() ?
                       GetDistance(MeasurementPoint::RoadEnd) >= distance :
                       GetDistance(MeasurementPoint::RoadEnd) > distance;
            }
            return false;
        }

        bool Section::CoversInterval(double startDistance, double endDistance) const {
            double sectionStart = GetDistance(MeasurementPoint::RoadStart);
            double sectionEnd = GetDistance(MeasurementPoint::RoadEnd);

            bool startDistanceSmallerSectionEnd = nextSections.empty() ?
                                                  startDistance <= sectionEnd
                                                                       : startDistance < sectionEnd;
            bool endDistanceGreaterSectionStart = previousSections.empty() ? endDistance >= sectionStart
                                                                           : endDistance > sectionStart;

            return startDistanceSmallerSectionEnd && endDistanceGreaterSectionStart;
        }

        const Interfaces::Lanes &Section::GetLanes() const {
            return lanes;
        }

        double Section::GetLength() const {
            //All lanes must have equal length, so we simple take the length of first lane
            return lanes.empty() ? 0.0 : lanes.front()->GetLength();
        }

        void Section::SetCenterLaneBoundary(std::vector<Id> laneBoundaryId) {
            centerLaneBoundary = laneBoundaryId;
        }

        std::vector<Id> Section::GetCenterLaneBoundary() const {
            return centerLaneBoundary;
        }

        double Section::GetSOffset() const {
            return sOffset;
        }


        Road::Road(bool isInStreamDirection, const std::string &id) :
                isInStreamDirection(isInStreamDirection),
                id(id) {
        }

        const std::string &Road::GetId() const {
            return id;
        }

        const Interfaces::Sections &Road::GetSections() const {
            return sections;
        }

        void Road::AddSection(Interfaces::Section &section) {
            section.SetRoad(this);
            sections.push_back(&section);
        }

        double Road::GetLength() const {
            double length = 0;
            for (const auto &section: sections) {
                length += section->GetLength();
            }
            return length;
        }

        const std::string &Road::GetSuccessor() const {
            return successor;
        }

        const std::string &Road::GetPredecessor() const {
            return predecessor;
        }

        void Road::SetSuccessor(const std::string &successor) {
            this->successor = successor;
        }

        void Road::SetPredecessor(const std::string &predecessor) {
            this->predecessor = predecessor;
        }

        bool Road::IsInStreamDirection() const {
            return isInStreamDirection;
        }

        double Road::GetDistance(MeasurementPoint mp) const {
            if (mp == MeasurementPoint::RoadStart) {
                return 0;
            } else {
                return GetLength();
            }
        }

        StationaryObject::StationaryObject(osi3::StationaryObject *osiStationaryObject, void *linkedObject) :
                osiObject{osiStationaryObject} {
            this->linkedObject = linkedObject;
        }

        void StationaryObject::CopyToGroundTruth(osi3::GroundTruth &target) const {
            auto newStationaryObject = target.add_stationary_object();
            newStationaryObject->CopyFrom(*osiObject);
        }

        Primitive::Dimension StationaryObject::GetDimension() const {
            return GetDimensionFromOsiObject(osiObject);
        }

        Primitive::AbsOrientation StationaryObject::GetAbsOrientation() const {
            osi3::Orientation3d osiOrientation = osiObject->base().orientation();
            Primitive::AbsOrientation orientation;

            orientation.yaw = osiOrientation.yaw();
            orientation.pitch = osiOrientation.pitch();
            orientation.roll = osiOrientation.roll();

            return orientation;
        }

        Primitive::AbsPosition StationaryObject::GetReferencePointPosition() const {
            const osi3::Vector3d osiPosition = osiObject->base().position();
            Primitive::AbsPosition position;

            position.x = osiPosition.x();
            position.y = osiPosition.y();
            position.z = osiPosition.z();

            return position;
        }

        double StationaryObject::GetAbsVelocityDouble() const {
            return 0.0;
        }

        double StationaryObject::GetAbsAccelerationDouble() const {
            return 0.0;
        }

        const RoadIntervals &StationaryObject::GetTouchedRoads() const {
            return *touchedRoads;
        }

        Id StationaryObject::GetId() const {
            return osiObject->id().value();
        }

        void StationaryObject::SetReferencePointPosition(const Primitive::AbsPosition &newPosition) {
            osi3::Vector3d *osiPosition = osiObject->mutable_base()->mutable_position();

            osiPosition->set_x(newPosition.x);
            osiPosition->set_y(newPosition.y);
            osiPosition->set_z(newPosition.z);
        }

        void StationaryObject::SetDimension(const Primitive::Dimension &newDimension) {
            osi3::Dimension3d *osiDimension = osiObject->mutable_base()->mutable_dimension();

            osiDimension->set_length(newDimension.length);
            osiDimension->set_width(newDimension.width);
            osiDimension->set_height(newDimension.height);
        }

        void StationaryObject::SetAbsOrientation(const Primitive::AbsOrientation &newOrientation) {
            osi3::Orientation3d *osiOrientation = osiObject->mutable_base()->mutable_orientation();

            osiOrientation->set_yaw(newOrientation.yaw);
            osiOrientation->set_pitch(newOrientation.pitch);
            osiOrientation->set_roll(newOrientation.roll);
        }

        void StationaryObject::AddLaneAssignment(const Interfaces::Lane &lane) {
            assignedLanes.push_back(&lane);
        }

        const Interfaces::Lanes &StationaryObject::GetLaneAssignments() const {
            return assignedLanes;
        }

        void StationaryObject::ClearLaneAssignments() {
            assignedLanes.clear();
        }

        void StationaryObject::SetTouchedRoads(const RoadIntervals &touchedRoads) {
            this->touchedRoads = &touchedRoads;
        }

        const RoadIntervals &MovingObject::GetTouchedRoads() const
        {
            return *touchedRoads;
        }



        InvalidLane::~InvalidLane() {
            if (osiLane) {
                delete osiLane;
            }
        }


        TrafficSign::TrafficSign(osi3::TrafficSign *osiObject) : osiSign{osiObject} {
        }

        bool TrafficSign::SetSpecification(RoadSignalInterface *signal, Position position) {
            bool mapping_succeeded = true;

            const auto baseStationary = osiSign->mutable_main_sign()->mutable_base();

            baseStationary->mutable_position()->set_x(position.xPos);
            baseStationary->mutable_position()->set_y(position.yPos);
            baseStationary->mutable_position()->set_z(signal->GetZOffset() + 0.5 * signal->GetHeight());
            baseStationary->mutable_dimension()->set_width(signal->GetWidth());
            baseStationary->mutable_dimension()->set_height(signal->GetHeight());
            baseStationary->mutable_orientation()->set_yaw(position.yawAngle);

            const auto mutableOsiClassification = osiSign->mutable_main_sign()->mutable_classification();
            const std::string odType = signal->GetType();

            auto signalsMapping = OpenDriveTypeMapper::GetSignalsMapping(signal->GetCountry());

            if(signalsMapping->trafficSignsTypeOnly.find(odType) != signalsMapping->trafficSignsTypeOnly.end())
            {
                mutableOsiClassification->set_type(signalsMapping->trafficSignsTypeOnly.at(odType));
            }
            else if(signalsMapping->trafficSignsWithValue.find(odType) != signalsMapping->trafficSignsWithValue.end())
            {
                mutableOsiClassification->set_type(signalsMapping->trafficSignsWithValue.at(odType));
                mutableOsiClassification->mutable_value()->set_value_unit(OpenDriveTypeMapper::OdToOsiTrafficSignUnit(signal->GetUnit()));
                mutableOsiClassification->mutable_value()->set_value(signal->GetValue());
            }
            else if(signalsMapping->trafficSignsWithText.find(odType) != signalsMapping->trafficSignsWithText.end())
            {
                mutableOsiClassification->set_type(signalsMapping->trafficSignsWithText.at(odType));
                mutableOsiClassification->mutable_value()->set_text(signal->GetText());
            }
            else if(signalsMapping->trafficSignsPredefinedValueAndUnit.find(odType) != signalsMapping->trafficSignsPredefinedValueAndUnit.end())
            {
                const auto &specification = signalsMapping->trafficSignsPredefinedValueAndUnit.at(odType);

                mutableOsiClassification->mutable_value()->set_value_unit(specification.first);

                const auto subType = signal->GetSubType();
                if(specification.second.find(subType) != specification.second.end())
                {
                    const auto &valueAndType = specification.second.at(subType);

                    mutableOsiClassification->mutable_value()->set_value(valueAndType.first);
                    mutableOsiClassification->set_type(valueAndType.second);
                }
                else
                {
                    mutableOsiClassification->set_type(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER);
                    mapping_succeeded = false;
                }
            }
            else if(signalsMapping->trafficSignsSubTypeDefinesValue.find(odType) != signalsMapping->trafficSignsSubTypeDefinesValue.end())
            {
                const auto &valueAndUnit = signalsMapping->trafficSignsSubTypeDefinesValue.at(odType);

                mutableOsiClassification->set_type(valueAndUnit.first);

                const double value = std::stoi(signal->GetSubType());
                mutableOsiClassification->mutable_value()->set_value(value);
                mutableOsiClassification->mutable_value()->set_value_unit(valueAndUnit.second);
            } else {
                mutableOsiClassification->set_type(
                        osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER);
                mapping_succeeded = false;
            }

            return mapping_succeeded;
        }

        bool TrafficSign::AddSupplementarySign(RoadSignalInterface *odSignal, Position position) {
            auto *supplementarySign = osiSign->add_supplementary_sign();

            const auto baseStationary = supplementarySign->mutable_base();

            baseStationary->mutable_position()->set_x(position.xPos);
            baseStationary->mutable_position()->set_y(position.yPos);
            baseStationary->mutable_position()->set_z(odSignal->GetZOffset() + 0.5 * odSignal->GetHeight());
            baseStationary->mutable_dimension()->set_width(odSignal->GetWidth());
            baseStationary->mutable_dimension()->set_height(odSignal->GetHeight());
            double yaw = position.yawAngle + odSignal->GetHOffset() + (odSignal->GetOrientation() ? 0 : M_PI);
            yaw = CommonHelper::SetAngleToValidRange(yaw);
            baseStationary->mutable_orientation()->set_yaw(yaw);

            if (odSignal->GetType() == "1004") {
                supplementarySign->mutable_classification()->set_type(
                        osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_SPACE);
                auto osiValue = supplementarySign->mutable_classification()->mutable_value()->Add();

                if (odSignal->GetSubType() == "30") {
                    osiValue->set_value(odSignal->GetValue());
                    osiValue->set_value_unit(osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER);
                    osiValue->set_text(std::to_string(odSignal->GetValue()) + " m");
                    return true;
                } else if (odSignal->GetSubType() == "31") {
                    osiValue->set_value(odSignal->GetValue());
                    osiValue->set_value_unit(osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER);
                    osiValue->set_text(std::to_string(odSignal->GetValue()) + " km");
                    return true;
                } else if (odSignal->GetSubType() == "32") {
                    osiValue->set_value(100.0);
                    osiValue->set_value_unit(osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER);
                    osiValue->set_text("STOP 100 m");
                    return true;
                } else {
                    supplementarySign->mutable_classification()->set_type(
                            osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_OTHER);
                    return false;
                }
            } else {
                supplementarySign->mutable_classification()->set_type(
                        osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_OTHER);
                return false;
            }
        }

        std::pair<double, CommonTrafficSign::Unit>
        TrafficSign::GetValueAndUnitInSI(const double osiValue, const osi3::TrafficSignValue_Unit osiUnit) const {
            if (OpenDriveTypeMapper::unitConversionMap.find(osiUnit) != OpenDriveTypeMapper::unitConversionMap.end()) {
                const auto &conversionParameters = OpenDriveTypeMapper::unitConversionMap.at(osiUnit);

                return {osiValue * conversionParameters.first, conversionParameters.second};
            } else {
                return {osiValue, CommonTrafficSign::Unit::None};
            }
        }


        CommonTrafficSign::Entity TrafficSign::GetSpecification(const double relativeDistance) const {
            CommonTrafficSign::Entity specification;

            specification.distanceToStartOfRoad = s;
            specification.relativeDistance = relativeDistance;

            const auto osiType = osiSign->main_sign().classification().type();

            if (OpenDriveTypeMapper::typeConversionMap.find(osiType) != OpenDriveTypeMapper::typeConversionMap.end()) {
                specification.type = OpenDriveTypeMapper::typeConversionMap.at(osiType);
            }

            const auto &osiMainSign = osiSign->main_sign().classification().value();
            const auto valueAndUnit = GetValueAndUnitInSI(osiMainSign.value(), osiMainSign.value_unit());

            specification.value = valueAndUnit.first;
            specification.unit = valueAndUnit.second;
            specification.text = osiSign->main_sign().classification().value().text();

            std::transform(osiSign->supplementary_sign().cbegin(),
                           osiSign->supplementary_sign().cend(),
                           std::back_inserter(specification.supplementarySigns),
                           [this, relativeDistance](const auto &osiSupplementarySign) {
                               CommonTrafficSign::Entity supplementarySignSpecification;
                               supplementarySignSpecification.distanceToStartOfRoad = s;
                               supplementarySignSpecification.relativeDistance = relativeDistance;

                               if (osiSupplementarySign.classification().type() ==
                                   osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_SPACE) {
                                   const auto &osiSupplementarySignValueStruct = osiSupplementarySign.classification().value().Get(
                                           0);
                                   const auto supplementarySignValueAndUnit = GetValueAndUnitInSI(
                                           osiSupplementarySignValueStruct.value(),
                                           osiSupplementarySignValueStruct.value_unit());
                                   supplementarySignSpecification.type = CommonTrafficSign::Type::DistanceIndication;
                                   supplementarySignSpecification.value = supplementarySignValueAndUnit.first;
                                   supplementarySignSpecification.unit = supplementarySignValueAndUnit.second;
                                   supplementarySignSpecification.text = osiSupplementarySignValueStruct.text();
                               }

                               return supplementarySignSpecification;
                           });

            return specification;
        }

        Primitive::AbsPosition TrafficSign::GetReferencePointPosition() const {
            const osi3::Vector3d osiPosition = osiSign->main_sign().base().position();
            Primitive::AbsPosition position;

            position.x = osiPosition.x();
            position.y = osiPosition.y();
            position.z = osiPosition.z();

            return position;
        }

        Primitive::Dimension TrafficSign::GetDimension() const {
            return GetDimensionFromOsiObject(&osiSign->main_sign());
        }

        bool TrafficSign::IsValidForLane(OWL::Id laneId) const {
            auto assignedLanes = osiSign->main_sign().classification().assigned_lane_id();
            for (auto lane: assignedLanes) {
                if (lane.value() == laneId) {
                    return true;
                }
            }
            return false;
        }

        void TrafficSign::CopyToGroundTruth(osi3::GroundTruth &target) const {
            auto newTrafficSign = target.add_traffic_sign();
            newTrafficSign->CopyFrom(*osiSign);
        }

        RoadMarking::RoadMarking(osi3::RoadMarking *osiObject) : osiSign{osiObject} {
        }

        bool RoadMarking::SetSpecification(RoadSignalInterface *signal, Position position) {
            bool mapping_succeeded = true;

            const auto baseStationary = osiSign->mutable_base();

            baseStationary->mutable_position()->set_x(position.xPos);
            baseStationary->mutable_position()->set_y(position.yPos);
            baseStationary->mutable_position()->set_z(0.0);
            baseStationary->mutable_dimension()->set_width(signal->GetWidth());
            baseStationary->mutable_dimension()->set_length(0.5);
            baseStationary->mutable_orientation()->set_yaw(position.yawAngle);

            const auto mutableOsiClassification = osiSign->mutable_classification();
            const std::string odType = signal->GetType();

    auto signalsMapping = OpenDriveTypeMapper::GetSignalsMapping(signal->GetCountry());

    mutableOsiClassification->set_type(osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_SYMBOLIC_TRAFFIC_SIGN);
    mutableOsiClassification->set_monochrome_color(osi3::RoadMarking_Classification_Color::RoadMarking_Classification_Color_COLOR_WHITE);
    if(signalsMapping->roadMarkings.find(odType) != signalsMapping->roadMarkings.end())
    {
        mutableOsiClassification->set_traffic_main_sign_type(signalsMapping->roadMarkings.at(odType));
    }
    else
    {
        mutableOsiClassification->set_traffic_main_sign_type(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER);
        mapping_succeeded = false;
    }

    return mapping_succeeded;
}

        bool RoadMarking::SetSpecification(RoadObjectInterface *object, Position position) {
            bool mapping_succeeded = true;

            const auto baseStationary = osiSign->mutable_base();

            baseStationary->mutable_position()->set_x(position.xPos);
            baseStationary->mutable_position()->set_y(position.yPos);
            baseStationary->mutable_position()->set_z(0.0);
            baseStationary->mutable_dimension()->set_width(object->GetWidth());
            baseStationary->mutable_dimension()->set_length(object->GetLength());
            double yaw = object->GetHdg();
            yaw = CommonHelper::SetAngleToValidRange(yaw);
            baseStationary->mutable_orientation()->set_yaw(yaw);

            const auto mutableOsiClassification = osiSign->mutable_classification();

            mutableOsiClassification->set_type(
                    osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_SYMBOLIC_TRAFFIC_SIGN);
            mutableOsiClassification->set_monochrome_color(
                    osi3::RoadMarking_Classification_Color::RoadMarking_Classification_Color_COLOR_WHITE);
            if (object->GetType() == RoadObjectType::crosswalk) {
                mutableOsiClassification->set_traffic_main_sign_type(
                        osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ZEBRA_CROSSING);
            } else {
                mutableOsiClassification->set_traffic_main_sign_type(
                        osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER);
                mapping_succeeded = false;
            }

            return mapping_succeeded;
        }

        CommonTrafficSign::Entity RoadMarking::GetSpecification(const double relativeDistance) const {
            CommonTrafficSign::Entity specification;

            specification.distanceToStartOfRoad = s;
            specification.relativeDistance = relativeDistance;

            const auto osiType = osiSign->classification().traffic_main_sign_type();

            if (OpenDriveTypeMapper::typeConversionMap.find(osiType) != OpenDriveTypeMapper::typeConversionMap.end()) {
                specification.type = OpenDriveTypeMapper::typeConversionMap.at(osiType);
            }

            specification.text = osiSign->classification().value().text();

            return specification;
        }

        Primitive::AbsPosition RoadMarking::GetReferencePointPosition() const {
            const osi3::Vector3d osiPosition = osiSign->base().position();
            Primitive::AbsPosition position;

            position.x = osiPosition.x();
            position.y = osiPosition.y();
            position.z = osiPosition.z();

            return position;
        }

        Primitive::Dimension RoadMarking::GetDimension() const {
            return GetDimensionFromOsiObject(osiSign);
        }

        bool RoadMarking::IsValidForLane(OWL::Id laneId) const {
            auto assignedLanes = osiSign->classification().assigned_lane_id();
            for (auto lane: assignedLanes) {
                if (lane.value() == laneId) {
                    return true;
                }
            }
            return false;
        }

        void RoadMarking::CopyToGroundTruth(osi3::GroundTruth &target) const {
            auto newRoadMarking = target.add_road_marking();
            newRoadMarking->CopyFrom(*osiSign);
        }

        LaneBoundary::LaneBoundary(osi3::LaneBoundary *osiLaneBoundary, double width, double sStart, double sEnd,
                                   LaneMarkingSide side) :
                osiLaneBoundary(osiLaneBoundary),
                sStart(sStart),
                sEnd(sEnd),
                width(width),
                side(side) {
        }

        Id LaneBoundary::GetId() const {
            return osiLaneBoundary->id().value();
        }

        double LaneBoundary::GetWidth() const {
            return width;
        }

        double LaneBoundary::GetSStart() const {
            return sStart;
        }

        double LaneBoundary::GetSEnd() const {
            return sEnd;
        }

        LaneMarking::Type LaneBoundary::GetType() const {
            return OpenDriveTypeMapper::OsiToOdLaneMarkingType(osiLaneBoundary->classification().type());
        }

        LaneMarking::Color LaneBoundary::GetColor() const {
            return OpenDriveTypeMapper::OsiToOdLaneMarkingColor(osiLaneBoundary->classification().color());
        }

        LaneMarkingSide LaneBoundary::GetSide() const {
            return side;
        }

        void LaneBoundary::AddBoundaryPoint(const Common::Vector2d &point, double heading) {
            constexpr double doubleLineDistance = 0.15;
            auto boundaryPoint = osiLaneBoundary->add_boundary_line();
            switch (side) {
                case LaneMarkingSide::Single :
                    boundaryPoint->mutable_position()->set_x(point.x);
                    boundaryPoint->mutable_position()->set_y(point.y);
                    break;
                case LaneMarkingSide::Left :
                    boundaryPoint->mutable_position()->set_x(point.x - doubleLineDistance * std::sin(heading));
                    boundaryPoint->mutable_position()->set_y(point.y + doubleLineDistance * std::cos(heading));
                    break;
                case LaneMarkingSide::Right :
                    boundaryPoint->mutable_position()->set_x(point.x + doubleLineDistance * std::sin(heading));
                    boundaryPoint->mutable_position()->set_y(point.y - doubleLineDistance * std::cos(heading));
                    break;
            }
            boundaryPoint->set_width(width);
        }

        void LaneBoundary::CopyToGroundTruth(osi3::GroundTruth &target) const {
            auto newLaneBoundary = target.add_lane_boundary();
            newLaneBoundary->CopyFrom(*osiLaneBoundary);
        }

        Lane::LaneAssignmentCollector::LaneAssignmentCollector() noexcept {
            downstreamOrderAssignments.reserve(INITIAL_COLLECTION_SIZE);
            upstreamOrderAssignments.reserve(INITIAL_COLLECTION_SIZE);
        }

        void Lane::LaneAssignmentCollector::Insert(const LaneOverlap &laneOverlap,
                                                   const OWL::Interfaces::WorldObject *object) {
            downstreamOrderAssignments.emplace_back(laneOverlap, object);
            upstreamOrderAssignments.emplace_back(laneOverlap, object);
            dirty = true;
        }

        void Lane::LaneAssignmentCollector::Clear() {
            downstreamOrderAssignments.clear();
            upstreamOrderAssignments.clear();
            dirty = false;
        }

        void Lane::LaneAssignmentCollector::Sort() const {
            std::sort(downstreamOrderAssignments.begin(), downstreamOrderAssignments.end(),
                      [](const auto &lhs, const auto &rhs) {
        return lhs.first.sMin.roadPosition.s < rhs.first.sMin.roadPosition.s ||
               (lhs.first.sMin.roadPosition.s == rhs.first.sMin.roadPosition.s && lhs.first.sMax.roadPosition.s < rhs.first.sMax.roadPosition.s);
                      });

            std::sort(upstreamOrderAssignments.begin(), upstreamOrderAssignments.end(),
                      [](const auto &lhs, const auto &rhs) {
        return lhs.first.sMax.roadPosition.s > rhs.first.sMax.roadPosition.s ||
               (lhs.first.sMax.roadPosition.s == rhs.first.sMax.roadPosition.s && lhs.first.sMin.roadPosition.s > rhs.first.sMin.roadPosition.s);
                      });

            dirty = false;
        }

        const Interfaces::LaneAssignments &Lane::LaneAssignmentCollector::Get(bool downstream) const {
            if (dirty) {
                Sort();
            }

            if (downstream) {
                return downstreamOrderAssignments;
            } else {
                return upstreamOrderAssignments;
            }
        }


// namespace Implementation

    }

} // namespace OWL
