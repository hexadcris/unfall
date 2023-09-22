/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  StochasticsBinding.h
//! @brief This file contains the interface to the stochastics library.
//-----------------------------------------------------------------------------

#pragma once

#include <memory>

#include "common/opExport.h"
#include "common/callbacks.h"
#include "include/stochasticsInterface.h"

//class StochasticsInterface;

namespace core
{

class StochasticsLibrary;
class FrameworkConfig;

class SIMULATIONCOREEXPORT StochasticsBinding
{
public:
    StochasticsBinding(CallbackInterface *callbacks);
    StochasticsBinding(const StochasticsBinding&) = delete;
    StochasticsBinding(StochasticsBinding&&) = delete;
    StochasticsBinding& operator=(const StochasticsBinding&) = delete;
    StochasticsBinding& operator=(StochasticsBinding&&) = delete;
    virtual ~StochasticsBinding();

    //-----------------------------------------------------------------------------
    //! Gets the stochastics library and stores it,
    //! then creates a new stochasticsInterface of the library.
    //!
    //! @param[in]  libraryPath         Path of the library
    //! @return                         StochasticsInterface created from the library
    //-----------------------------------------------------------------------------
    StochasticsInterface *Instantiate(std::string libraryPath);

    //-----------------------------------------------------------------------------
    //! Unloads the stochasticsInterface binding by deleting the library.
    //-----------------------------------------------------------------------------
    void Unload();

private:
    std::shared_ptr<StochasticsLibrary> library = nullptr;
    CallbackInterface *callbacks {nullptr};
};

} // namespace core


