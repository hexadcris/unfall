/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2016 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/


#include <memory>
#include <qglobal.h>
#include "Common/primitiveSignals.h"
#include "sensor_distance_implementation.h"

Sensor_Distance_Implementation::Sensor_Distance_Implementation(std::string componentName,
                                                               bool isInit,
                                                               int priority,
                                                               int offsetTime,
                                                               int responseTime,
                                                               int cycleTime,
                                                               StochasticsInterface *stochastics,
                                                               WorldInterface *world,
                                                               const ParameterInterface *parameters, PublisherInterface * const publisher,
                                                               const CallbackInterface *callbacks,
                                                               AgentInterface *agent) :
    SensorInterface(componentName,
                    isInit,
                    priority,
                    offsetTime,
                    responseTime,
                    cycleTime,
                    stochastics,
                    world,
                    parameters,
                    publisher,
                    callbacks,
                    agent)
{
}

void Sensor_Distance_Implementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);

    std::runtime_error("invalid link");

}

void Sensor_Distance_Implementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    std::stringstream log;
    log << COMPONENTNAME << " (component " << GetComponentName() << ", agent " << GetAgent()->GetAgentId() << ", agentTypeId " << GetAgent()->GetAgentTypeId() << " output data for local link " << localLinkId << ": ";
    LOG(CbkLogLevel::Debug, log.str());

    double value;

    if (localLinkId==0)
    {
        value = out_distanceToNextAgent;
        try
        {
            data = std::make_shared<DoubleSignal const>(value);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msgEx = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msgEx);
            throw std::runtime_error(msgEx);
        }
    }
    else if (localLinkId==1)
    {
        value = out_agentVelocity;
        try
        {
            data = std::make_shared<DoubleSignal const>(value);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msgEx = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msgEx);
            throw std::runtime_error(msgEx);
        }
    }
    else
    {
        const std::string msgDefault = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msgDefault);
        throw std::runtime_error(msgDefault);
    }

}

void Sensor_Distance_Implementation::Trigger(int time)
{
    Q_UNUSED(time);

    out_distanceToNextAgent = GetAgent()->GetDistanceToFrontAgent(0);
    out_agentVelocity = GetAgent()->GetVelocityX();
}
