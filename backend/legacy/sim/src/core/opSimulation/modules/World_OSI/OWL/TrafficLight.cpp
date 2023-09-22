/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficLight.h"

#include <utility>
#include "OWL/OsiDefaultValues.h"


using namespace OWL::Implementation;


TrafficLightBase::TrafficLightBase(const CallbackInterface* callbacks, std::string id) :
    callbacks(callbacks),
    id(id)
{
}

void TrafficLightBase::initDefaultTrafficLight(osi3::TrafficLight *pTrafficLight)
{
    if(pTrafficLight == nullptr)
    {
        LOGWARN("Could not initialize osi traffic light, because the object is null");
        return;
    }

    OsiDefaultValues osiDefaultValues;
    pTrafficLight->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OTHER);
    pTrafficLight->mutable_base()->mutable_position()->CopyFrom(osiDefaultValues.d_3d);
    pTrafficLight->mutable_base()->mutable_orientation()->CopyFrom(osiDefaultValues.d_orientation);
    pTrafficLight->mutable_base()->mutable_dimension()->CopyFrom(osiDefaultValues.d_dimension);
    pTrafficLight->mutable_base()->mutable_base_polygon()->Clear();
    pTrafficLight->mutable_classification()->set_is_out_of_service(false);
    pTrafficLight->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_OTHER);
    pTrafficLight->mutable_classification()->set_icon(osi3::TrafficLight_Classification_Icon_ICON_OTHER);
    pTrafficLight->mutable_id()->set_value(osiDefaultValues.d_identifier.value());
}

void TrafficLightBase::SetModeIfColorIsMatchingOsi(osi3::TrafficLight *osiTrafficLightObject,
                                                   osi3::TrafficLight::Classification::Color color,
                                                   osi3::TrafficLight_Classification_Mode mode)
{
    if(osiTrafficLightObject != nullptr)
    {
        if (osiTrafficLightObject->classification().color() == color)
            osiTrafficLightObject->mutable_classification()->set_mode(mode);
    }
    else
    {
        LOGWARN("Could not set color of the osi object, because the osi traffic light object is null");
    }
}

CommonTrafficLight::State TrafficLightBase::GetStateOsi(const osi3::TrafficLight *osiTrafficLightObject) const
{
    if (osiTrafficLightObject->classification().mode() == osi3::TrafficLight_Classification_Mode_MODE_OFF)
        return CommonTrafficLight::State::Off;
    if (osiTrafficLightObject->classification().mode() == osi3::TrafficLight_Classification_Mode_MODE_FLASHING &&
        osiTrafficLightObject->classification().color() == osi3::TrafficLight_Classification_Color_COLOR_YELLOW)
        return CommonTrafficLight::State::YellowFlashing;
    if (osiTrafficLightObject->classification().mode() == osi3::TrafficLight_Classification_Mode_MODE_CONSTANT &&
        osiTrafficLightObject->classification().color() == osi3::TrafficLight_Classification_Color_COLOR_YELLOW)
        return CommonTrafficLight::State::Yellow;
    if (osiTrafficLightObject->classification().mode() == osi3::TrafficLight_Classification_Mode_MODE_CONSTANT &&
        osiTrafficLightObject->classification().color() == osi3::TrafficLight_Classification_Color_COLOR_RED)
        return CommonTrafficLight::State::Red;
    if (osiTrafficLightObject->classification().mode() == osi3::TrafficLight_Classification_Mode_MODE_CONSTANT &&
        osiTrafficLightObject->classification().color() == osi3::TrafficLight_Classification_Color_COLOR_GREEN)
        return CommonTrafficLight::State::Green;

    LOGWARN("traffic light object with Unknown state: " + osiTrafficLightObject->classification().GetDescriptor()->DebugString());
    return CommonTrafficLight::State::Unknown;
}


std::vector<osi3::TrafficLight::Classification::Color>
TrafficLightBase::fetchColorsFromSignal(RoadSignalInterface *signal, const std::map<std::string, std::map<std::string, std::vector<osi3::TrafficLight_Classification_Color>>>& colorMap) {

    auto colorTypeSet = helper::map::query(colorMap, signal->GetType());
    if (!colorTypeSet.has_value())
    {
        LOGERROR("Could not get colorTypeSet for trafficlight signal \"" + signal->GetType());
        return {osi3::TrafficLight_Classification_Color_COLOR_UNKNOWN};
    }

    auto colors = helper::map::query(colorTypeSet.value(), signal->GetSubType());

    if (!colors.has_value())
    {
        LOGERROR("Could not get color spec for trafficlight signal \"" + signal->GetType() + "and subtype " + signal->GetSubType());
        return {osi3::TrafficLight_Classification_Color_COLOR_UNKNOWN};
    }

    return colors.value();
}

osi3::TrafficLight_Classification_Icon TrafficLightBase::fetchIconsFromSignal(RoadSignalInterface *signal,

                                                                              const std::map<std::string, std::map<std::string, osi3::TrafficLight_Classification_Icon>> &iconMap) {
    auto type = helper::map::query(iconMap, signal->GetType());

    if (!type.has_value()) {
        LOGERROR("Could not get Icons for trafficlight signal \"" + signal->GetType());
        return osi3::TrafficLight_Classification_Icon_ICON_UNKNOWN;
    }

    auto icons = helper::map::query(type.value(), signal->GetSubType());
    if (!icons.has_value()) {
        LOGERROR("Could not get icon spec for trafficlight signal \"" + signal->GetType() + "and subtype " + signal->GetSubType());
        return osi3::TrafficLight_Classification_Icon_ICON_UNKNOWN;
    }

    return osi3::TrafficLight_Classification_Icon{icons.value()};
}

void TrafficLightBase::SetBaseOfOsiObject(const RoadSignalInterface *signal, const Position &position,
                                          osi3::TrafficLight *osiTrafficLightObject, int numberOfSignals) {
    double yaw = position.yawAngle + signal->GetHOffset() + (signal->GetOrientation() ? 0 : M_PI);
    yaw = CommonHelper::SetAngleToValidRange(yaw);

    if(numberOfSignals <= 1) numberOfSignals = 1;

    osiTrafficLightObject->mutable_base()->mutable_position()->set_x(position.xPos);
    osiTrafficLightObject->mutable_base()->mutable_position()->set_y(position.yPos);
    osiTrafficLightObject->mutable_base()->mutable_position()->set_z(signal->GetZOffset() + 0.5 * signal->GetHeight());
    osiTrafficLightObject->mutable_base()->mutable_dimension()->set_width(signal->GetWidth());
    osiTrafficLightObject->mutable_base()->mutable_dimension()->set_height(signal->GetHeight() / static_cast<float>(numberOfSignals));
    osiTrafficLightObject->mutable_base()->mutable_dimension()->set_length(
        signal->GetWidth()); //OpenDrive 1.6 only supports height und width, so we make a cube here
    osiTrafficLightObject->mutable_base()->mutable_orientation()->set_yaw(yaw);

}

void TrafficLightBase::Log(CbkLogLevel logLevel, const char *file, int line, const std::string &message) const
{
    if(callbacks)
    {
        callbacks->Log(logLevel,
                       file,
                       line,
                       message);
    }
}

ThreeSignalsTrafficLight::ThreeSignalsTrafficLight(const std::string &opendrive_id, osi3::TrafficLight *osiLightRed,
                                                   osi3::TrafficLight *osiLightYellow,
                                                   osi3::TrafficLight *osiLightGreen,
                                                   const CallbackInterface* callbacks) :
        osiLightRed(osiLightRed),
        osiLightYellow(osiLightYellow),
        osiLightGreen(osiLightGreen),
        TrafficLightBase(callbacks, opendrive_id)
{
    //TODO remove these lines, or rename class accordingly to red, yellow, green
    osiLightRed->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_RED);
    osiLightYellow->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_YELLOW);
    osiLightGreen->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_GREEN);
}

void ThreeSignalsTrafficLight::initDefaultTrafficLight()
{
    initDefaultTrafficLight(osiLightRed);
    initDefaultTrafficLight(osiLightYellow);
    initDefaultTrafficLight(osiLightGreen);

    //TODO remove these lines, or rename class accordingly to red, yellow, green
    osiLightRed->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_RED);
    osiLightYellow->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_YELLOW);
    osiLightGreen->mutable_classification()->set_color(osi3::TrafficLight_Classification_Color_COLOR_GREEN);
}

bool ThreeSignalsTrafficLight::IsValidForLane(Id laneId) const {
    auto assignedLanes = osiLightRed->classification().assigned_lane_id();
    for (const auto lane: assignedLanes) {
        if (lane.value() == laneId) {
            return true;
        }
    }
    return false;
}

bool ThreeSignalsTrafficLight::SetSpecification(RoadSignalInterface *signal, const Position &position) {
    bool success =
            SetSpecificationOnOsiObject(signal, position, osiLightRed) &&
            SetSpecificationOnOsiObject(signal, position, osiLightYellow) &&
            SetSpecificationOnOsiObject(signal, position, osiLightGreen);
    osiLightGreen->mutable_base()->mutable_position()->set_z( osiLightYellow->base().position().z() -
                                                                       osiLightYellow->base().dimension().height());
    osiLightRed->mutable_base()->mutable_position()->set_z( osiLightYellow->base().position().z() +
                                                                    osiLightYellow->base().dimension().height());
    return success;
}

OWL::Primitive::AbsPosition ThreeSignalsTrafficLight::GetReferencePointPosition() const {
    const osi3::Vector3d osiPosition = osiLightGreen->base().position();
    Primitive::AbsPosition position;

    position.x = osiPosition.x();
    position.y = osiPosition.y();
    position.z = osiPosition.z();

    return position;
}

void ThreeSignalsTrafficLight::CopyToGroundTruth(osi3::GroundTruth &target) const {
    auto newLightRed = target.add_traffic_light();
    newLightRed->CopyFrom(*osiLightRed);
    auto newLightYellow = target.add_traffic_light();
    newLightYellow->CopyFrom(*osiLightYellow);
    auto newLightGreen = target.add_traffic_light();
    newLightGreen->CopyFrom(*osiLightGreen);
}

OWL::Primitive::Dimension ThreeSignalsTrafficLight::GetDimension() const {
    return OWL::GetDimensionFromOsiObject(osiLightRed);
}

void ThreeSignalsTrafficLight::SetState(CommonTrafficLight::State newState) {
    switch (newState) {
        case CommonTrafficLight::State::Off :
            osiLightRed->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            osiLightYellow->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            osiLightGreen->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            break;
        case CommonTrafficLight::State::Red :
            osiLightRed->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            osiLightYellow->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            osiLightGreen->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            break;
        case CommonTrafficLight::State::Yellow :
            osiLightRed->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            osiLightYellow->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            osiLightGreen->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            break;
        case CommonTrafficLight::State::Green :
            osiLightRed->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            osiLightYellow->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            osiLightGreen->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            break;
        case CommonTrafficLight::State::RedYellow :
            osiLightRed->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            osiLightYellow->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            osiLightGreen->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            break;
        case CommonTrafficLight::State::YellowFlashing :
            osiLightRed->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            osiLightYellow->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_FLASHING);
            osiLightGreen->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            break;
        case CommonTrafficLight::State::Unknown:
            LOGWARN("Could not set the traffic light state, because the state is unknown.");
            break;
        default:
            LOGWARN("Could not set the traffic light state, because the state is undefined.");
            break;
    }
}

CommonTrafficLight::Entity ThreeSignalsTrafficLight::GetSpecification(const double relativeDistance) const {
    CommonTrafficLight::Entity specification;

    specification.relativeDistance = relativeDistance;

    const auto osiType = osiLightRed->classification().icon();

    auto type = helper::map::query(OpenDriveTypeMapper::trafficLightTypeThreeLightsConversionMap,
                                   osiType);
    specification.type = type.has_value() ? type.value() : CommonTrafficLight::Type::Undefined;

    specification.state = GetState();

    return specification;
}

CommonTrafficLight::State ThreeSignalsTrafficLight::GetState() const
{
    const CommonTrafficLight::State redLightState = GetStateOsi(osiLightRed);
    const CommonTrafficLight::State yellowLightState = GetStateOsi(osiLightYellow) ;
    const CommonTrafficLight::State greenLightState = GetStateOsi(osiLightGreen);

    const CommonTrafficLight::State offState = CommonTrafficLight::State::Off;
    const CommonTrafficLight::State unknownState = CommonTrafficLight::State::Unknown;
    const CommonTrafficLight::State redState = CommonTrafficLight::State::Red;
    const CommonTrafficLight::State yellowState = CommonTrafficLight::State::Yellow;
    const CommonTrafficLight::State greenState = CommonTrafficLight::State::Green;

    const bool trafficLightIsInOffState = redLightState == offState && yellowLightState == offState && greenLightState == offState;
    const bool trafficLightIsInUnknownState = redLightState == unknownState && yellowLightState == unknownState && greenLightState == unknownState;
    const bool trafficLightIsInRedState = redLightState == redState && yellowLightState == offState && greenLightState == offState;
    const bool trafficLightIsInYellowState = redLightState == offState && yellowLightState == yellowState && greenLightState == offState;
    const bool trafficLightIsInGreenState = redLightState == offState && yellowLightState == offState && greenLightState == greenState;
    const bool trafficLightIsInRedYellowState = redLightState == redState && yellowLightState == yellowState && greenLightState == offState;
    const bool trafficLightIsInYellowFlashingState = yellowLightState == CommonTrafficLight::State::YellowFlashing;

    if (trafficLightIsInOffState)
        return CommonTrafficLight::State::Off;
    else if (trafficLightIsInUnknownState)
        return CommonTrafficLight::State::Unknown;
    else if (trafficLightIsInRedState)
        return CommonTrafficLight::State::Red;
    else if (trafficLightIsInYellowState)
        return CommonTrafficLight::State::Yellow;
    else if (trafficLightIsInGreenState)
        return CommonTrafficLight::State::Green;
    else if (trafficLightIsInRedYellowState)
        return CommonTrafficLight::State::RedYellow;
    else if (trafficLightIsInYellowFlashingState)
        return CommonTrafficLight::State::YellowFlashing;

    LOGWARN("Illegal traffic light state");
    return CommonTrafficLight::State::Unknown;
}

bool OWL::Implementation::ThreeSignalsTrafficLight::SetSpecificationOnOsiObject(RoadSignalInterface *signal,
                                                                                const Position &position,
                                                                                osi3::TrafficLight *osiTrafficLightObject) {

   SetBaseOfOsiObject(signal,position,osiTrafficLightObject, 3);
   auto icon = fetchIconsFromSignal(signal, OpenDriveTypeMapper::ThreeSignalsTrafficLightsIcons);
   osiTrafficLightObject->mutable_classification()->set_icon(icon);
   return true;
}

TwoSignalsTrafficLight::TwoSignalsTrafficLight(const std::string &opendrive_id, osi3::TrafficLight *osiLightTop,
                                               osi3::TrafficLight *osiLightBottom,
                                               const CallbackInterface* callbacks) :
        osiLightTop(osiLightTop),
        osiLightBottom(osiLightBottom),
        TrafficLightBase(callbacks, opendrive_id)
{
}

void TwoSignalsTrafficLight::initDefaultTrafficLight()
{
    initDefaultTrafficLight(osiLightTop);
    initDefaultTrafficLight(osiLightBottom);
}

bool TwoSignalsTrafficLight::IsValidForLane(Id laneId) const {
    auto assignedLanes = osiLightTop->classification().assigned_lane_id();
    for (const auto lane: assignedLanes) {
        if (lane.value() == laneId) {
            return true;
        }
    }
    return false;
}

bool TwoSignalsTrafficLight::SetSpecification(RoadSignalInterface *signal, const Position &position) {
    bool success =
            SetSpecificationOnOsiObject(signal, position, osiLightTop, true) &&
            SetSpecificationOnOsiObject(signal, position, osiLightBottom, false);
    double c = osiLightTop->base().position().z();
    osiLightTop->mutable_base()->mutable_position()->set_z( c +
    osiLightTop->base().dimension().height() / 2.0);
    double d = osiLightBottom->base().position().z();
    osiLightBottom->mutable_base()->mutable_position()->set_z( d -
    osiLightBottom->base().dimension().height() / 2.0);
  return success;
}

OWL::Primitive::AbsPosition TwoSignalsTrafficLight::GetReferencePointPosition() const {
    const osi3::Vector3d osiPosition = osiLightBottom->base().position();
    Primitive::AbsPosition position{};

    position.x = osiPosition.x();
    position.y = osiPosition.y();
    position.z = osiPosition.z();

    return position;
}

void TwoSignalsTrafficLight::CopyToGroundTruth(osi3::GroundTruth &target) const {
    auto newLightTop = target.add_traffic_light();
    newLightTop->CopyFrom(*osiLightTop);
    auto newLightBottom = target.add_traffic_light();
    newLightBottom->CopyFrom(*osiLightBottom);
}

OWL::Primitive::Dimension TwoSignalsTrafficLight::GetDimension() const {
    return OWL::GetDimensionFromOsiObject(osiLightTop);
}

void TwoSignalsTrafficLight::SetState(CommonTrafficLight::State newState)
{
    if (osiLightTop != nullptr)
        osiLightTop->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
    if (osiLightBottom != nullptr)
        osiLightBottom->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);

    switch (newState) {
        case CommonTrafficLight::State::Off :
            break;
        case CommonTrafficLight::State::Red :
            SetTopModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_RED,
                                        osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            SetBottomModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_RED,
                                           osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            break;
        case CommonTrafficLight::State::Yellow :
            SetTopModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_YELLOW,
                                        osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            SetBottomModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_YELLOW,
                                           osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            break;
        case CommonTrafficLight::State::Green :
            SetTopModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_GREEN,
                                        osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            SetBottomModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_GREEN,
                                           osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            break;
        case CommonTrafficLight::State::RedYellow :
            SetTopModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_RED,
                                        osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            SetBottomModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_RED,
                                           osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            SetTopModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_YELLOW,
                                        osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            SetBottomModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_YELLOW,
                                           osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            break;
        case CommonTrafficLight::State::YellowFlashing :
            SetTopModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_YELLOW,
                                        osi3::TrafficLight_Classification_Mode_MODE_FLASHING);
            SetBottomModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_YELLOW,
                                           osi3::TrafficLight_Classification_Mode_MODE_FLASHING);
            break;
        case CommonTrafficLight::State::Unknown:
            LOGWARN("Could not set the traffic light state, because the state is unknown.");
            break;
        default:
            LOGWARN("Could not set the traffic light state, because the state is undefined.");
            break;
    }
}

CommonTrafficLight::Entity TwoSignalsTrafficLight::GetSpecification(const double relativeDistance) const {
    CommonTrafficLight::Entity specification;

    specification.relativeDistance = relativeDistance;

    const auto osiType = osiLightTop->classification().icon();

    auto type = helper::map::query(OpenDriveTypeMapper::trafficLightTypeTwoLightsConversionMap,
                                   osiType);
    specification.type = type.has_value() ? type.value() : CommonTrafficLight::Type::Undefined;

    specification.state = GetState();

    return specification;
}

CommonTrafficLight::State TwoSignalsTrafficLight::GetState() const
{
    CommonTrafficLight::State topLightState = GetTopState();
    CommonTrafficLight::State bottomLightState = GetBottomState();

    const CommonTrafficLight::State offState = CommonTrafficLight::State::Off;
    const CommonTrafficLight::State unknownState = CommonTrafficLight::State::Unknown;
    const CommonTrafficLight::State redState = CommonTrafficLight::State::Red;
    const CommonTrafficLight::State yellowState = CommonTrafficLight::State::Yellow;
    const CommonTrafficLight::State greenState = CommonTrafficLight::State::Green;
    const CommonTrafficLight::State yellowFlashingState = CommonTrafficLight::State::YellowFlashing;

    const bool trafficLightIsInOffState = topLightState == offState && bottomLightState == offState;
    const bool trafficLightIsInUnknownState = topLightState == unknownState && bottomLightState == unknownState;
    const bool trafficLightIsInRedState = (topLightState == redState && bottomLightState == offState) || (topLightState == offState && bottomLightState == redState);
    const bool trafficLightIsInYellowState = (topLightState == yellowState && bottomLightState == offState) || (topLightState == offState && bottomLightState == yellowState);
    const bool trafficLightIsInGreenState = (topLightState == greenState && bottomLightState == offState) || (topLightState == offState && bottomLightState == greenState);
    const bool trafficLightIsInRedYellowState = (topLightState == redState && bottomLightState == yellowState) || (topLightState == yellowState && bottomLightState == redState);
    const bool trafficLightIsInYellowFlashingState = topLightState == yellowFlashingState && bottomLightState == yellowFlashingState;

    if (trafficLightIsInOffState)
        return CommonTrafficLight::State::Off;
    else if (trafficLightIsInUnknownState)
        return CommonTrafficLight::State::Unknown;
    else if (trafficLightIsInYellowFlashingState)
        return CommonTrafficLight::State::YellowFlashing;
    else if (trafficLightIsInRedYellowState)
        return CommonTrafficLight::State::RedYellow;
    else if (trafficLightIsInRedState)
        return CommonTrafficLight::State::Red;
    else if (trafficLightIsInYellowState)
        return CommonTrafficLight::State::Yellow;
    else if (trafficLightIsInGreenState)
        return CommonTrafficLight::State::Green;

    LOGWARN("Illegal traffic light state");
    return CommonTrafficLight::State::Unknown;
}

CommonTrafficLight::State TwoSignalsTrafficLight::GetTopState() const
{
    return GetStateOsi(osiLightTop);
}

CommonTrafficLight::State TwoSignalsTrafficLight::GetBottomState() const
{
    return GetStateOsi(osiLightBottom);
}

void TwoSignalsTrafficLight::SetTopModeIfColorIsMatching(osi3::TrafficLight::Classification::Color color,
                                                         osi3::TrafficLight_Classification_Mode mode) {
    SetModeIfColorIsMatchingOsi(osiLightTop,color,mode);
}

void TwoSignalsTrafficLight::SetBottomModeIfColorIsMatching(osi3::TrafficLight::Classification::Color color,
                                                            osi3::TrafficLight_Classification_Mode mode) {
    SetModeIfColorIsMatchingOsi(osiLightBottom,color,mode);
}

bool OWL::Implementation::TwoSignalsTrafficLight::SetSpecificationOnOsiObject
        (RoadSignalInterface *signal, const Position &position, osi3::TrafficLight *osiTrafficLightObject, bool isTop) {

    SetBaseOfOsiObject(signal, position, osiTrafficLightObject, 2);
    auto icon = fetchIconsFromSignal(signal,
                                                                     OpenDriveTypeMapper::TwoSignalsTrafficLightsIcons);
    auto colors = fetchColorsFromSignal(signal, OpenDriveTypeMapper::TwoSignalsTrafficLightsColors);
    if (colors.size() == 2) {
        int colorIndex = isTop ? 0 : 1;
        osiTrafficLightObject->mutable_classification()->set_icon(icon);
        osiTrafficLightObject->mutable_classification()->set_color(colors[colorIndex]);
        return true;
    } else {
        LOGERROR("Colors from the  \"" + signal->GetType() + "and subtype " + signal->GetSubType());
        return false;
    }
}

OneSignalsTrafficLight::OneSignalsTrafficLight(const std::string &opendrive_id, osi3::TrafficLight *osiTrafficLight,
                                               const CallbackInterface* callbacks) :
        osiTrafficLight(osiTrafficLight),
        TrafficLightBase(callbacks, opendrive_id)
{
}

void OneSignalsTrafficLight::initDefaultTrafficLight()
{
    initDefaultTrafficLight(osiTrafficLight);
}

bool OneSignalsTrafficLight::IsValidForLane(Id laneId) const {
    auto assignedLanes = osiTrafficLight->classification().assigned_lane_id();
    for (const auto lane: assignedLanes) {
        if (lane.value() == laneId) {
            return true;
        }
    }
    return false;
}

bool OneSignalsTrafficLight::SetSpecification(RoadSignalInterface *signal, const Position &position) {
    return
            SetSpecificationOnOsiObject(signal, position, osiTrafficLight);
}

OWL::Primitive::AbsPosition OneSignalsTrafficLight::GetReferencePointPosition() const {
    const osi3::Vector3d osiPosition = osiTrafficLight->base().position();
    Primitive::AbsPosition position{};

    position.x = osiPosition.x();
    position.y = osiPosition.y();
    position.z = osiPosition.z();

    return position;
}

void OneSignalsTrafficLight::CopyToGroundTruth(osi3::GroundTruth &target) const {
    auto newLight = target.add_traffic_light();
    newLight->CopyFrom(*osiTrafficLight);
}

OWL::Primitive::Dimension OneSignalsTrafficLight::GetDimension() const {
    return OWL::GetDimensionFromOsiObject(osiTrafficLight);
}

void OneSignalsTrafficLight::SetState(CommonTrafficLight::State newState) {
    osiTrafficLight->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
    switch (newState) {
        case CommonTrafficLight::State::Off :
            osiTrafficLight->mutable_classification()->set_mode(osi3::TrafficLight_Classification_Mode_MODE_OFF);
            break;
        case CommonTrafficLight::State::Red :
            SetModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_RED,
                                     osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            break;
        case CommonTrafficLight::State::Yellow :
            SetModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_YELLOW,
                                     osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            break;
        case CommonTrafficLight::State::Green :
            SetModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_GREEN,
                                     osi3::TrafficLight_Classification_Mode_MODE_CONSTANT);
            break;
        case CommonTrafficLight::State::RedYellow :
            LOGWARN("RedYellow State does not exist for one signal traffic light");
            break;
        case CommonTrafficLight::State::YellowFlashing :
            SetModeIfColorIsMatching(osi3::TrafficLight_Classification_Color_COLOR_YELLOW,
                                     osi3::TrafficLight_Classification_Mode_MODE_FLASHING);
            break;
        case CommonTrafficLight::State::Unknown:
            LOGWARN("Could not set the traffic light state, because the state is unknown.");
            break;
        default:
            LOGWARN("Could not set the traffic light state, because the state is undefined.");
            break;
    }
}

CommonTrafficLight::Entity OneSignalsTrafficLight::GetSpecification(const double relativeDistance) const {
    CommonTrafficLight::Entity specification;

    specification.relativeDistance = relativeDistance;

    const auto osiType = osiTrafficLight->classification().icon();

    auto type = helper::map::query(OpenDriveTypeMapper::trafficLightTypeOneLightConversionMap,
                                   osiType);
    specification.type = type.has_value() ? type.value() : CommonTrafficLight::Type::Undefined;

    specification.state = GetState();

    return specification;
}

CommonTrafficLight::State OneSignalsTrafficLight::GetState() const {

    return GetStateOsi(osiTrafficLight);
}

void OneSignalsTrafficLight::SetModeIfColorIsMatching(osi3::TrafficLight::Classification::Color color,
                                                      osi3::TrafficLight_Classification_Mode mode) {
    SetModeIfColorIsMatchingOsi(osiTrafficLight, color, mode);
}

bool OWL::Implementation::OneSignalsTrafficLight::SetSpecificationOnOsiObject
        (RoadSignalInterface *signal, const Position &position, osi3::TrafficLight *osiTrafficLightObject) {

    SetBaseOfOsiObject(signal, position, osiTrafficLight, 1);
    auto icon = fetchIconsFromSignal(signal,
                                                                     OpenDriveTypeMapper::OneSignalTrafficLightsIcons);
    auto colors = fetchColorsFromSignal(signal, OpenDriveTypeMapper::OneSignalTrafficLightsColors);

    if (!colors.empty()) {
        osiTrafficLightObject->mutable_classification()->set_icon(icon);
        osiTrafficLightObject->mutable_classification()->set_color(colors[0]);
        return true;
    } else {
        LOGERROR("Colors from the  \"" + signal->GetType() + "and subtype " + signal->GetSubType());
        return false;
    }
}

