/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2020 HLRS, University of Stuttgart
 *               2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
/**********************************************
***********************************************/

#include "include/callbackInterface.h"
#include "AlgorithmAFDM.h"
#include "src/followingDriverModel.h"
#include <exception>

const std::string Version = "0.0.1";
static const CallbackInterface *Callbacks = nullptr;

extern "C" ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_SHARED_EXPORT const std::string &OpenPASS_GetVersion()
{
    return Version;
}

extern "C" ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_SHARED_EXPORT ModelInterface *OpenPASS_CreateInstance(
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
        AgentInterface *agent,
        const CallbackInterface *callbacks)
{
    Callbacks = callbacks;

    try
    {
        return (ModelInterface*)(new (std::nothrow) AlgorithmAgentFollowingDriverModelImplementation(
                                     componentName,
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
                                     agent));
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return nullptr;
    }
    catch(...)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }
}

extern "C" ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_SHARED_EXPORT void OpenPASS_DestroyInstance(ModelInterface *implementation)
{
    delete (AlgorithmAgentFollowingDriverModelImplementation*)implementation;
}

extern "C" ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_SHARED_EXPORT bool OpenPASS_UpdateInput(
        ModelInterface *implementation,
        int localLinkId,
        const std::shared_ptr<SignalInterface const> &data,
        int time)
{
    try
    {
        implementation->UpdateInput(localLinkId, data, time);
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch(...)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_SHARED_EXPORT bool OpenPASS_UpdateOutput(
        ModelInterface *implementation,
        int localLinkId,
        std::shared_ptr<SignalInterface const> &data,
        int time)
{
    try
    {
        implementation->UpdateOutput(localLinkId, data, time);
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch(...)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" ALGORITHM_AGENTFOLLOWINGDRIVERMODEL_SHARED_EXPORT bool OpenPASS_Trigger(
        ModelInterface *implementation,
        int time)
{
    try
    {
        implementation->Trigger(time);
    }
    catch(const std::runtime_error &ex)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch(const std::exception& exstd)
    {
        if(Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, exstd.what());
        }

        return false;
    }
    catch(...)
    {
        if(Callbacks != nullptr)
        {
            std::exception_ptr p = std::current_exception();
#ifndef _MSC_VER
            const std::string exType = p ? p.__cxa_exception_type()->name() : "null";
#else
            const std::string exType =  "null";
#endif

            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception - type: " + exType);
        }

        return false;
    }

    return true;
}
