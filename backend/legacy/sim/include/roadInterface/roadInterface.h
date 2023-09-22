/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  RoadInterface.h
//! @brief This file contains a interface representing a road.
//-----------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>

#include "roadElementTypes.h"
#include "roadLaneSectionInterface.h"
#include "roadSignalInterface.h"
#include "roadObjectInterface.h"
#include "roadElevation.h"
#include "roadLaneInterface.h"
#include "roadLaneWidth.h"
#include "roadLaneOffset.h"
#include "roadGeometryInterface.h"
#include "roadLinkInterface.h"

//-----------------------------------------------------------------------------
//! Struct containing values for parametric cubic polynomial geometry
//! @param[in]  aU                   constant factor of the polynomial for u
//! @param[in]  bU                   linear factor of the polynomial for u
//! @param[in]  cU                   quadratic factor of the polynomial for u
//! @param[in]  dU                   cubic factor of the polynomial for u
//! @param[in]  aV                   constant factor of the polynomial for v
//! @param[in]  bV                   linear factor of the polynomial for v
//! @param[in]  cV                   quadratic factor of the polynomial for v
//! @param[in]  dV                   cubic factor of the polynomial for v
//-----------------------------------------------------------------------------
struct ParamPoly3Parameters{
    double aU;
    double bU;
    double cU;
    double dU;
    double aV;
    double bV;
    double cV;
    double dV;
};

//-----------------------------------------------------------------------------
//! Class representing a road.
//-----------------------------------------------------------------------------
class RoadInterface
{
public:
    RoadInterface() = default;
    RoadInterface(const RoadInterface&) = delete;
    RoadInterface(RoadInterface&&) = delete;
    RoadInterface& operator=(const RoadInterface&) = delete;
    RoadInterface& operator=(RoadInterface&&) = delete;
    virtual ~RoadInterface() = default;

    //-----------------------------------------------------------------------------
    //! Adds a line geometry to a road by creating a new RoadGeometryLine object and
    //! adding it to the stored list of geometries.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddGeometryLine(double s,
                         double x,
                         double y,
                         double hdg,
                         double length) = 0;

    //-----------------------------------------------------------------------------
    //! Adds an arc geometry to a road by creating a new RoadGeometryArc object and
    //! adding it to the stored list of geometries.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @param[in]  curvature           constant curvature throughout the element
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddGeometryArc(double s,
                        double x,
                        double y,
                        double hdg,
                        double length,
                        double curvature) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a spiral geometry to a road by creating a new RoadGeometrySpiral object and
    //! adding it to the stored list of geometries.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @param[in]  curvStart           curvature at the start of the element
    //! @param[in]  curvEnd             curvature at the end of the element
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddGeometrySpiral(double s,
                           double x,
                           double y,
                           double hdg,
                           double length,
                           double curvStart,
                           double curvEnd) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a cubic polynomial geometry to a road by creating a new RoadGeometryPoly3
    //! object and adding it to the stored list of geometries.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @param[in]  a                   constant factor of the polynomial
    //! @param[in]  b                   linear factor of the polynomial
    //! @param[in]  c                   quadratic factor of the polynomial
    //! @param[in]  d                   cubic factor of the polynomial
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddGeometryPoly3(double s,
                          double x,
                          double y,
                          double hdg,
                          double length,
                          double a,
                          double b,
                          double c,
                          double d) = 0;
    //-----------------------------------------------------------------------------
    //! Adds a parametric cubic polynomial geometry to a road by creating a new
    //! RoadGeometryparamPoly3 object and adding it to the stored list of geometries.
    //! Each coordinate is calculated in a local (u,v) coordinate system
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @param[in]  parameters          Factors for the polynomials describing the road
    //! @return                          false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddGeometryParamPoly3(double s,
                          double x,
                          double y,
                          double hdg,
                          double length,
                          ParamPoly3Parameters parameters) = 0;

    //-----------------------------------------------------------------------------
    //! Adds an elevation profile defined via a cubic polynomial to a road by creating
    //! a new RoadElevation object and adding it to the stored list of elevations.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  a                   constant factor of the polynomial
    //! @param[in]  b                   linear factor of the polynomial
    //! @param[in]  c                   quadratic factor of the polynomial
    //! @param[in]  d                   cubic factor of the polynomial
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddElevation(double s,
                      double a,
                      double b,
                      double c,
                      double d) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a lane offset defined via a cubic polynomial to a road by creating a new
    //! RoadLaneOffset object and adding it to the stored list of lane offsets.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  a                   constant factor of the polynomial
    //! @param[in]  b                   linear factor of the polynomial
    //! @param[in]  c                   quadratic factor of the polynomial
    //! @param[in]  d                   cubic factor of the polynomial
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddLaneOffset(double s,
                       double a,
                       double b,
                       double c,
                       double d) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a new link to a road by creating a new RoadLink object and adding it
    //! to the stored list of links.
    //!
    //! @param[in]  type                type of link
    //! @param[in]  elementType         element type of the link
    //! @param[in]  elementId           ID of the linked element
    //! @param[in]  contactPoint        contact point type of link on the linked element
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    virtual bool AddLink(RoadLinkType type,
                 RoadLinkElementType elementType,
                 const std::string &elementId,
                 ContactPointType contactPoint) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a new lane section to a road by creating a new RoadLaneSection object
    //! and adding it to the stored list of lane sections.
    //!
    //! @param[in]  start               start position s-coordinate
    //! @return                         created road lane section
    //-----------------------------------------------------------------------------
    virtual RoadLaneSectionInterface *AddRoadLaneSection(double start) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a new lane section to a road by creating a new RoadLaneSection object
    //! and adding it to the stored list of lane sections.
    //!
    //! @param[in]  start               start position s-coordinate
    //-----------------------------------------------------------------------------
    virtual void AddRoadSignal(const RoadSignalSpecification &signal) = 0;

    virtual void AddRoadObject(const RoadObjectSpecification &object) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the ID of the road.
    //!
    //! @return                         ID of the road
    //-----------------------------------------------------------------------------
    virtual const std::string GetId() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the stored list of elevation profiles.
    //!
    //! @return                         list of elevation profiles
    //-----------------------------------------------------------------------------
    virtual const std::vector<RoadElevation*> & GetElevations() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the stored list of lane offsets.
    //!
    //! @return                         list of lane offsets
    //-----------------------------------------------------------------------------
    virtual const std::vector<RoadLaneOffset*> & GetLaneOffsets() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the stored list of road geometries.
    //!
    //! @return                         list of road geometries
    //-----------------------------------------------------------------------------
    virtual const std::vector<RoadGeometryInterface*> & GetGeometries() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the stored list of road links.
    //!
    //! @return                         list of road links
    //-----------------------------------------------------------------------------
    virtual const std::vector<RoadLinkInterface*> & GetRoadLinks() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the stored list of lane sections.
    //!
    //! @return                         list of lane sections
    //-----------------------------------------------------------------------------
    virtual const std::vector<RoadLaneSectionInterface*> & GetLaneSections() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the stored list signals
    //!
    //! @return                         list of signals
    //-----------------------------------------------------------------------------
    virtual const std::vector<RoadSignalInterface*> & GetRoadSignals() const = 0;

    virtual const std::vector<RoadObjectInterface*> & GetRoadObjects() const = 0;

    //-----------------------------------------------------------------------------
    //! Sets the flag, if the road is in the reference direction or not.
    //!
    //! @param[in]  inDirection         Flag, if the road is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    virtual void SetInDirection(bool inDirection) = 0;

    //-----------------------------------------------------------------------------
    //! Returns if the road is in the reference direction or not.
    //!
    //! @return                         Flag, if the road is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    virtual bool GetInDirection() const = 0;

    virtual void AddRoadType(const RoadTypeSpecification &info) = 0;

    virtual RoadTypeInformation GetRoadType(double start) const = 0;

    virtual void SetJunctionId(const std::string& junctionId) = 0;

    virtual const std::string GetJunctionId() const = 0;

};


