/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "include/roadInterface/roadInterface.h"
#include "include/roadInterface/roadSignalInterface.h"

//-----------------------------------------------------------------------------
//! Class representing a road signal
//-----------------------------------------------------------------------------
class RoadSignal : public RoadSignalInterface
{
public:
    RoadSignal(RoadInterface *road, const RoadSignalSpecification signal) :
        road{road},
        signal{signal}
    {}

    std::string GetCountry() const override;
    std::string GetType() const override;
    std::string GetSubType() const override;
    virtual std::string GetId() const override;
    double GetValue() const override;
    RoadSignalUnit GetUnit() const override;
    std::string GetText() const override;
    double GetS() const override;
    double GetT() const override;

    //-----------------------------------------------------------------------------
    //! Returns the road from which this section is a part of.
    //!
    //! @return                         Road from which this section is a part of
    //-----------------------------------------------------------------------------
    RoadInterface *GetRoad()
    {
        return road;
    }

    bool IsValidForLane(int laneId) const override;
    virtual double GetHeight() const override;
    virtual double GetWidth() const override;
    virtual double GetPitch() const;
    virtual double GetRoll() const;
    virtual bool GetIsDynamic() const override;

    virtual std::vector<std::string> GetDependencies() const override;

    virtual double GetZOffset() const override;

    virtual bool GetOrientation() const override;

    virtual double GetHOffset() const override;

private:
    RoadInterface* road;
    const RoadSignalSpecification signal;

public:
    RoadSignal(const RoadSignal&) = delete;
    RoadSignal(RoadSignal&&) = delete;
    RoadSignal& operator=(const RoadSignal&) = delete;
    RoadSignal& operator=(RoadSignal&&) = delete;
    virtual ~RoadSignal() override = default;
};
