/********************************************************************************
 * Copyright (c) 2018-2019 AMFD GmbH
 *               2016-2017 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  lateralSignal.h
//! @brief This file contains all functions for class
//! LateralSignal
//!
//! This class contains all functionality of the signal.
//-----------------------------------------------------------------------------
#pragma once

#include "include/modelInterface.h"

class LateralSignal: public ComponentStateSignalInterface
{
public:
    const std::string COMPONENTNAME = "LateralSignal";

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    LateralSignal()
    {
        componentState = ComponentState::Disabled;
    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    LateralSignal(
            ComponentState componentState,
            double laneWidth,
            double lateralDeviation,
            double gainLateralDeviation,
            double headingError,
            double gainHeadingError,
            double kappaManoeuvre,
            double kappaRoad,
            std::vector <double> curvatureOfSegmentsToNearPoint,
            std::vector <double> curvatureOfSegmentsToFarPoint
            ):
        laneWidth(laneWidth),
        lateralDeviation(lateralDeviation),
        gainLateralDeviation(gainLateralDeviation),
        headingError(headingError),
        gainHeadingError(gainHeadingError),
        kappaManoeuvre(kappaManoeuvre),
        kappaRoad(kappaRoad),
        curvatureOfSegmentsToNearPoint(curvatureOfSegmentsToNearPoint),
        curvatureOfSegmentsToFarPoint(curvatureOfSegmentsToFarPoint)
    {
        this->componentState = componentState;
    }

    virtual ~LateralSignal()
    {}

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return                       Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME;
        return stream.str();
    }

    double laneWidth {0.};
    double lateralDeviation {0.};
    double gainLateralDeviation {20.0};
    double headingError {0.};
    double gainHeadingError {7.5};
    double kappaManoeuvre {0.};
    double kappaRoad {0.};
    std::vector <double> curvatureOfSegmentsToNearPoint {0.};
    std::vector <double> curvatureOfSegmentsToFarPoint {0.};

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    LateralSignal(LateralSignal &other) :
        LateralSignal(
            other.componentState,
            other.laneWidth,
            other.lateralDeviation,
            other.gainLateralDeviation,
            other.headingError,
            other.gainHeadingError,
            other.kappaManoeuvre
            )
    {
    }

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    LateralSignal(
            ComponentState componentState,
            double laneWidth,
            double lateralDeviation,
            double gainLateralDeviation,
            double headingError,
            double gainHeadingError,
            double kappaManoeuvre
            ):
        laneWidth(laneWidth),
        lateralDeviation(lateralDeviation),
        gainLateralDeviation(gainLateralDeviation),
        headingError(headingError),
        gainHeadingError(gainHeadingError),
        kappaManoeuvre(kappaManoeuvre)
    {
        this->componentState = componentState;
    }

    LateralSignal& operator=(const LateralSignal& other)
    {
        componentState = other.componentState;
        laneWidth = other.laneWidth;
        lateralDeviation = other.lateralDeviation;
        gainLateralDeviation = other.gainLateralDeviation;
        headingError = other.headingError;
        gainHeadingError = other.gainHeadingError;
        kappaManoeuvre = other.kappaManoeuvre;

        return *this;
    }
};
