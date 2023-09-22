/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  RunResult.h
//! @brief This file represents the result of a simulation run.
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <map>
#include <tuple>
#include "include/worldInterface.h"
#include "include/observationInterface.h"

namespace core
{

//-----------------------------------------------------------------------------
/** \brief stores run results for output data
*
* 	\ingroup opSimulation
*/
//-----------------------------------------------------------------------------
class RunResult : public RunResultInterface
{
public:
    RunResult() = default;
    RunResult(const RunResult&) = delete;
    RunResult(RunResult&&) = delete;
    RunResult& operator=(const RunResult&) = delete;
    RunResult& operator=(RunResult&&) = delete;
    virtual ~RunResult() = default;

    /*!
    * \brief AddCollisionId
    *
    * \details add agent id to list of collided agents
    *
    * @param[in]     int  id of collided agent
    */
    void AddCollisionId(const int agentId);

    void SetTimeOver()
    {
        result |= maskTimeOver;
    }

    virtual void SetEndCondition()
    {
        result |= maskEndCondition;
    }

    virtual const std::vector<int> *GetCollisionIds() const
    {
        return &collisionIds;
    }

    virtual bool IsCollision() const
    {
        return 0 != (result & maskCollision);
    }

    virtual bool IsTimeOver() const
    {
        return 0 != (result & maskTimeOver);
    }

    virtual bool IsEndCondition() const
    {
        return 0 != (result & maskEndCondition);
    }

private:
    const std::uint32_t maskCollision = 0x1;
    const std::uint32_t maskTimeOver = 0x2;
    const std::uint32_t maskEndCondition = 0x4;

    std::uint32_t result = 0;
    std::vector<int> collisionIds;
};

} // namespace core


