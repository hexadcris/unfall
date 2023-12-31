/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Component.h
//! @brief This file contains the internal representation of a model component
//!        during a simulation run.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include "include/componentInterface.h"
#include "include/modelInterface.h"
#include "include/observationInterface.h"
#include "include/publisherInterface.h"

namespace core {

class Channel;
class ModelLibrary;
class Agent;
class ChannelBuffer;
class ObservationModule;

class Component : public ComponentInterface
{
public:
    Component(std::string name,
              Agent* agent) :
        agent(agent),
        name(name),
        modelLibrary(nullptr),
        implementation(nullptr)
    {}
    Component(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(const Component&) = delete;
    Component& operator=(Component&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, deletes all saved output channel buffers.
    //-----------------------------------------------------------------------------
    virtual ~Component() override;


    //-----------------------------------------------------------------------------
    //! Sets the model interface implementation.
    //!
    //! @param[in]     implementation       Model interface implementation to set
    //-----------------------------------------------------------------------------
    void SetImplementation(ModelInterface* implementation) override;

    void SetParameter(std::unique_ptr<ParameterInterface> parameter) override
    {
        this->parameter = std::move(parameter);
    }

    //-----------------------------------------------------------------------------
    //! Returns the stored agent.
    //!
    //! @return                             Stored agent
    //-----------------------------------------------------------------------------
    Agent* GetAgent() const override;

    //-----------------------------------------------------------------------------
    //! Adds the provided channel with the provided ID to the stored list of input
    //! channels.
    //!
    //! @param[in]     input                Input channel to add
    //! @param[in]     linkId               ID of the channel to add
    //! @return                             Flag if adding the channel was successful
    //-----------------------------------------------------------------------------
    bool AddInputLink(Channel* input, int linkId) override;

    //-----------------------------------------------------------------------------
    //! Adds the provided channel with the provided ID to the stored list of output
    //! channels.
    //!
    //! @param[in]     output               Output channel to add
    //! @param[in]     linkId               ID of the channel to add
    //! @return                             Flag if adding the channel was successful
    //-----------------------------------------------------------------------------
    bool AddOutputLink(Channel* output, int linkId) override;

    //-----------------------------------------------------------------------------
    //! Sets the observation modules map of the component
    //!
    //! @param[in]     observations          Observation modules (as interface)to add
    //-----------------------------------------------------------------------------
    virtual void SetObservations(const std::map<int, ObservationModule*>& observations) override;

    //-----------------------------------------------------------------------------
    //! Returns the map of IDs to stored input channels.
    //!
    //! @return                             Map of IDs to stored input channels
    //-----------------------------------------------------------------------------
    const std::map<int, Channel*>& GetInputLinks() const override;

    //-----------------------------------------------------------------------------
    //! Returns the map of IDs to stored output channels.
    //!
    //! @return                             Map of IDs to stored output channels
    //-----------------------------------------------------------------------------
    const std::map<int, Channel*>& GetOutputLinks() const override;

    //-----------------------------------------------------------------------------
    //! Returns the map of IDs to stored observation modules.
    //!
    //! @return                             Map of IDs to stored observation modules
    //-----------------------------------------------------------------------------
    const std::map<int, ObservationInterface*>& GetObservations() const override;

    //-----------------------------------------------------------------------------
    //! Calls the Trigger method on the stored model library with the stored model
    //! interface implementation and the time parameter provided.
    //!
    //! @param[in]     time                 Observation module (as interface)to add
    //! @return                             False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool TriggerCycle(int time) override;

    //-----------------------------------------------------------------------------
    //! Gets the data on the channel with the provided ID by updating the output
    //! via the model library using the stored model instance implementation and
    //! the provided time stamp, then setting this data on the channel with the
    //! provided ID.
    //!
    //! @param[in]     linkId               ID of the output channel
    //! @param[in]     time                 Time stamp for the output
    //! @return                             False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AcquireOutputData(int linkId, int time) override;

    //-----------------------------------------------------------------------------
    //! Releases the data on the channel with the provided ID.
    //!
    //! @param[in]     linkId               ID of the output channel
    //! @return                             False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool ReleaseOutputData(int linkId) override;

    //-----------------------------------------------------------------------------
    //! Updates the input data on the input channelwith the provided ID by updating
    //! the input via the model library using the stored model instance implementation
    //! and the provided time stamp.
    //!
    //! @param[in]     linkId               ID of the input channel
    //! @param[in]     time                 Time stamp for the output
    //! @return                             False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool UpdateInputData(int linkId, int time) override;

    //-----------------------------------------------------------------------------
    //! Creates a new channel buffer with the provided ID, store it in the stored
    //! list of output channel buffers and return it.
    //!
    //! @param[in]     linkId               ID of the channel output buffer to create
    //! @return                             Created output channel buffer
    //-----------------------------------------------------------------------------
    ChannelBuffer* CreateOutputBuffer(int linkId) override;

    //-----------------------------------------------------------------------------
    //! Insert the provided channel buffer with the provided ID in the list of stored
    //! input channel buffers.
    //!
    //! @param[in]     linkId               ID of the channel input buffer
    //! @param[in]     buffer               Channel input bufer to set
    //! @return                             False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool SetInputBuffer(int linkId, ChannelBuffer* buffer) override;

    //-----------------------------------------------------------------------------
    //! Returns if the stored model interface implementation is defined as init module.
    //!
    //! @return                             Returns false if error occured, otherwise
    //!                                     the flag if stored model interface
    //!                                     implementation is defined as init module
    //-----------------------------------------------------------------------------
    bool GetInit() const override;

    //-----------------------------------------------------------------------------
    //! Returns the priority of the stored model interface implementation.
    //!
    //! @return                             Returns std::numeric_limits<int>::min()
    //!                                     if error occured, otherwise the priority
    //!                                     of the stored model interface implementation
    //-----------------------------------------------------------------------------
    int GetPriority() const override;

    //-----------------------------------------------------------------------------
    //! Returns the offset time of the stored model interface implementation, i.e.
    //! the time at which this item is inserted for scheduling.
    //!
    //! @return                             Returns 0 if error occured, otherwise
    //!                                     the offset time of the stored model
    //!                                     interface implementation
    //-----------------------------------------------------------------------------
    int GetOffsetTime() const override;

    //-----------------------------------------------------------------------------
    //! Returns the response time of the stored model interface implementation.
    //!
    //! @return                             Returns 0 if error occured, otherwise
    //!                                     the respone time of the stored model
    //!                                     interface implementation
    //-----------------------------------------------------------------------------
    int GetResponseTime() const override;

    //-----------------------------------------------------------------------------
    //! Returns the cycle time of the stored model interface implementation, i.e.
    //! the time after which it is rescheduled.
    //!
    //! @return                             Returns 0 if error occured, otherwise
    //!                                     the cycle time of the stored model
    //!                                     interface implementation
    //-----------------------------------------------------------------------------
    int GetCycleTime() const override;

    //-----------------------------------------------------------------------------
    //! Set the provided model library as library to store.
    //!
    //! @param[in]     modelLibrary         Model library to set
    //! @return                             False if library is already set, true
    //!                                     otherwise
    //-----------------------------------------------------------------------------
    bool SetModelLibrary(ModelLibrary* modelLibrary) override;

    //-----------------------------------------------------------------------------
    //! Releases this component from the stored library.
    //!
    //! @return                             Result of releasing this component
    //-----------------------------------------------------------------------------
    bool ReleaseFromLibrary() override;

    //-----------------------------------------------------------------------------
    //! Returns the stored model interface instance.
    //!
    //! @return                             Stored model interface instance
    //-----------------------------------------------------------------------------
    ModelInterface* GetImplementation() const override;

    //-----------------------------------------------------------------------------
    //! Returns the stored ID.
    //!
    //! @return                             Stored ID
    //-----------------------------------------------------------------------------
    std::string GetName() const override;

private:
    Agent* agent;
    std::string name;
    std::map<int, Channel*> inputs;
    std::map<int, Channel*> outputs;
    std::map<int, ObservationInterface*> observations {};
    ModelLibrary* modelLibrary;
    ModelInterface* implementation;
    std::unique_ptr<ParameterInterface> parameter;
    std::unique_ptr<PublisherInterface> publisher;
    std::map<int, ChannelBuffer*> inputChannelBuffers;
    std::map<int, ChannelBuffer*> outputChannelBuffers;
};

} // namespace core


