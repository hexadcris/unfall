/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "transformLaneChange.h"

#include "common/openScenarioDefinitions.h"
#include "include/egoAgentInterface.h"
#include "oscActionsCalculation.h"

namespace openScenario::transformation {

std::shared_ptr<TrajectorySignal> LaneChange::ConvertToSignal(const openpass::events::LaneChangeEvent &event, WorldInterface *world, AgentInterface *agent, int cycleTime)
{
    const auto &laneChange = event.laneChange;

    double deltaS;
    double deltaTime;
    if (laneChange.dynamicsType == openScenario::LaneChangeParameter::DynamicsType::Distance)
    {
        deltaS = laneChange.dynamicsTarget;
        deltaTime = deltaS / agent->GetVelocity().Length();
    }
    else //LaneChange::DynamicsType::Time
    {
        deltaS = laneChange.dynamicsTarget * agent->GetVelocity().Length();
        deltaTime = laneChange.dynamicsTarget;
    }

    const auto &roadId = agent->GetEgoAgent().GetRoadId();
    const auto& agentReferencePoint = agent->GetRoadPosition(ObjectPointPredefined::Reference);
    if (agentReferencePoint.count(roadId) == 0)
    {
        throw std::runtime_error("Could not calculate LaneChange trajectory. Reference point is not on route.");
    }
    const auto currentLaneId = agentReferencePoint.at(roadId).laneId;
    const auto currentS = agentReferencePoint.at(roadId).roadPosition.s;
    int targetLaneId;
    if (laneChange.type == openScenario::LaneChangeParameter::Type::Absolute)
    {
        targetLaneId = laneChange.value;
    }
    else
    {
        const auto object = world->GetAgentByName(laneChange.object);
        const auto& objectReferencePoint = object->GetRoadPosition(ObjectPointPredefined::Reference);
        if (objectReferencePoint.count(roadId) == 0)
        {
            throw std::runtime_error("Could not calculate LaneChange trajectory. Reference agent is not on same road as acting agent->");
        }
        targetLaneId = objectReferencePoint.at(roadId).laneId + laneChange.value;
    }
    double deltaT;
    if (targetLaneId > currentLaneId) //Change to left
    {
        deltaT = 0.5 * world->GetLaneWidth(roadId, currentLaneId, currentS) + 0.5 * world->GetLaneWidth(roadId, targetLaneId, currentS);
        for (int laneId = currentLaneId + 1; laneId < targetLaneId; ++laneId)
        {
            deltaT += world->GetLaneWidth(roadId, laneId, currentS);
        }
    }
    else if (targetLaneId < currentLaneId) //Change to right
    {
        deltaT = -0.5 * world->GetLaneWidth(roadId, currentLaneId, currentS) - 0.5 * world->GetLaneWidth(roadId, targetLaneId, currentS);
        for (int laneId = currentLaneId - 1; laneId > targetLaneId; --laneId)
        {
            deltaT -= world->GetLaneWidth(roadId, laneId, currentS);
        }
    }
    else
    {
        return std::make_shared<TrajectorySignal>();
    }

    GlobalRoadPosition startPosition{roadId,
                                     currentLaneId,
                                     currentS,
                                     agentReferencePoint.at(roadId).roadPosition.t,
                                     agentReferencePoint.at(roadId).roadPosition.hdg};

    OscActionsCalculation calculation(world);
    auto trajectory = calculation.CalculateSinusiodalLaneChange(deltaS, deltaT, deltaTime, cycleTime / 1000.0, startPosition, 0.0);

    return std::make_shared<TrajectorySignal>(ComponentState::Acting, trajectory);
}
} // namespace openScenario::transformation
