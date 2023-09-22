/********************************************************************************
 * Copyright (c) 2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include <list>
#include <map>
#include "include/agentFactoryInterface.h"
#include "include/agentBlueprintProviderInterface.h"
#include "include/scenarioInterface.h"
#include "common/spawnPointLibraryDefinitions.h"

namespace core
{
class SpawnPoint;

class SpawnPointNetworkInterface
{
public:
    SpawnPointNetworkInterface() = default;
    SpawnPointNetworkInterface(const SpawnPointNetworkInterface&) = delete;
    SpawnPointNetworkInterface(SpawnPointNetworkInterface&&) = delete;
    SpawnPointNetworkInterface& operator=(const SpawnPointNetworkInterface&) = delete;
    SpawnPointNetworkInterface& operator=(SpawnPointNetworkInterface&&) = delete;
    virtual ~SpawnPointNetworkInterface() = default;

    //-----------------------------------------------------------------------------
    //! Gets the spawn point instance library either from the already stored libraries
    //! or create a new instance (which is then also stored), then create a new spawn
    //! point using the provided parameters.
    //!
    //! @param[in]  libraryInfos         Information for the SpawnPointLibrary
    //! @param[in]  agentFactory        Factory for the agents
    //! @param[in]  agentBlueprintProvider        AgentBlueprintProvider
    //! @param[in]  sampler             Sampler
    //! @return                         true, if successful
    //-----------------------------------------------------------------------------
    virtual bool Instantiate(const SpawnPointLibraryInfoCollection& libraryInfos,
                             AgentFactoryInterface* agentFactory,
                             const AgentBlueprintProviderInterface* agentBlueprintProvider,
                             StochasticsInterface* stochastics,
                             const ScenarioInterface* scenario,
                             const std::optional<ProfileGroup>& spawnPointProfiles) = 0;

    virtual bool TriggerPreRunSpawnZones() = 0;

    virtual bool TriggerRuntimeSpawnPoints(const int timestamp) = 0;

    virtual std::vector<Agent*> ConsumeNewAgents() = 0;

    //-----------------------------------------------------------------------------
    //! Clears all spawnpoints.
    //-----------------------------------------------------------------------------
    virtual void Clear() = 0;
};

} //core
