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

#include "AgentNetwork.h"

#include <algorithm>
#include <string>

#include <QFile>

#include "AgentAdapter.h"
#include "WorldImplementation.h"

AgentNetwork::AgentNetwork(WorldImplementation *world, const CallbackInterface *callbacks) :
    world(world),
    callbacks(callbacks)
{
}

AgentNetwork::~AgentNetwork()
{
    Clear();
}

void AgentNetwork::Clear()
{
    updateQueue.clear();
    removeQueue.clear();

    agents.clear();
    agentsById.clear();

    removedAgentsPrevious.clear();
}

AgentAdapter &AgentNetwork::CreateAgent(OWL::Interfaces::MovingObject &movingObject,
                                        WorldInterface *world,
                                        const CallbackInterface *callbacks,
                                        const World::Localization::Localizer &localizer)
{
    auto &agent = agents.emplace_back(movingObject, world, callbacks, localizer);
    agentsById.insert({agent.GetId(), &agent});
    return agent;
}

AgentInterface *AgentNetwork::GetAgent(int id) const
{
    if (agentsById.find(id) == agentsById.end())
    {
        return nullptr;
    }

    return agentsById.at(id);
}

std::list<AgentAdapter> &AgentNetwork::GetAgents()
{
    return agents;
}

const std::vector<int> AgentNetwork::GetRemovedAgentsInPreviousTimestep()
{
    auto agents = std::move(removedAgentsPrevious);
    removedAgentsPrevious.clear();
    return agents;
}

void AgentNetwork::QueueAgentUpdate(std::function<void()> func)
{
    updateQueue.push_back(func);
}

void AgentNetwork::QueueAgentRemove(const AgentInterface *agent)
{
    removeQueue.push_back(agent->GetId());
}

std::list<AgentAdapter>::iterator AgentNetwork::RemoveAgent(const std::list<AgentAdapter>::iterator agent)
{
    removedAgentsPrevious.push_back(agent->GetId());
    agentsById.erase(agent->GetId());
    world->RemoveAgent(&(*agent));
    return agents.erase(agent);
}

void AgentNetwork::PublishGlobalData(Publisher publish)
{
    for (auto &agent : agents)
    {
        const openpass::type::EntityId agentId = agent.GetId();

        publish(agentId, "XPosition", agent.GetPositionX());
        publish(agentId, "YPosition", agent.GetPositionY());
        publish(agentId, "VelocityEgo", agent.GetVelocity().Length());
        publish(agentId, "AccelerationEgo", agent.GetAcceleration().Projection(agent.GetYaw()));
        publish(agentId, "YawAngle", agent.GetYaw());
        publish(agentId, "RollAngle", agent.GetRoll());
        publish(agentId, "YawRate", agent.GetYawRate());
        publish(agentId, "SteeringAngle", agent.GetSteeringWheelAngle());
        publish(agentId, "TotalDistanceTraveled", agent.GetDistanceTraveled());

        const auto &egoAgent = agent.GetEgoAgent();
        if (egoAgent.HasValidRoute())
        {
            publish(agentId, "PositionRoute", egoAgent.GetMainLocatePosition().value().roadPosition.s);
            publish(agentId, "TCoordinate", egoAgent.GetPositionLateral());
            publish(agentId, "Lane", egoAgent.GetMainLocatePosition().value().laneId);
            publish(agentId, "Road", egoAgent.GetRoadId());
            publish(agentId, "SecondaryLanes", agent.GetTouchedRoads().at(egoAgent.GetRoadId()).lanes);
        }
        else
        {
            publish(agentId, "PositionRoute", NAN);
            publish(agentId, "TCoordinate", NAN);
            publish(agentId, "Lane", NAN);
            publish(agentId, "Road", NAN);
            publish(agentId, "SecondaryLanes", std::vector<int>{});
            publish(agentId, "AgentInFront", NAN);
        }
    }
}

// udpate global data at occurrence of time step
void AgentNetwork::SyncGlobalData()
{
    for (const auto &func : updateQueue)
    {
        func();
    }
    updateQueue.clear();

    for (auto idToRemove : removeQueue)
    {
        auto agent = std::find_if(agents.begin(), agents.end(),
         [idToRemove](const auto& agent){return agent.GetId() == idToRemove;});
        RemoveAgent(agent);
    }
    removeQueue.clear();

    auto agent = agents.begin();
    while (agent != agents.end())
    {
        agent->Unlocate();

        if (!agent->Update())
        {
            LOG(CbkLogLevel::Warning, "Could not locate agent");
        }

        if (!agent->IsAgentInWorld())
        {
            agent = RemoveAgent(agent);
        }
        else
        {
            ++agent;
        }
    }
}
