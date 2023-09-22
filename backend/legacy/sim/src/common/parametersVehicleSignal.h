/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/


/**
 * \file  ParametersAgentMachineSignal.h
 * \brief Provides all vehicle parameters for the driver
 *
 */

#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "include/signalInterface.h"
#include "common/globalDefinitions.h"

/*!
 * \brief Provides all vehicle parameters for the driver
 */

class ParametersVehicleSignal : public SignalInterface
{
public:
    const std::string COMPONENTNAME = "ParametersAgentMachineSignal";

    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    ParametersVehicleSignal(VehicleModelParameters vehicleParameters):
        vehicleParameters(vehicleParameters)
    {
    }

    virtual ~ParametersVehicleSignal() = default;

    //-----------------------------------------------------------------------------
    //! Returns the content/payload of the signal as an std::string
    //!
    //! @return                       Content/payload of the signal as an std::string
    //-----------------------------------------------------------------------------
    virtual operator std::string() const
    {
        std::ostringstream stream;
        stream << COMPONENTNAME;
        return stream.str();
    }

    VehicleModelParameters vehicleParameters;
};
