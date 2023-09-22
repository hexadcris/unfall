/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "systemConfig.h"

const std::map<int, std::shared_ptr<core::AgentTypeInterface>> &Configuration::SystemConfig::GetSystems() const
{
    return systems;
}

void Configuration::SystemConfig::SetSystems(std::map<int, std::shared_ptr<core::AgentTypeInterface>> systems)
{
    this->systems = systems;
}

bool Configuration::SystemConfig::AddSystem(const int& systemId, const std::shared_ptr<core::AgentTypeInterface> system)
{
    return systems.insert({systemId, system}).second;
}

void Configuration::SystemConfig::AddModelParameters(std::shared_ptr<ParameterInterface> modelParameters)
{
    parameters.push_back(modelParameters);
}
