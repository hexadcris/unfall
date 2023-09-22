/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2016-2017 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "include/modelInterface.h"

/*!
 * \brief records the state of the agent.
 *
 * This component is used to record the state of agent in every single timestep.
 *
 * \ingroup SensorRecordState
 */

class SensorRecordStateImplementation : public SensorInterface
{
public:
    const std::string COMPONENTNAME = "SensorRecordState";

    SensorRecordStateImplementation(
            std::string componentName,
            bool isInit,
            int priority,
            int offsetTime,
            int responseTime,
            int cycleTime,
            StochasticsInterface *stochastics,
            WorldInterface *world,
            const ParameterInterface *parameters,
            PublisherInterface * const publisher,
            const CallbackInterface *callbacks,
            AgentInterface *agent);

    SensorRecordStateImplementation(const SensorRecordStateImplementation&) = delete;
    SensorRecordStateImplementation(SensorRecordStateImplementation&&) = delete;
    SensorRecordStateImplementation& operator=(const SensorRecordStateImplementation&) = delete;
    SensorRecordStateImplementation& operator=(SensorRecordStateImplementation&&) = delete;
    virtual ~SensorRecordStateImplementation() = default;

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
    void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

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
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

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
    void Trigger(int time);
};


