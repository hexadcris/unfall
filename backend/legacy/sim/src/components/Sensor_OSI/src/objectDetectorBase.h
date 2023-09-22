/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2019 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/** \addtogroup SensorOSI
* @{
* \brief This file models the SensorOSI.
*
* \details This file models the Sensors which can be part of an agent.
*          The SensorOSI represent sensors which can detect other WorldObjects around the agent.
*          Those sensors have different approaches in detecting other WorldObjects.
*          The detected Objects can then be sent to the corresponding ADAS which uses them.
*
* \section MODULENAME_Outputs Outputs
* Output variables:
* name | meaning
* -----|------
* out_detectedObjects | Vector of all detected objects.
*
* Output channel IDs:
* Output Id | signal class | contained variables
* ----------|--------------|-------------
*  0        | SensorDataSignal  | out_detectedObjects
*
*
* \section MODULENAME_ExternalParameters External parameters
* name | meaning
* -----|---------
* sensorParameters | All parameters which describe the sensor.
*
* @} */

#pragma once

#include "include/modelInterface.h"
#include "include/profilesInterface.h"
#include "common/primitiveSignals.h"
#include "common/vector3d.h"
#include "common/sensorDataSignal.h"

#include "osi3/osi_sensorviewconfiguration.pb.h"
#include "osi3/osi_sensorview.pb.h"
#include "osi3/osi_sensordata.pb.h"

//-----------------------------------------------------------------------------
/** \brief This class is the common base for all OSI sensors.
*   \details This class is the common base for all OSI sensors.
*            It provides the basic functionality for all sensors.
*
*   \ingroup SensorOSI
*/
//-----------------------------------------------------------------------------
class ObjectDetectorBase : public SensorInterface
{
public:
    const std::string COMPONENTNAME = "Sensor_Model_Commonbase";

    ObjectDetectorBase(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        const ParameterInterface* parameters,
        PublisherInterface * const publisher,
        const CallbackInterface* callbacks,
        AgentInterface* agent);

    ObjectDetectorBase(const ObjectDetectorBase&) = delete;
    ObjectDetectorBase(ObjectDetectorBase&&) = delete;
    ObjectDetectorBase& operator=(const ObjectDetectorBase&) = delete;
    ObjectDetectorBase& operator=(ObjectDetectorBase&&) = delete;

    virtual ~ObjectDetectorBase() = default;

    /*!
     * \brief Update Inputs
     *
     * Function is called by framework when another component delivers a signal over
     * a channel to this component (scheduler calls update taks of other component).
     *
     * Refer to module description for input channels and input ids.
     *
     * \param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
     * \param[in]     data           Referenced signal (copied by sending component)
     * \param[in]     time           Current scheduling time
     */
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, int time) = 0;

    /*!
     * \brief Update outputs.
     *
     * Function is called by framework when this Component.has to deliver a signal over
     * a channel to another component (scheduler calls update task of this component).
     *
     * Refer to module description for output channels and output ids.
     *
     * \param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
     * \param[out]    data           Referenced signal (copied by this component)
     * \param[in]     time           Current scheduling time
     */
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time);

    /*!
     * \brief Process data within component.
     *
     * Function is called by framework when the scheduler calls the trigger task
     * of this component.
     *
     * Refer to module description for information about the module's task.
     *
     * \param[in]     time           Current scheduling time
     */
    virtual void Trigger(int time) = 0;

    /*!
     * For testing
     */
    const osi3::SensorData& getSensorData()
    {
        return sensorData;
    }

protected:

    /*!
     * \brief Adds the information of a detected moving object as DetectedMovingObject to the sensor data
     *
     * \param object            detected moving object
     * \param ownVelocity       velocity of own vehicle at sensor position in global coordinates
     * \param ownAcceleration   acceleration of own vehicle at sensor position in global coordinates
     * \param sensorPosition    position of sensor in global coordinates
     * \param ownYaw            yaw of sensor in global coordinates
     * \param ownYawRate        yawRate of own vehicle at sensor position in global coordinates
     */
    void AddMovingObjectToSensorData (const osi3::MovingObject& object, const point_t& ownVelocity, const point_t& ownAcceleration, const point_t& sensorPosition, double ownYaw, double ownYawRate, double ownYawAcceleration);

    /*!
     * \brief Adds the information of a detected stationary object as DetectedStationaryObject to the sensor data
     *
     * \param object            stationary moving object
     * \param sensorPosition    position of sensor in global coordinates
     * \param yaw               yaw of own vehicle in global coordinates
     */
    void AddStationaryObjectToSensorData (const osi3::StationaryObject& object, const point_t& sensorPosition, double yaw);

    /*!
     * \brief Returns the absolute position of the sensor
     *
     * This method calculates and returns the absolute position of the sensor by adding the agent's position
     * to the sensor's mounting position (and taking the yaw angle into account).
     *
     * \return The absolute position and yaw of the sensor with respect to world coordinates
     */
    Position GetAbsolutePosition();

    /*!
     * \brief Apply the predefined latency to the detected results
     *
     * The base implementation can be overriden in derived classes
     *
     * \param[in] time
     * \param[in] list of detected objects
     */
    osi3::SensorData ApplyLatency(int time, osi3::SensorData currentSensorData);

    /*!
   * \brief Rolls for the failure chance of the sensor.
   *
   * @return	     true if the sensor failed. false otherwise.
   */
   bool HasDetectionError();

   /*!
   * \brief Generates the basic SensorViewConfiguration
   *
   * @return SensorViewConfiguration for parametrization of Sensorview
   */
   virtual osi3::SensorViewConfiguration GenerateSensorViewConfiguration();

   /*!
   * \brief Calculates the bounding box of an object from the osi information
   *
   * @param[in] dimension                       osi dimension
   * @param[in] position                        osi position
   * @param[in] orientation                     osi orientation
   * @param[in] distanceReferencePointToFront   distance from the reference point to the front
   * @return bounding box of object
   */
   static polygon_t CalculateBoundingBox(const osi3::Dimension3d dimension, const osi3::Vector3d position, const osi3::Orientation3d orientation);

   /*!
   * \brief Translates the polygon from local coordinates to global coordinates
   *
   * @param[in] polygon                 polygon in local coordinates
   * @param[in] sensorPositionGlobal    position of the sensor in global coordinates
   * @param[in] yaw                     yaw of the sensor in global coordinates
   * @return polygon in global coordinates
   */
   polygon_t TransformPolygonToGlobalCoordinates(polygon_t polygon, point_t sensorPositionGlobal, double yaw) const;

   /*!
   * \brief Translates a point from global coordinates to local coordinates
   *
   * @param[in] point                   point in global coordinates
   * @param[in] sensorPositionGlobal    position of the sensor in global coordinates
   * @param[in] yaw                     yaw of the sensor in global coordinates
   * @return point in local coordinates
   */
   virtual point_t TransformPointToLocalCoordinates(point_t point, point_t sensorPositionGlobal, double yaw);

   /*!
   * \brief Calculates the position of the sensor in global coordinates
   *
   * @param[in] vehiclePosition     position of the vehicle in global coordinates
   * @param[in] yaw                 yaw of the vehicle in global coordinates
   * @return position of the sensor in global coordinates
   */
   point_t CalculateGlobalSensorPosition (point_t vehiclePosition, double yaw) const;

   /*!
   * \brief Calculates the relative velocity or acceleration between this vehicle and another in local vehicle coordinates
   *
   * @param[in] absolute    absolute velocity/acceleration of other vehicle in global coordinates
   * @param[in] own         absolute velocity/acceleration of own vehicle in global coordinates
   * @param[in] yaw         yaw of own vehicle in global coordinates
   * @return relative velocity/acceleration of other vehicle in vehicle coordinates
   */
   virtual point_t CalculateRelativeVector(const point_t absolute, const point_t own, double yaw);

   //! Returns the MovingObject in the sensor view which was defined as host vehicle (by id)
   static const osi3::MovingObject* FindHostVehicleInSensorView(const osi3::SensorView& sensorView);

   //! Returns the world coordinate position of the sensor
   point_t GetSensorPosition() const;

   std::list<std::pair<int, osi3::SensorData>> detectedObjectsBuffer;

   osi3::SensorData sensorData;

   openpass::sensors::Position position;
   int id;
   double failureProbability;
   double latency {0.0};
   int latencyInMs {0};

private:
    static constexpr double MIN_FAILURE_PROBABILITY = 1e-12;

    /*!
    * \brief This method parses the initial parameters required by the module.
    *
    */
    void ParseBasicParameter();
};


