/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "vehicleModels.h"

namespace Configuration
{

VehicleModels::VehicleModels()
{
}

VehicleModels::~VehicleModels()
{
}

const VehicleModelMap &VehicleModels::GetVehicleModelMap() const
{
    return vehicleModelMap;
}

void VehicleModels::AddVehicleModel(const std::string& name, const ParametrizedVehicleModelParameters& vehicleModel)
{
    vehicleModelMap.insert({name, vehicleModel});
}

VehicleModelParameters VehicleModels::GetVehicleModel(std::string vehicleModelType, const openScenario::Parameters& parameters) const
{
    auto find_result = vehicleModelMap.find(vehicleModelType);
    if (find_result == vehicleModelMap.cend())
    {
        throw std::runtime_error("No VehicleModel with name \"" + vehicleModelType + "\" defined in VehicleCatalog and PedestrianCatalog");
    }
    return find_result->second.Get(parameters);
}

} //namespace Configuration
