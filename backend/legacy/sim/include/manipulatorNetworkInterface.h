/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <string>
#include <vector>

class ScenarioInterface;

namespace core {

class EventNetworkInterface;
class Manipulator;

class ManipulatorNetworkInterface
{
public:
    ManipulatorNetworkInterface() = default;
    ManipulatorNetworkInterface(const ManipulatorNetworkInterface&) = delete;
    ManipulatorNetworkInterface(ManipulatorNetworkInterface&&) = delete;
    ManipulatorNetworkInterface& operator=(const ManipulatorNetworkInterface&) = delete;
    ManipulatorNetworkInterface& operator=(ManipulatorNetworkInterface&&) = delete;
    virtual ~ManipulatorNetworkInterface() = default;

    virtual bool Instantiate(const std::string libraryPath,
                             const ScenarioInterface *scenario,
                             EventNetworkInterface* eventNetwork) = 0;

    virtual void Clear() = 0;

    virtual const std::vector<const Manipulator*> GetManipulators() const = 0;
};

}// namespace core
