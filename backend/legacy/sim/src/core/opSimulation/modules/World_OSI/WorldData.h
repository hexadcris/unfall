/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2018-2021 in-tech GmbH
 *               2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  WorldData.h
//! @brief This file provides access to underlying data structures for
//!        scenery and dynamic objects
//-----------------------------------------------------------------------------

#pragma once

#include <unordered_map>

#include "OWL/DataTypes.h"
#include "OWL/TrafficLight.h"
#include "OWL/MovingObject.h"
#include "include/roadInterface/roadInterface.h"
#include "include/roadInterface/junctionInterface.h"
#include "include/worldInterface.h"
#include "include/callbackInterface.h"
#include "common/openScenarioDefinitions.h"
#include "common/hypot.h"
#include "osi3/osi_groundtruth.pb.h"
#include "osi3/osi_sensorview.pb.h"
#include "osi3/osi_sensorviewconfiguration.pb.h"

#ifdef USE_PROTOBUF_ARENA
#include <google/protobuf/arena.h>
#endif

namespace OWL {

using Lane              = Interfaces::Lane;
using LaneBoundary      = Interfaces::LaneBoundary;
using Section           = Interfaces::Section;
using Road              = Interfaces::Road;
using Junction          = Interfaces::Junction;
using StationaryObject  = Implementation::StationaryObject;
using MovingObject      = Implementation::MovingObject;
using TrafficSign       = Implementation::TrafficSign;
using RoadMarking       = Implementation::RoadMarking;

using CLane              = const Interfaces::Lane;
using CSection           = const Interfaces::Section;
using CRoad              = const Interfaces::Road;
using CStationaryObject  = const Implementation::StationaryObject;
using CMovingObject      = const Implementation::MovingObject;

using Lanes = std::vector<Lane*>;
using Sections = std::vector<Section*>;
using Roads = std::vector<Road*>;
using StationaryObjects = std::vector<StationaryObject*>;
using MovingObjects = std::vector<MovingObject*>;

using CLanes = const std::vector<CLane*>;
using CSections = const std::vector<CSection*>;
using CRoads = const std::vector<CRoad*>;
using CStationaryObjects = const std::vector<CStationaryObject*>;
using CMovingObjects = const std::vector<CMovingObject*>;

template<typename T>
using IdMapping = std::map<OWL::Id, std::unique_ptr<T>>;

enum class SignalType
{
    TrafficSign,
    TrafficLight,
    Other
};

#ifdef USE_PROTOBUF_ARENA
//If using protobuf arena the SensorView must not be manually deleted.
struct DoNothing
{
    void operator() (osi3::SensorView*) {}
};
using SensorView_ptr = std::unique_ptr<osi3::SensorView, DoNothing>;
#else
using SensorView_ptr = std::unique_ptr<osi3::SensorView>;
#endif

namespace Interfaces {

//!This class contains the entire road network and all objects in the world
class WorldData
{
public:
    virtual ~WorldData() = default;

    //!Deletes all objects, roads, sections and lanes
    virtual void Clear() = 0;

    /*!
     * \brief Creates a OSI SensorView
     *
     * \param[in]   conf      SensorViewConfiguration to create SensorView from
     * \param[in]   host_id   The Id of the associated Agent
     *
     * \return      A OSI SensorView with filtered GroundTruth
     */
    virtual SensorView_ptr GetSensorView(osi3::SensorViewConfiguration& conf, int agentId) = 0;

    /*!
     * \brief Frees temporary objects
     *
     * A temporary arena is used to allocate SensorViews and copies of GroundTruth. This
     * method should be called after each time step.
     *
     * \note This method only provides a temporary solution against leaking arena memory and will be removed in the near future.
     */
    virtual void ResetTemporaryMemory() = 0;

    virtual const osi3::GroundTruth& GetOsiGroundTruth() const = 0;

    //!Returns a map of all Roads with their OSI Id
    virtual const std::unordered_map<std::string, Road*>& GetRoads() const = 0;

    //!Creates a new MovingObject linked to an AgentAdapter and returns it
    //!
    //! \param Id            Unique ID
    virtual Interfaces::MovingObject &AddMovingObject(const Id id) = 0;

    //!Creates a new StationaryObject linked to a TrafficObjectAdapter and returns it
    //!
    //! \param Id            Unique ID
    //! \param linkedObject  Object of type TrafficObjectAdapter which will be linked to new StationaryObject
    virtual Interfaces::StationaryObject& AddStationaryObject(const Id id, void* linkedObject) = 0;

    //!Creates a new TrafficSign and returns it
    //! \param Id            Unique ID
    //! \param odId          OpenDRIVE Id
    virtual Interfaces::TrafficSign& AddTrafficSign(const Id id, const std::string odId) = 0;

    //!Creates a new TrafficLight and returns it
    //! \param Id            Unique ID
    //! \param odId          OpenDRIVE Id
    //! \param openDriveType OpenDrive type (e.g "1.000.001 for generic three lights traffic light)
    virtual Interfaces::TrafficLight& AddTrafficLight(const std::vector<Id> ids,
                                                      const std::string odId,const std::string& openDriveType) = 0;

    //!Creates a new RoadMarking and returns it
    //! \param Id            Unique ID
    virtual Interfaces::RoadMarking& AddRoadMarking(const Id id) = 0;

    //! Adds a traffic sign to the assigned signs of lane
    //!
    //! \param laneId       OSI Id of the lane
    //! \param trafficSign  traffic sign to assign
    virtual void AssignTrafficSignToLane(OWL::Id laneId, Interfaces::TrafficSign& trafficSign) = 0;

    //! Adds a road marking to the assigned road markings of lane
    //!
    //! \param laneId       OSI Id of the lane
    //! \param roadMarking  roadMarking to assign
    virtual void AssignRoadMarkingToLane(OWL::Id laneId, Interfaces::RoadMarking& roadMarking) = 0;

    //! Adds a traffic light to the assigned traffic lights of lane
    //!
    //! \param laneId       OSI Id of the lane
    //! \param trafficLight traffic light to assign
    virtual void AssignTrafficLightToLane(OWL::Id laneId, Interfaces::TrafficLight &trafficLight) = 0;

    //!Deletes the moving object with the specified Id
    virtual void RemoveMovingObjectById(Id id) = 0; // change Id to MovingObject

    //!Returns the mapping of OpenDrive Ids to OSI Ids for trafficSigns
    virtual const std::unordered_map<std::string, Id>& GetTrafficSignIdMapping() const = 0;

    //!Return the OWL type of a signal of OpenDrive or type Unknown if there is no signal with this Id
    virtual SignalType GetSignalType(Id id) const = 0;

    //!Returns an invalid lane
    virtual const Implementation::InvalidLane& GetInvalidLane() const = 0;

    //!Returns a map of all lanes with their OSI Id
    virtual const IdMapping<Lane>& GetLanes() const = 0;

    //!Returns the lane with given OSI Idd
    virtual const Lane& GetLane(Id id) const = 0;

    //!Returns the lane boundaries with given OSI Id
    virtual const LaneBoundary& GetLaneBoundary(Id id) const = 0;

    //!Returns a map of all junctions with their OSI Id
    virtual const std::map<std::string, Junction*>& GetJunctions() const = 0;

    //!Returns the traffic sign with given OSI Id
    virtual TrafficSign& GetTrafficSign(Id id) = 0;

    //!Returns the traffic light with given OSI Id
    virtual TrafficLight& GetTrafficLight(Id id) = 0;
    
    //!Returns the stationary object with given Id
    virtual const StationaryObject& GetStationaryObject(Id id) const = 0;

    //!Returns the moving object with given Id
    virtual const MovingObject& GetMovingObject(Id id) const = 0;

    //! Sets the road graph as imported from OpenDrive
    //!
    //! \param roadGraph        graph representation of road network
    //! \param vertexMapping    mapping from roads (with direction) to the vertices of the roadGraph
    virtual void SetRoadGraph (const RoadGraph&& roadGraph, const RoadGraphVertexMapping&& vertexMapping) = 0;

    virtual void SetTurningRates(const TurningRates& turningRates) = 0;

    //! Returns the graph representation of the road network
    virtual const RoadGraph& GetRoadGraph() const = 0;

    //! Returns the mapping from roads (with direction) to the vertices of the roadGraph
    virtual const RoadGraphVertexMapping& GetRoadGraphVertexMapping() const = 0;

    virtual const TurningRates& GetTurningRates() const = 0;

    //!Creates a new lane with parameters specified by the OpenDrive lane
    //!
    //! \param Id              Unique ID
    //! \param odSection       OpenDrive section to add lane to
    //! \param odLane          OpenDrive lane to add
    //! \param laneBoundaries  Osi Ids of the left lane boundaries of the new lane
    virtual void AddLane(const Id id, RoadLaneSectionInterface& odSection, const RoadLaneInterface& odLane, const std::vector<Id> laneBoundaries) = 0;

    //! Creates a new lane boundary specified by the OpenDrive RoadMark
    //!
    //! \param Id               Unique ID
    //! \param odLaneRoadMark   OpenDrive roadMark (= laneBoundary) to add
    //! \param sectionStart     Start s coordinate of the section
    //! \param side             Specifies which side of a double line to add (or Single if not a double line)
    //! \return Osi id of the newly created laneBoundary
    virtual Id AddLaneBoundary(const Id id, const RoadLaneRoadMark &odLaneRoadMark, double sectionStart, LaneMarkingSide side) = 0;

    //! Sets the ids of the center lane boundaries for a section
    //!
    //! \param odSection        OpenDrive section for which to set the center line
    //! \param laneBoundaryIds  Osi ids of the center lane boundaries
    virtual void SetCenterLaneBoundary(const RoadLaneSectionInterface& odSection, std::vector<Id> laneBoundaryIds) = 0;

    //!Creates a new section with parameters specified by the OpenDrive section
    //!
    //! \param odRoad    OpenDrive road to add section to
    //! \param odSection OpenDrive section to add
    virtual void AddSection(const RoadInterface& odRoad, const RoadLaneSectionInterface& odSection) = 0;

    //!Creates a new road with parameters specified by the OpenDrive road
    //!
    //! \param odRoad    OpenDrive road to add
    virtual void AddRoad(const RoadInterface& odRoad) = 0;

    //!Creates a new junction with parameters specified by the OpenDrive junction
    //!
    //! \param odJunction    OpenDrive junction to add
    virtual void AddJunction(const JunctionInterface* odJunction) = 0;

    //!Adds a connection road (path) to a junction
    virtual void AddJunctionConnection(const JunctionInterface* odJunction, const RoadInterface& odRoad) = 0;

    //!Adds a priority entry of to connecting roads to a junction
    virtual void AddJunctionPriority(const JunctionInterface* odJunction, const std::string& high, const std::string& low) = 0;

    //!Adds a new lane geometry joint to the end of the current list of joints of the specified lane
    //! and a new geometry element and the boundary points of all affected right lane boundaries
    //!
    //! @param pointLeft    left point of the new joint
    //! @param pointCenter  reference point of the new joint
    //! @param pointRight   right point of the new joint
    //! @param sOffset      s offset of the new joint
    //! @param curvature    curvature of the lane at sOffset
    //! @param heading      heading of the lane at sOffset
    virtual void AddLaneGeometryPoint(const RoadLaneInterface& odLane,
                                      const Common::Vector2d& pointLeft,
                                      const Common::Vector2d& pointCenter,
                                      const Common::Vector2d& pointRight,
                                      const double sOffset,
                                      const double curvature,
                                      const double heading) = 0;

    //! Adds a new boundary point to the center line of the specified section
    //!
    //! \param odSection    section to add boundary point to
    //! \param pointCenter  point to add
    //! \param sOffset      s offset of the new point
    //! \param heading      heading of the road at sOffset
    virtual void AddCenterLinePoint(const RoadLaneSectionInterface& odSection,
                                    const Common::Vector2d& pointCenter,
                                    const double sOffset,
                                    double heading) = 0;

    //!Sets successorOdLane as successor of odLane
    virtual void AddLaneSuccessor(/* const */ RoadLaneInterface& odLane,
            /* const */ RoadLaneInterface& successorOdLane) = 0;

    //!Sets predecessorOdLane as predecessor of odLane
    virtual void AddLanePredecessor(/* const */ RoadLaneInterface& odLane,
            /* const */ RoadLaneInterface& predecessorOdLane) = 0;

    //!Sets successorRoad as successor of road in OSI
    virtual void SetRoadSuccessor(const RoadInterface& road,  const RoadInterface& successorRoad) = 0;

    //!Sets predecessorRoad as predecessor of road in OSI
    virtual void SetRoadPredecessor(const RoadInterface& road,  const RoadInterface& predecessorRoad) = 0;

    //!Sets successorSection as successor of section in OSI
    virtual void SetSectionSuccessor(const RoadLaneSectionInterface& section,  const RoadLaneSectionInterface& successorSection) = 0;

    //!Sets predecessorSection as predecessor of section in OSI
    virtual void SetSectionPredecessor(const RoadLaneSectionInterface& section,  const RoadLaneSectionInterface& predecessorSection) = 0;

    //!Sets a junction as the successor of a road
    virtual void SetRoadSuccessorJunction(const RoadInterface& road,  const JunctionInterface* successorJunction) = 0;

    //!Sets a junction as the predecessor of a road
    virtual void SetRoadPredecessorJunction(const RoadInterface& road,  const JunctionInterface* predecessorJunction) = 0;

    //!Currently not implemented
    virtual void ConnectLanes(/*const*/ RoadLaneSectionInterface& firstOdSection,
                                        /*const*/ RoadLaneSectionInterface& secondOdSection,
                                        const std::map<int, int>& lanePairs,
                                        bool isPrev) = 0;


    static constexpr double THRESHOLD_ILLUMINATION_LEVEL1 = 0.01;       //!< Upper threshold for osi3::EnvironmentalConditions::AmbientIllumination::Level1
    static constexpr double THRESHOLD_ILLUMINATION_LEVEL2 = 1.0;        //!< Upper threshold for osi3::EnvironmentalConditions::AmbientIllumination::Level2
    static constexpr double THRESHOLD_ILLUMINATION_LEVEL3 = 3.0;        //!< Upper threshold for osi3::EnvironmentalConditions::AmbientIllumination::Level3
    static constexpr double THRESHOLD_ILLUMINATION_LEVEL4 = 10.0;       //!< Upper threshold for osi3::EnvironmentalConditions::AmbientIllumination::Level4
    static constexpr double THRESHOLD_ILLUMINATION_LEVEL5 = 20.0;       //!< Upper threshold for osi3::EnvironmentalConditions::AmbientIllumination::Level5
    static constexpr double THRESHOLD_ILLUMINATION_LEVEL6 = 400.0;      //!< Upper threshold for osi3::EnvironmentalConditions::AmbientIllumination::Level6
    static constexpr double THRESHOLD_ILLUMINATION_LEVEL7 = 1000.0;     //!< Upper threshold for osi3::EnvironmentalConditions::AmbientIllumination::Level7
    static constexpr double THRESHOLD_ILLUMINATION_LEVEL8 = 10000.0;    //!< Upper threshold for osi3::EnvironmentalConditions::AmbientIllumination::Level8
    static constexpr double THRESHOLD_FOG_DENSE = 50.0;         //!< Upper threshold for osi3::EnvironmentalConditions::Fog::Dense
    static constexpr double THRESHOLD_FOG_THICK = 200.0;        //!< Upper threshold for osi3::EnvironmentalConditions::Fog::Thick
    static constexpr double THRESHOLD_FOG_LIGHT = 1000.0;       //!< Upper threshold for osi3::EnvironmentalConditions::Fog::Light
    static constexpr double THRESHOLD_FOG_MIST = 2000.0;        //!< Upper threshold for osi3::EnvironmentalConditions::Fog::Mist
    static constexpr double THRESHOLD_FOG_POOR = 4000.0;        //!< Upper threshold for osi3::EnvironmentalConditions::Fog::Poor_Visibility
    static constexpr double THRESHOLD_FOG_MODERATE = 10000.0;   //!< Upper threshold for osi3::EnvironmentalConditions::Fog::Moderate_Visibility
    static constexpr double THRESHOLD_FOG_GOOD = 40000.0;       //!< Upper threshold for osi3::EnvironmentalConditions::Fog::Good_Visibility

    //! \brief Translates an openScenario environment to OSI3
    //!
    //! The following thresholds are used for a mapping of illumination level:
    //! - \see THRESHOLD_ILLUMINATION_LEVEL1
    //! - \see THRESHOLD_ILLUMINATION_LEVEL2
    //! - \see THRESHOLD_ILLUMINATION_LEVEL3
    //! - \see THRESHOLD_ILLUMINATION_LEVEL4
    //! - \see THRESHOLD_ILLUMINATION_LEVEL5
    //! - \see THRESHOLD_ILLUMINATION_LEVEL6
    //! - \see THRESHOLD_ILLUMINATION_LEVEL7
    //! - \see THRESHOLD_ILLUMINATION_LEVEL8
    //!
    //! The following thresholds are used for a mapping of fog:
    //! - \see THRESHOLD_FOG_DENSE
    //! - \see THRESHOLD_FOG_THICK
    //! - \see THRESHOLD_FOG_LIGHT
    //! - \see THRESHOLD_FOG_MIST
    //! - \see THRESHOLD_FOG_POOR
    //! - \see THRESHOLD_FOG_MODERATE
    //! - \see THRESHOLD_FOG_GOOD
    virtual void SetEnvironment(const openScenario::EnvironmentAction& environment) = 0;

    //!Resets the world for new run; deletes all moving objects
    virtual void Reset() = 0;

    /*!
     * \brief Retrieves the OWL Id of an agent
     *
     * \param   agentId[in]     Agent id (as used in AgentInterface)
     *
     * \return  OWL Id of the underlying OSI object
     */
    virtual OWL::Id GetOwlId(int agentId) const = 0;

    /*!
     * \brief Retrieves the simualtion framework Id of an agent associated to the given OWL Id
     *
     * \param   owlId[in]     OWL Id
     *
     * \return  Agent Id of the associated agent
     */
    virtual int GetAgentId(const OWL::Id owlId) const = 0;
};

}

class WorldData : public Interfaces::WorldData
{
public:

#ifdef USE_PROTOBUF_ARENA
    using GroundTruth_ptr = osi3::GroundTruth*;
#else
    using GroundTruth_ptr = std::unique_ptr<osi3::GroundTruth>;
#endif

    WorldData(const CallbackInterface* callbacks);

    ~WorldData() override;

    void Clear() override;

    SensorView_ptr GetSensorView(osi3::SensorViewConfiguration& conf, int agentId) override;

    void ResetTemporaryMemory() override;

    const osi3::GroundTruth& GetOsiGroundTruth() const override;

    /*!
     * \brief Retrieves a filtered OSI GroundTruth
     *
     * \param[in]   conf        The OSI SensorViewConfiguration to be used for filtering
     * \param[in]   reference   Host of the sensor
     *
     * \return      A OSI GroundTruth filtered by the given SensorViewConfiguration
     */
    GroundTruth_ptr GetFilteredGroundTruth(const osi3::SensorViewConfiguration& conf, const Interfaces::MovingObject& reference);

    /*!
     * \brief Retrieves the TrafficSigns located in the given sector (geometric shape)
     *
     * \param[in]   origin              Origin of the sector shape
     * \param[in]   radius              Radius of the sector shape
     * \param[in]   leftBoundaryAngle   Left boundary angle of the sector shape
     * \param[in]   rightBoundaryAngle  Right boundary angle of the sector shape
     *
     * \return      Vector of TrafficSing pointers located in the given sector
     */
    std::vector<const Interfaces::TrafficSign*> GetTrafficSignsInSector(const Primitive::AbsPosition& origin,
                                                                        double radius,
                                                                        double leftBoundaryAngle,
                                                                        double rightBoundaryAngle);

        /*!
         * \brief Retrieves TrafficLights located in the given sector (geometric shape)
         *
         * \param[in]   origin              Origin of the sector shape
         * \param[in]   radius              Radius of the sector shape
         * \param[in]   leftBoundaryAngle   Left boundary angle of the sector shape
         * \param[in]   rightBoundaryAngle  Right boundary angle of the sector shape
         *
         * \return      Vector of TrafficLight pointers located in the given sector
         */
        std::vector<const Interfaces::TrafficLight*> GetTrafficLightsInSector(const Primitive::AbsPosition& origin,
                                                                              double radius,
                                                                              double leftBoundaryAngle,
                                                                              double rightBoundaryAngle);

    /*!
     * \brief Retrieves the RoadMarkings located in the given sector (geometric shape)
     *
     * \param[in]   origin              Origin of the sector shape
     * \param[in]   radius              Radius of the sector shape
     * \param[in]   leftBoundaryAngle   Left boundary angle of the sector shape
     * \param[in]   rightBoundaryAngle  Right boundary angle of the sector shape
     *
     * \return      Vector of RoadMarking pointers located in the given sector
     */
    std::vector<const Interfaces::RoadMarking*> GetRoadMarkingsInSector(const Primitive::AbsPosition& origin,
                                                                        double radius,
                                                                        double leftBoundaryAngle,
                                                                        double rightBoundaryAngle);

    /*!
     * \brief Retrieves the StationaryObjects located in the given sector (geometric shape)
     *
     * \param[in]   origin              Origin of the sector shape
     * \param[in]   radius              Radius of the sector shape
     * \param[in]   leftBoundaryAngle   Left boundary angle of the sector shape
     * \param[in]   rightBoundaryAngle  Right boundary angle of the sector shape
     *
     * \return      Vector of StationaryObject pointers located in the given sector
     */
    std::vector<const Interfaces::StationaryObject*> GetStationaryObjectsInSector(const Primitive::AbsPosition& origin,
                                                                                  double radius,
                                                                                  double leftBoundaryAngle,
                                                                                  double rightBoundaryAngle);

    /*!
     * \brief Retrieves the MovingObjects located in the given sector (geometric shape)
     *
     * \param[in]   origin              Origin of the sector shape
     * \param[in]   radius              Radius of the sector shape
     * \param[in]   leftBoundaryAngle   Left boundary angle of the sector shape
     * \param[in]   rightBoundaryAngle  Right boundary angle of the sector shape
     *
     * \return      Vector of MovingObject pointers located in the given sector
     */
    std::vector<const Interfaces::MovingObject*> GetMovingObjectsInSector(const Primitive::AbsPosition& origin,
                                                                          double radius,
                                                                          double leftBoundaryAngle,
                                                                          double rightBoundaryAngle);

    /*!
     * \brief Add the information about the host vehicle to the given SensorView
     *
     * \param host_id       id of the host vehicle
     * \param sensorView    SensorView to modify
     */
    void AddHostVehicleToSensorView(Id host_id, osi3::SensorView& sensorView);

    OWL::Id GetOwlId(int agentId) const override;
    int GetAgentId(const OWL::Id owlId) const override;

    void SetRoadGraph (const RoadGraph&& roadGraph, const RoadGraphVertexMapping&& vertexMapping) override;
    void SetTurningRates (const TurningRates& turningRates) override;
    void AddLane(const Id id, RoadLaneSectionInterface &odSection, const RoadLaneInterface& odLane, const std::vector<Id> laneBoundaries) override;
    Id AddLaneBoundary(const Id id, const RoadLaneRoadMark &odLaneRoadMark, double sectionStart, LaneMarkingSide side) override;
    virtual void SetCenterLaneBoundary(const RoadLaneSectionInterface& odSection, std::vector<Id> laneBoundaryIds) override;
    void AddSection(const RoadInterface& odRoad, const RoadLaneSectionInterface& odSection) override;
    void AddRoad(const RoadInterface& odRoad) override;
    void AddJunction(const JunctionInterface* odJunction) override;
    void AddJunctionConnection(const JunctionInterface* odJunction, const RoadInterface& odRoad) override;
    void AddJunctionPriority(const JunctionInterface* odJunction,  const std::string& high, const std::string& low) override;

    void AddLaneSuccessor(/* const */ RoadLaneInterface& odLane,
                                      /* const */ RoadLaneInterface& successorOdLane) override;
    void AddLanePredecessor(/* const */ RoadLaneInterface& odLane,
                                        /* const */ RoadLaneInterface& predecessorOdLane) override;


    void ConnectLanes(/*const*/ RoadLaneSectionInterface& firstOdSection,
                                /*const*/ RoadLaneSectionInterface& secondOdSection,
                                const std::map<int, int>& lanePairs,
                                bool isPrev) override;
    void SetRoadSuccessor(const RoadInterface& road,  const RoadInterface& successorRoad) override;
    void SetRoadPredecessor(const RoadInterface& road,  const RoadInterface& predecessorRoad) override;
    void SetRoadSuccessorJunction(const RoadInterface& road,  const JunctionInterface* successorJunction) override;
    void SetRoadPredecessorJunction(const RoadInterface& road,  const JunctionInterface* predecessorJunction) override;
    void SetSectionSuccessor(const RoadLaneSectionInterface& section,  const RoadLaneSectionInterface& successorSection) override;
    void SetSectionPredecessor(const RoadLaneSectionInterface& section,  const RoadLaneSectionInterface& predecessorSection) override;

    Interfaces::MovingObject& AddMovingObject(const Id id) override;
    Interfaces::StationaryObject& AddStationaryObject(const Id id, void* linkedObject) override;
    Interfaces::TrafficSign& AddTrafficSign(const Id id, const std::string odId) override;
    Interfaces::TrafficLight& AddTrafficLight(const std::vector<Id> ids, const std::string odId,const std::string& type) override;
    Interfaces::RoadMarking& AddRoadMarking(const Id id) override;

    void AssignTrafficSignToLane(OWL::Id laneId, Interfaces::TrafficSign &trafficSign) override;
    void AssignRoadMarkingToLane(OWL::Id laneId, Interfaces::RoadMarking& roadMarking) override;
    void AssignTrafficLightToLane(OWL::Id laneId, Interfaces::TrafficLight &trafficLight) override;

    void RemoveMovingObjectById(Id id) override;

    void AddLaneGeometryPoint(const RoadLaneInterface& odLane,
                              const Common::Vector2d& pointLeft,
                              const Common::Vector2d& pointCenter,
                              const Common::Vector2d& pointRight,
                              const double sOffset,
                              const double curvature,
                              const double heading) override;

    virtual void AddCenterLinePoint(const RoadLaneSectionInterface& odSection,
                                    const Common::Vector2d& pointCenter,
                                    const double sOffset,
                                    double heading) override;

    const IdMapping<StationaryObject>& GetStationaryObjects() const;

    const Interfaces::StationaryObject& GetStationaryObject(Id id) const override;
    const Interfaces::MovingObject& GetMovingObject(Id id) const override;

    const RoadGraph& GetRoadGraph() const override;
    const RoadGraphVertexMapping& GetRoadGraphVertexMapping() const override;
    const IdMapping<Lane>& GetLanes() const override;
    const Lane& GetLane(Id id) const override;
    const LaneBoundary& GetLaneBoundary(Id id) const override;
    const TurningRates& GetTurningRates() const override;
    const std::unordered_map<std::string, Road*>& GetRoads() const override;
    const std::map<std::string, Junction*>& GetJunctions() const override;
    Interfaces::TrafficSign& GetTrafficSign(Id id) override;
    Interfaces::TrafficLight& GetTrafficLight(Id id) override;
    const Implementation::InvalidLane& GetInvalidLane() const override {return invalidLane;}

    const std::unordered_map<std::string, Id>& GetTrafficSignIdMapping() const override
    {
        return trafficSignIdMapping;
    }

    SignalType GetSignalType(Id id) const override;

    void SetEnvironment(const openScenario::EnvironmentAction& environment) override;


    static bool IsCloseToSectorBoundaries(double distanceToCenter,
                                          double angle,
                                          double leftBoundaryAngle,
                                          double rightBoundaryAngle,
                                          double maxDistanceToBoundary)
    {
        const double rightAngleDifference = angle - rightBoundaryAngle;
        const double leftAngleDifference = angle - leftBoundaryAngle;

        //For angles > 90° the center of the sector is the closest point
        const double distanceToRightBoundary = std::abs(rightAngleDifference) >= M_2_PI ? distanceToCenter
                                                                                        : distanceToCenter * std::sin(rightAngleDifference);
        const double distanceToLeftBoundary = std::abs(leftAngleDifference) >= M_2_PI ? distanceToCenter
                                                                                      : distanceToCenter * std::sin(leftAngleDifference);
        return distanceToRightBoundary <= maxDistanceToBoundary || distanceToLeftBoundary <= maxDistanceToBoundary;
    }

    /*!
     * \brief Applies a sector-shaped filter on a list of objects
     *
     * \param[in]   objects               List of objects to apply filter to
     * \param[in]   origin                Origin of sector-shape
     * \param[in]   radius                Radius of the sector-shape. Negative value results in empty result
     * \param[in]   leftBoundaryAngle     Angle of left boundary of sector-shape (view from origin) [-PI, PI] [rad]
     * \param[in]   rightBoundaryAngle    Angle of right boundary of sector-shape (view from origin) [-PI, PI] [rad]
     *
     * \return      Vector of objects inside the sector-shape
     */
    template<typename T>
    std::vector<const T*> ApplySectorFilter(const std::vector<T*>& objects,
                                            const Primitive::AbsPosition& origin,
                                            double radius,
                                            double leftBoundaryAngle,
                                            double rightBoundaryAngle)
    {
        std::vector<const T*> filteredObjects;
        constexpr double EPS = 1e-9;
        bool anglesDiffer = std::abs(leftBoundaryAngle - rightBoundaryAngle) > EPS;

        if (!anglesDiffer || radius <= 0.0)
        {
            return filteredObjects;
        }
        bool wrappedAngle = leftBoundaryAngle < rightBoundaryAngle;
        anglesDiffer = std::abs(leftBoundaryAngle - rightBoundaryAngle) > EPS;

        for (const auto& object : objects)
        {
            const auto& absPosition = object->GetReferencePointPosition();
            const auto relativePosition = absPosition - origin;
            const auto distance = relativePosition.distance();
            const auto dimension = object->GetDimension();
            const auto diagonal = openpass::hypot(dimension.width, dimension.length);

            if (distance > radius + 0.5 * diagonal)
            {
                continue;
            }

            if (anglesDiffer && distance > 0.0)
            {
                double direction = std::atan2(relativePosition.y, relativePosition.x);

                if (wrappedAngle)
                {
                    if (direction < rightBoundaryAngle && direction > leftBoundaryAngle)
                    {
                        if (!IsCloseToSectorBoundaries(distance, direction, leftBoundaryAngle, rightBoundaryAngle, 0.5 * diagonal))
                        {
                            continue;
                        }
                    }
                }
                else if (direction < rightBoundaryAngle || direction > leftBoundaryAngle)
                {
                    if (!IsCloseToSectorBoundaries(distance, direction, leftBoundaryAngle, rightBoundaryAngle, 0.5 * diagonal))
                    {
                        continue;
                    }
                }
            }

            filteredObjects.push_back(object);
        }

        return filteredObjects;
    }

    void Reset() override;

    void initializeDefaultTrafficLight(osi3::TrafficLight *pTrafficLight);

protected:
    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char* file,
             int line,
             const std::string& message)
    {
        if (callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:
    const CallbackInterface* callbacks;
    uint64_t next_free_uid{0};
    std::unordered_map<std::string, Id>         trafficSignIdMapping;

    IdMapping<Lane>             lanes;
    IdMapping<LaneBoundary>     laneBoundaries;
    IdMapping<StationaryObject> stationaryObjects;
    IdMapping<MovingObject>     movingObjects;
    IdMapping<Interfaces::TrafficSign>      trafficSigns;
    IdMapping<Interfaces::TrafficLight>     trafficLights;
    IdMapping<Interfaces::RoadMarking>      roadMarkings;

    std::unordered_map<std::string, Road*> roadsById;
    std::map<std::string, Junction*> junctionsById;

    std::unordered_map<const RoadInterface*, std::unique_ptr<Road>>                 roads;
    std::unordered_map<const RoadLaneSectionInterface*, std::unique_ptr<Section>>   sections;
    std::unordered_map<const JunctionInterface*, std::unique_ptr<Junction>>         junctions;

    std::unordered_map<const RoadLaneInterface*, osi3::Lane*> osiLanes;

    RoadGraph roadGraph;
    RoadGraphVertexMapping vertexMapping;
    TurningRates turningRates;

#ifdef USE_PROTOBUF_ARENA
    google::protobuf::Arena arena;
    google::protobuf::Arena tempArena;
#endif
    GroundTruth_ptr osiGroundTruth;

    const Implementation::InvalidLane invalidLane;
    };

}
