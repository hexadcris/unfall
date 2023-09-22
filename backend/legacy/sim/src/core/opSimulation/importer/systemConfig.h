/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once
#include <vector>
#include "modelElements/parameters.h"
#include "include/systemConfigInterface.h"
#include "include/agentTypeInterface.h"

namespace Configuration
{

class SystemConfig : public SystemConfigInterface
{
public:
    virtual const std::map<int, std::shared_ptr<core::AgentTypeInterface>>& GetSystems() const override;
    virtual void SetSystems(std::map<int, std::shared_ptr<core::AgentTypeInterface>> systems) override;
    virtual bool AddSystem(const int& systemId, const std::shared_ptr< core::AgentTypeInterface> system) override;
    virtual void AddModelParameters(std::shared_ptr<ParameterInterface> modelParameters) override;

private:
    std::map<int, std::shared_ptr<core::AgentTypeInterface>> systems {};
    std::vector<std::shared_ptr<ParameterInterface>> parameters {};
};

}

