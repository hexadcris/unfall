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
//! @file  ModelInterface.h
//! @brief This file contains the interface of the component models to interact
//!        with the framework.
//!
//! contains classes SignalInterface, ModelInterface, RestrictedModelInterface,
//! UnrestrictedModelInterface, AlgorithmInterface, ActionInterface,
//! DynamicsInterface, SensorInterface, InitInterface
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <string>

#include "include/agentInterface.h"
#include "include/callbackInterface.h"
#include "include/observationInterface.h"
#include "include/publisherInterface.h"
#include "include/signalInterface.h"

class ParameterInterface;
class StochasticsInterface;
class WorldInterface;

namespace core {
class EventNetworkInterface;
}

//-----------------------------------------------------------------------------
//! Interface of functionality which has to be provided by component models
//-----------------------------------------------------------------------------
class ModelInterface
{
public:
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //-----------------------------------------------------------------------------
    ModelInterface(bool isInit,
                   int priority,
                   int offsetTime,
                   int responseTime,
                   int cycleTime) :
        isInit(isInit),
        priority(priority),
        offsetTime(offsetTime),
        responseTime(responseTime),
        cycleTime(cycleTime)
    {}
    ModelInterface(const ModelInterface&) = delete;
    ModelInterface(ModelInterface&&) = delete;
    ModelInterface& operator=(const ModelInterface&) = delete;
    ModelInterface& operator=(ModelInterface&&) = delete;
    virtual ~ModelInterface() = default;

    //-----------------------------------------------------------------------------
    //! Function is called by framework when another component delivers a signal over
    //! a channel to this component (scheduler calls update taks of other component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    //! @param[in]     data           Referenced signal (copied by sending component)
    //! @param[in]     time           Current scheduling time
    //-----------------------------------------------------------------------------
    virtual void UpdateInput(int localLinkId,
                             const std::shared_ptr<SignalInterface const> &data,
                             int time) = 0;

    //-----------------------------------------------------------------------------
    //! Function is called by framework when this Component.has to deliver a signal over
    //! a channel to another component (scheduler calls update task of this component).
    //!
    //! @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    //! @param[out]    data           Referenced signal (copied by this component)
    //! @param[in]     time           Current scheduling time
    //-----------------------------------------------------------------------------
    virtual void UpdateOutput(int localLinkId,
                              std::shared_ptr<SignalInterface const> &data,
                              int time) = 0;

    //-----------------------------------------------------------------------------
    //! Function is called by framework when the scheduler calls the trigger task
    //! of this component
    //!
    //! @param[in]     time           Current scheduling time
    //-----------------------------------------------------------------------------
    virtual void Trigger(int time) = 0;

    //-----------------------------------------------------------------------------
    //! Checks if this component is configured as init module
    //!
    //! @return                       True if this component is an init module
    //-----------------------------------------------------------------------------
    bool GetInit() const
    {
        return isInit;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves priority of this component
    //!
    //! @return                       Component priority
    //-----------------------------------------------------------------------------
    int GetPriority() const
    {
        return priority;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves offset time of the trigger task of this component
    //!
    //! @return                       Trigger task offset time
    //-----------------------------------------------------------------------------
    int GetOffsetTime() const
    {
        return offsetTime;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves response time of the update task of this component (time between
    //! trigger task and update task)
    //!
    //! @return                       Update task response time
    //-----------------------------------------------------------------------------
    int GetResponseTime() const
    {
        return responseTime;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves cycle time of the trigger task of this component
    //!
    //! @return                       Trigger task cycle time
    //-----------------------------------------------------------------------------
    int GetCycleTime() const
    {
        return cycleTime;
    }

private:
    bool isInit;                          //!< Indicates if component is an init module
    int priority;                         //!< Priority of this component
    int offsetTime;                       //!< Offset time of this components trigger task
    int responseTime;                     //!< Response time of this components update task
    int cycleTime;                        //!< Cycle time of this components trigger task
};

//-----------------------------------------------------------------------------
//! Interface of restricted functionality
//-----------------------------------------------------------------------------
class RestrictedModelInterface : public ModelInterface
{
public:
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //-----------------------------------------------------------------------------
    RestrictedModelInterface(std::string componentName,
                             bool isInit,
                             int priority,
                             int offsetTime,
                             int responseTime,
                             int cycleTime,
                             StochasticsInterface *stochastics,
                             const ParameterInterface *parameters,
                             PublisherInterface * const publisher,
                             const CallbackInterface *callbacks) :
        ModelInterface(isInit, priority, offsetTime, responseTime, cycleTime),
        callbacks(callbacks),
        componentName(componentName),
        stochastics(stochastics),
        parameters(parameters),
        publisher(publisher)
    {}
    RestrictedModelInterface(const RestrictedModelInterface&) = delete;
    RestrictedModelInterface(RestrictedModelInterface&&) = delete;
    RestrictedModelInterface& operator=(const RestrictedModelInterface&) = delete;
    RestrictedModelInterface& operator=(RestrictedModelInterface&&) = delete;
    virtual ~RestrictedModelInterface() = default;

    //-----------------------------------------------------------------------------
    //! Retrieves name of this component.
    //!
    //! @return                       Component name of this model
    //-----------------------------------------------------------------------------
    std::string GetComponentName() const
    {
        return componentName;
    }

protected:
    //-----------------------------------------------------------------------------
    //! Retrieves the stochastics functionality of the framework
    //!
    //! @return                       Stochastics functionality
    //-----------------------------------------------------------------------------
    StochasticsInterface *GetStochastics() const
    {
        return stochastics;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves the configuration parameters of this component
    //!
    //! @return                       Reference to configuration parameters
    //-----------------------------------------------------------------------------
    const ParameterInterface *GetParameters() const
    {
        return parameters;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves the reference to the publishing module for data recording
    //!
    //! @return                       Publisher
    //-----------------------------------------------------------------------------
    PublisherInterface* GetPublisher() const
    {
        return publisher;
    }

    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char *file,
             int line,
             const std::string &message) const
    {
        if(callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:
    // Access to the following members is provided by the corresponding member
    // functions.
    const CallbackInterface *callbacks;   //!< Reference to framework callbacks
    std::string componentName;                      //!< Name of this component
    StochasticsInterface *stochastics;    //!< Reference to the stochastics functionality of the framework
    const ParameterInterface *parameters; //!< Reference to the configuration parameters
    PublisherInterface * const publisher; //!< Reference to the publisher module
};

//-----------------------------------------------------------------------------
//! Interface of unrestricted functionality
//-----------------------------------------------------------------------------
class UnrestrictedModelInterface : public RestrictedModelInterface
{
public:
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     componentId    Corresponds to "id" of "Component"
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //! @param[in]     stochastics    Pointer to the stochastics class loaded by the framework
    //! @param[in]     world          Pointer to the world
    //! @param[in]     parameters     Pointer to the parameters of the module
    //! @param[in]     observations   Pointer to the observations of the module
    //! @param[in]     callbacks      Pointer to the callbacks
    //! @param[in]     agent          Pointer to the agent in which the module is situated
    //-----------------------------------------------------------------------------
    UnrestrictedModelInterface(std::string componentName,
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
                               AgentInterface *agent) :
        RestrictedModelInterface(componentName,
                                 isInit,
                                 priority,
                                 offsetTime,
                                 responseTime,
                                 cycleTime,
                                 stochastics,
                                 parameters,
                                 publisher,
                                 callbacks),
        agent(agent),
        world(world)
    {}
    UnrestrictedModelInterface(const UnrestrictedModelInterface&) = delete;
    UnrestrictedModelInterface(UnrestrictedModelInterface&&) = delete;
    UnrestrictedModelInterface& operator=(const UnrestrictedModelInterface&) = delete;
    UnrestrictedModelInterface& operator=(UnrestrictedModelInterface&&) = delete;
    virtual ~UnrestrictedModelInterface() = default;

protected:
    //-----------------------------------------------------------------------------
    //! Retrieves the world representation from the framework
    //!
    //! @return                       World (scenery and agents)
    //-----------------------------------------------------------------------------
    WorldInterface *GetWorld() const
    {
        return world;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves the agent which contains this component
    //!
    //! @return                       Agent embedding this component
    //-----------------------------------------------------------------------------
    AgentInterface *GetAgent() const
    {
        return agent;
    }

private:
    AgentInterface *agent;                //!< Reference to agent containing this component
public: //RP: BAD HACK! just for testing FMI
    WorldInterface *world;                //!< Reference to the world representation of the framework
};

class AlgorithmInterface : public RestrictedModelInterface
{
public:
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     componentId    Corresponds to "id" of "Component"
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //! @param[in]     stochastics    Pointer to the stochastics class loaded by the framework
    //! @param[in]     parameters     Pointer to the parameters of the module
    //! @param[in]     observations   Pointer to the observations of the module
    //! @param[in]     callbacks      Pointer to the callbacks
    //! @param[in]     agent          Pointer to the agent in which the module is situated
    //-----------------------------------------------------------------------------
    AlgorithmInterface(std::string componentName,
                       bool isInit,
                       int priority,
                       int offsetTime,
                       int responseTime,
                       int cycleTime,
                       StochasticsInterface *stochastics,
                       const ParameterInterface *parameters,
                       PublisherInterface * const publisher,
                       const CallbackInterface *callbacks,
                       AgentInterface *agent) :
        RestrictedModelInterface(componentName,
                                 isInit,
                                 priority,
                                 offsetTime,
                                 responseTime,
                                 cycleTime,
                                 stochastics,
                                 parameters,
                                 publisher,
                                 callbacks),
        agent(agent)
    {}
    AlgorithmInterface(const AlgorithmInterface&) = delete;
    AlgorithmInterface(AlgorithmInterface&&) = delete;
    AlgorithmInterface& operator=(const AlgorithmInterface&) = delete;
    AlgorithmInterface& operator=(AlgorithmInterface&&) = delete;
    virtual ~AlgorithmInterface() = default;

protected:
    //-----------------------------------------------------------------------------
    //! Retrieves the agent id
    //!
    //! @return                       agent id
    //-----------------------------------------------------------------------------
    AgentInterface* GetAgent() const
    {
        return agent;
    }

private:
    AgentInterface *agent;                //!< Agent id
};

class ActionInterface : public UnrestrictedModelInterface
{
    using UnrestrictedModelInterface::UnrestrictedModelInterface;
};

class DynamicsInterface : public UnrestrictedModelInterface
{
    using UnrestrictedModelInterface::UnrestrictedModelInterface;
};

class SensorInterface : public UnrestrictedModelInterface
{
    using UnrestrictedModelInterface::UnrestrictedModelInterface;
};

class InitInterface : public UnrestrictedModelInterface
{
    using UnrestrictedModelInterface::UnrestrictedModelInterface;
};

//-----------------------------------------------------------------------------
//! Interface of restricted functionality
//-----------------------------------------------------------------------------
class ManipulationModelInterface : public ModelInterface
{
public:
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //-----------------------------------------------------------------------------
    ManipulationModelInterface(std::string componentName,
                             bool isInit,
                             int priority,
                             int offsetTime,
                             int responseTime,
                             int cycleTime,
                             core::EventNetworkInterface *eventNetwork,
                             AgentInterface *agent,
                             const CallbackInterface *callbacks) :
        ModelInterface(isInit, priority, offsetTime, responseTime, cycleTime),
        componentName(componentName),
        eventNetwork(eventNetwork),
        agent(agent),
        callbacks(callbacks)
    {}
    ManipulationModelInterface(const ManipulationModelInterface&) = delete;
    ManipulationModelInterface(ManipulationModelInterface&&) = delete;
    ManipulationModelInterface& operator=(const ManipulationModelInterface&) = delete;
    ManipulationModelInterface& operator=(ManipulationModelInterface&&) = delete;
    virtual ~ManipulationModelInterface() = default;

    //-----------------------------------------------------------------------------
    //! Retrieves id of this component.
    //!
    //! @return                       Component id of this model
    //-----------------------------------------------------------------------------
    std::string GetComponentName() const
    {
        return componentName;
    }

protected:
    //-----------------------------------------------------------------------------
    //! Retrieves the eventNetwork functionality of the framework
    //!
    //! @return                       EventNetwork functionality
    //-----------------------------------------------------------------------------
    const core::EventNetworkInterface *GetEventNetwork() const
    {
        return eventNetwork;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves the agent id
    //!
    //! @return                       agent id
    //-----------------------------------------------------------------------------
    AgentInterface* GetAgent() const
    {
        return agent;
    }

    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel,
             const char *file,
             int line,
             const std::string &message) const
    {
        if(callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:
    // Access to the following members is provided by the corresponding member
    // functions.
    std::string componentName;                      //!< Id of this component
    const core::EventNetworkInterface *eventNetwork;   //!< Reference to framework eventNetwork
    AgentInterface *agent;
    const CallbackInterface *callbacks;   //!< Reference to framework callbacks
};

class UnrestrictedEventModelInterface : public UnrestrictedModelInterface
{
public:
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     componentId    Corresponds to "id" of "Component"
    //! @param[in]     isInit         Corresponds to "init" of "Component"
    //! @param[in]     priority       Corresponds to "priority" of "Component"
    //! @param[in]     offsetTime     Corresponds to "offsetTime" of "Component"
    //! @param[in]     responseTime   Corresponds to "responseTime" of "Component"
    //! @param[in]     cycleTime      Corresponds to "cycleTime" of "Component"
    //! @param[in]     eventNetwork   Pointer to event network
    //! @param[in]     callbacks      Pointer to the callbacks
    //-----------------------------------------------------------------------------
    UnrestrictedEventModelInterface(std::string componentName,
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
                                    AgentInterface *agent,
                                    core::EventNetworkInterface * const eventNetwork):
        UnrestrictedModelInterface(componentName,
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
                                   agent),
        eventNetwork(eventNetwork)
    {}
    UnrestrictedEventModelInterface(const UnrestrictedEventModelInterface&) = delete;
    UnrestrictedEventModelInterface(UnrestrictedEventModelInterface&&) = delete;
    UnrestrictedEventModelInterface& operator=(const UnrestrictedEventModelInterface&) = delete;
    UnrestrictedEventModelInterface& operator=(UnrestrictedEventModelInterface&&) = delete;
    virtual ~UnrestrictedEventModelInterface() = default;

protected:
    //-----------------------------------------------------------------------------
    //! Retrieves the EventNetwork
    //!
    //! @return                       EventNetwork
    //-----------------------------------------------------------------------------
    core::EventNetworkInterface* GetEventNetwork() const
    {
        return eventNetwork;
    }

private:
    core::EventNetworkInterface * const eventNetwork;
};
