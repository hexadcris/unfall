/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include <string>
#include "roadElementTypes.h"
//#include "RoadElementInterface.h"

//! Represents a RoadSignal in OpenDrive
class RoadSignalInterface// : public RoadElementInterface
{
public:
    RoadSignalInterface() = default;
    RoadSignalInterface(const RoadSignalInterface&) = delete;
    RoadSignalInterface(RoadSignalInterface&&) = delete;
    RoadSignalInterface& operator=(const RoadSignalInterface&) = delete;
    RoadSignalInterface& operator=(RoadSignalInterface&&) = delete;
    virtual ~RoadSignalInterface() = default;

    //-----------------------------------------------------------------------------
    /// @brief Returns the unique identification string of the signal
    /// @return id
    //-----------------------------------------------------------------------------
    virtual std::string GetId() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the s coordinate of the signal
    ///
    /// @return s [m]
    //-----------------------------------------------------------------------------
    virtual double GetS() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the t coordinate of the signal
    ///
    /// @return s [m]
    //-----------------------------------------------------------------------------
    virtual double GetT() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns if the signal is valid for a given lane
    //! @param[in] laneId   lane Id of interest
    //!
    //! @return             true if signal is valid
    //-----------------------------------------------------------------------------
    virtual bool IsValidForLane(int laneId) const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the country of the signal
    /// @return country
    //-----------------------------------------------------------------------------
    virtual std::string GetCountry() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the type of the signal (e.g. code according to StVO)
    /// @return type
    //-----------------------------------------------------------------------------
    virtual std::string GetType() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the subtype of the signal (e.g. code according to StVO)
    /// @return type
    //-----------------------------------------------------------------------------
    virtual std::string GetSubType() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the value
    /// @return value
    //-----------------------------------------------------------------------------
    virtual double GetValue() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the unit
    /// @return unit
    //-----------------------------------------------------------------------------
    virtual RoadSignalUnit GetUnit() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the text on the signal
    /// @return text
    //-----------------------------------------------------------------------------
    virtual std::string GetText() const = 0;
    virtual std::vector<std::string> GetDependencies() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns true if the signal is dynamic
    ///
    /// @return true if dynamic == "yes"
    //-----------------------------------------------------------------------------
    virtual bool GetIsDynamic() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the width of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetWidth() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the height of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetHeight() const = 0;

    //-----------------------------------------------------------------------------
    /// @brief Returns the pitch of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetPitch() const = 0;
    //-----------------------------------------------------------------------------
    /// @brief Returns the roll of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetRoll() const = 0;

    virtual double GetZOffset() const = 0;
    virtual bool GetOrientation() const = 0;
    virtual double GetHOffset() const = 0;
};

