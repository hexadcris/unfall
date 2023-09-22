/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2016-2017 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  parameters_vehicle.h
*	@brief This file provides the exported methods.
*
*   This file provides the exported methods which are available outside of the library. */
//-----------------------------------------------------------------------------

#pragma once

#include <QtCore/qglobal.h>

#if defined(PARAMETERS_VEHICLE_LIBRARY)
#  define PARAMETERS_VEHICLE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define PARAMETERS_VEHICLE_SHARED_EXPORT Q_DECL_IMPORT
#endif

#include "include/modelInterface.h"


