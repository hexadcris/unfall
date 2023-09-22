/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef OPENPASS_TRAFFICLIGHT_H
#define OPENPASS_TRAFFICLIGHT_H

#include <utility>

#include "OWL/DataTypes.h"
#include "include/callbackInterface.h"

namespace OWL::Implementation {

class TrafficLightBase : public Interfaces::TrafficLight
{
public:
    TrafficLightBase(const CallbackInterface *callbacks, std::string id);

    virtual ~TrafficLightBase() = default;

    virtual std::string GetId() const override
    {
        return id;
    }

    virtual double GetS() const override
    {
        return s;
    }

    virtual void SetS(double sPos) override
    {
        s = sPos;
    }

    void Log(CbkLogLevel logLevel, const char *file, int line, const std::string &message) const;

    void SetModeIfColorIsMatchingOsi(osi3::TrafficLight *osiTrafficLightObject,
                                     osi3::TrafficLight::Classification::Color color,
                                     osi3::TrafficLight_Classification_Mode mode);

    void SetBaseOfOsiObject(const RoadSignalInterface *signal, const Position &position,
                            osi3::TrafficLight *osiTrafficLightObject, int numberOfSignals);

    osi3::TrafficLight_Classification_Icon fetchIconsFromSignal(RoadSignalInterface *signal,
                                                                const std::map<std::string, std::map<std::string, osi3::TrafficLight_Classification_Icon>> &iconMap);

    std::vector<osi3::TrafficLight::Classification::Color>
    fetchColorsFromSignal(RoadSignalInterface *signal, const std::map<std::string, std::map<std::string, std::vector<osi3::TrafficLight_Classification_Color>>> &colorMap);

    CommonTrafficLight::State GetStateOsi(const osi3::TrafficLight *osiTrafficLightObject) const;

    virtual void initDefaultTrafficLight() = 0;
    void initDefaultTrafficLight(osi3::TrafficLight *pTrafficLight);

protected:
    std::string id{""};
    double s{0.0};

    const CallbackInterface *callbacks;
};

/************************************************************
 * ASAM TrafficLight 1.000.001 sub ALL (red, yellow, green) &
 * ASAM TrafficLight 1.000.011 (red, yellow, green with Arrow Icons)
 ***********************************************************/
class ThreeSignalsTrafficLight : public TrafficLightBase
{
public:
    ThreeSignalsTrafficLight(const std::string &opendrive_id, osi3::TrafficLight *osiLightRed,
                             osi3::TrafficLight *osiLightYellow,
                             osi3::TrafficLight *osiLightGreen,
                             const CallbackInterface *callbacks);

    virtual ~ThreeSignalsTrafficLight() = default;

    virtual bool IsValidForLane(OWL::Id laneId) const override;

    virtual bool SetSpecification(RoadSignalInterface *signal, const Position &position) override;

    virtual Primitive::AbsPosition GetReferencePointPosition() const override;

    virtual Primitive::Dimension GetDimension() const override;

    void SetValidForLane(OWL::Id laneId) override
    {
        osiLightRed->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
        osiLightYellow->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
        osiLightGreen->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
    }

    virtual void CopyToGroundTruth(osi3::GroundTruth &target) const;

    virtual void SetState(CommonTrafficLight::State newState);

    virtual CommonTrafficLight::Entity GetSpecification(const double relativeDistance) const override;

    virtual CommonTrafficLight::State GetState() const;

    void initDefaultTrafficLight() override;

    // Is need that overloading and overwriting at the same time works
    using TrafficLightBase::initDefaultTrafficLight;

private:
    bool SetSpecificationOnOsiObject(RoadSignalInterface *signal, const Position &position,
                                     osi3::TrafficLight *osiTrafficLightObject);

    osi3::TrafficLight *osiLightRed;
    osi3::TrafficLight *osiLightYellow;
    osi3::TrafficLight *osiLightGreen;
};

/************************************************************
 * ASAM TrafficLight 1.000.002 sub -
 * ASAM TrafficLight 1.000.007 sub - (red, green with bike & pedestrian icon)
 * ASAM TrafficLight 1.000.009 sub 10,20,30 (any two conbinations of red yellow or green)
 * ASAM TrafficLight 1.000.013 sub - (red, green with bike icon)
 *
 ***********************************************************/
class TwoSignalsTrafficLight : public TrafficLightBase
{
public:
    TwoSignalsTrafficLight(const std::string &opendrive_id, osi3::TrafficLight *osiLightTop,
                           osi3::TrafficLight *osiLightBottom,
                           const CallbackInterface *callbacks);

    virtual ~TwoSignalsTrafficLight() = default;

    virtual bool IsValidForLane(OWL::Id laneId) const override;

    virtual bool SetSpecification(RoadSignalInterface *signal, const Position &position) override;

    virtual Primitive::AbsPosition GetReferencePointPosition() const override;

    virtual Primitive::Dimension GetDimension() const override;

    virtual void SetValidForLane(OWL::Id laneId) override
    {
        osiLightTop->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
        osiLightBottom->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
    }

    virtual void CopyToGroundTruth(osi3::GroundTruth &target) const;

    virtual void SetState(CommonTrafficLight::State newState);

    virtual CommonTrafficLight::Entity GetSpecification(const double relativeDistance) const override;

    virtual CommonTrafficLight::State GetState() const;

    void initDefaultTrafficLight() override;

    // Is need that overloading and overwriting at the same time works
    using TrafficLightBase::initDefaultTrafficLight;

private:
    bool SetSpecificationOnOsiObject(RoadSignalInterface *signal, const Position &position,
                                     osi3::TrafficLight *osiTrafficLightObject, bool isTop);

    void SetTopModeIfColorIsMatching(osi3::TrafficLight::Classification::Color color,
                                     osi3::TrafficLight_Classification_Mode mode);

    void SetBottomModeIfColorIsMatching(osi3::TrafficLight::Classification::Color color,
                                        osi3::TrafficLight_Classification_Mode mode);

    CommonTrafficLight::State GetTopState() const;

    CommonTrafficLight::State GetBottomState() const;

    std::string subtype;

    osi3::TrafficLight *osiLightTop{nullptr};
    osi3::TrafficLight *osiLightBottom{nullptr};
};

/************************************************************
 * ASAM TrafficLight 1.000.002 sub 10,20 (red, green with pedestrian icon)
 * ASAM TrafficLight 1.000.007 sub 10,20,30 (red, green with bike & pedestrian icon)
 * ASAM TrafficLight 1.000.013 20,30,40 (red, green with bike icon)
 * ASAM TrafficLight 1.000.020 ALL
 * ASAM TrafficLight 1.000.008 ALL
 * ASAM TrafficLight 1.000.012 ALL
 ***********************************************************/
class OneSignalsTrafficLight : public TrafficLightBase
{
public:
    OneSignalsTrafficLight(const std::string &opendrive_id, osi3::TrafficLight *osiTrafficLight,
                           const CallbackInterface *callbacks);

    virtual ~OneSignalsTrafficLight() = default;

    virtual bool IsValidForLane(OWL::Id laneId) const override;

    virtual bool SetSpecification(RoadSignalInterface *signal, const Position &position) override;

    virtual Primitive::AbsPosition GetReferencePointPosition() const override;

    virtual Primitive::Dimension GetDimension() const override;

    virtual void SetValidForLane(OWL::Id laneId) override
    {
        osiTrafficLight->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
    }

    virtual void CopyToGroundTruth(osi3::GroundTruth &target) const;

    virtual void SetState(CommonTrafficLight::State newState);

    virtual CommonTrafficLight::Entity GetSpecification(const double relativeDistance) const override;

    virtual CommonTrafficLight::State GetState() const;

    void initDefaultTrafficLight() override;

    // Is need that overloading and overwriting at the same time works
    using TrafficLightBase::initDefaultTrafficLight;

private:
    bool SetSpecificationOnOsiObject(RoadSignalInterface *signal, const Position &position,
                                     osi3::TrafficLight *osiTrafficLightObject);

    void SetModeIfColorIsMatching(osi3::TrafficLight::Classification::Color color,
                                  osi3::TrafficLight_Classification_Mode mode);
    std::string subtype;

    osi3::TrafficLight *osiTrafficLight{nullptr};
};
} // namespace OWL::Implementation
#endif // OPENPASS_TRAFFICLIGHT_H
