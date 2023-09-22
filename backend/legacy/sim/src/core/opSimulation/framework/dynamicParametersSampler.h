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
#include <unordered_map>
#include "include/stochasticsInterface.h"
#include "include/profilesInterface.h"
#include "common/sensorDefinitions.h"

class DynamicParameterSampler;

struct DynamicParameters
{
    friend DynamicParameterSampler;
    std::unordered_map<int, double> sensorLatencies;

    static DynamicParameterSampler make(StochasticsInterface& stochastics,
                                        std::string &vehicleProfileName,
                                        const ProfilesInterface* profiles);

    //For testing;
    static DynamicParameters empty();
private:
    DynamicParameters() = default;
};

class DynamicParameterSampler
{
public:
    DynamicParameterSampler(StochasticsInterface& stochastics,
                            std::string &vehicleProfileName,
                            const ProfilesInterface* profiles);

    operator DynamicParameters&&()
    {
        return std::move(dynamicParameter);
    }

    DynamicParameterSampler& SampleSensorLatencies();

private:
    DynamicParameters dynamicParameter;
    StochasticsInterface& stochastics;
    std::string& vehicleProfileName;
    const ProfilesInterface *profiles;
};
