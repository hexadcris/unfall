/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "SpawnerScenario.h"

#include "agentBlueprint.h"
#include "common/RoutePlanning/RouteCalculation.h"
#include "framework/agentFactory.h"
#include "framework/sampler.h"
#include "include/agentInterface.h"
#include "include/worldInterface.h"

constexpr size_t MAX_DEPTH = 10; //! Limits search depths in case of cyclic network

SpawnerScenario::SpawnerScenario(const SpawnPointDependencies* dependencies,
                       const CallbackInterface* callbacks):
    SpawnPointInterface(dependencies->world, callbacks),
    dependencies(*dependencies)
{
    if (!dependencies->scenario)
    {
        LogError(std::string(COMPONENTNAME) + " received no scenario (required)");
    }
}

SpawnPointInterface::Agents SpawnerScenario::Trigger([[maybe_unused]]int time)
{
    Agents agents;

    for (const auto& entity : dependencies.scenario.value()->GetEntities())
    {
        if (entity.spawnInfo.spawning)
        {
            try
            {
                auto agentBlueprint = dependencies.agentBlueprintProvider->SampleAgent(entity.catalogReference.entryName, entity.assignedParameters);
                agentBlueprint.SetAgentProfileName(entity.catalogReference.entryName);
                agentBlueprint.SetAgentCategory(entity.name == "Ego"
                                                    ? AgentCategory::Ego
                                                    : AgentCategory::Scenario);
                agentBlueprint.SetObjectName(entity.name);
                agentBlueprint.SetSpawnParameter(CalculateSpawnParameter(entity,
                                                                         agentBlueprint.GetVehicleModelParameters()));

                core::Agent* newAgent = dependencies.agentFactory->AddAgent(&agentBlueprint);

                if(newAgent != nullptr)
                {
                    agents.emplace_back(newAgent);
                }
                else
                {
                    LogError(" failed to add agent successfully for entity "
                           + entity.name);
                }

            }
            catch(const std::runtime_error& error)
            {
                LogError("SpawnerScenario encountered an Error: " + std::string(error.what()));
            }
        }
    }

    return agents;
}

bool SpawnerScenario::ValidateOverlappingAgents(const STCoordinates &stCoordinates,
                                                             const openScenario::LanePosition& lanePosition,
                                                             const VehicleModelParameters& vehicleModelParameters)
{
    const Position position = GetWorld()->LaneCoord2WorldCoord(stCoordinates.s,
                                                         stCoordinates.t,
                                                         lanePosition.roadId,
                                                         lanePosition.laneId);

    if (GetWorld()->IntersectsWithAgent(position.xPos,
                                        position.yPos,
                                        position.yawAngle,
                                        vehicleModelParameters.boundingBoxDimensions.length,
                                        vehicleModelParameters.boundingBoxDimensions.width,
                                        vehicleModelParameters.boundingBoxDimensions.length * 0.5 + vehicleModelParameters.boundingBoxCenter.x))
    {
        return false;
    }

    return true;
}

bool SpawnerScenario::ValidateSTCoordinatesOnLane(const STCoordinates &stCoordinate,
                                                  const openScenario::LanePosition &lanePosition,
                                                  const double vehicleWidth)
{
    if (!GetWorld()->IsSValidOnLane(lanePosition.roadId,
                                    lanePosition.laneId,
                                    stCoordinate.s))
    {
        return false;
    }

    //Check if lane width > vehicle width
    double laneWidth = GetWorld()->GetLaneWidth(lanePosition.roadId,
                                                lanePosition.laneId,
                                                stCoordinate.s);
    if (vehicleWidth > laneWidth + std::abs(stCoordinate.t))
    {
        return false;
    }

    //Is vehicle completely inside the lane
    bool isVehicleCompletelyInLane = (laneWidth - vehicleWidth) * 0.5 >= std::abs(stCoordinate.t);
    if (isVehicleCompletelyInLane)
    {
        return true;
    }

    //Is vehicle more than 50 % on the lane
    double allowedRange = laneWidth * 0.5;
    bool outsideAllowedRange =  std::abs(stCoordinate.t) > allowedRange;
    if (outsideAllowedRange)
    {
        return false;
    }

    //Is vehicle partly on invalid lane
    bool isOffsetToLeftLane = (stCoordinate.t >= 0);
    int otherLaneId = isOffsetToLeftLane ? (lanePosition.laneId + 1) : (lanePosition.laneId - 1);

    if (!GetWorld()->IsSValidOnLane(lanePosition.roadId,
                                    otherLaneId,
                                    stCoordinate.s)) //other lane is invalid
    {
        return false;
    }

    return true;
}

STCoordinates SpawnerScenario::GetSTCoordinates(const openScenario::LanePosition &lanePosition,
                                                            const VehicleModelParameters &vehicleModelParameters)
{
    if(!lanePosition.stochasticS.has_value() && !lanePosition.stochasticOffset.has_value())
    {
        return {lanePosition.s, lanePosition.offset.value_or(0.0)};
    }
    else
    {
        for (int trial = 0; trial < NUMBER_OF_TRIALS_STOCHASTIC; trial++)
        {
            STCoordinates stCoordinates;

            if(lanePosition.stochasticS.has_value())
            {
                stCoordinates.s = CalculateAttributeValue(lanePosition.stochasticS.value());
            }
            else
            {
                stCoordinates.s = lanePosition.s;
            }

            if(lanePosition.stochasticOffset.has_value())
            {
                stCoordinates.t = CalculateAttributeValue(lanePosition.stochasticOffset.value());
            }
            else
            {
                stCoordinates.t = lanePosition.offset.value_or(0.0);
            }


            if (ValidateOverlappingAgents(stCoordinates,
                                                             lanePosition,
                                                             vehicleModelParameters)
                    && ValidateSTCoordinatesOnLane(stCoordinates,
                                                             lanePosition,
                                                             vehicleModelParameters.boundingBoxDimensions.width))
            {
                return stCoordinates;
            }
        }

        LogError("Sampling valid stochastic spawn distance failed.");
    }
}

SpawnParameter SpawnerScenario::CalculateSpawnParameter(const ScenarioEntity &entity,
                                                           const VehicleModelParameters &vehicleModelParameters)
{
    const auto& spawnInfo = entity.spawnInfo;
    SpawnParameter spawnParameter;

    // Define position and orientation
    // Lane Spawning
    if(std::holds_alternative<openScenario::LanePosition>(spawnInfo.position))
    {
        const auto &lanePosition = std::get<openScenario::LanePosition>(spawnInfo.position);

        const auto &stCoordinate = GetSTCoordinates(lanePosition, vehicleModelParameters);
        const auto &pos = GetWorld()->LaneCoord2WorldCoord(stCoordinate.s,
                                                           stCoordinate.t,
                                                           lanePosition.roadId,
                                                           lanePosition.laneId);

        spawnParameter.positionX = pos.xPos;
        spawnParameter.positionY = pos.yPos;
        spawnParameter.yawAngle = pos.yawAngle;

        if(lanePosition.orientation.has_value())
        {
            spawnParameter.yawAngle += lanePosition.orientation.value().h.value_or(0.0);
        }
    }
    // World Spawning
    else if (std::holds_alternative<openScenario::WorldPosition>(spawnInfo.position))
    {
        const auto &worldPosition = std::get<openScenario::WorldPosition>(spawnInfo.position);
        spawnParameter.positionX = worldPosition.x;
        spawnParameter.positionY = worldPosition.y;
        spawnParameter.yawAngle = worldPosition.h.value_or(0.0);
    }
    else {
        LogError("This Spawner only supports Lane- & WorldPositions.");
    }

    if(!IsInsideWorld(spawnParameter))
    {
        LogError("Agent \"" + entity.name + "\" is outside world");
    }

    // Define velocity
    if(spawnInfo.stochasticVelocity.has_value())
    {
        spawnParameter.velocity = CalculateAttributeValue(spawnInfo.stochasticVelocity.value());
    }
    else
    {
        spawnParameter.velocity = spawnInfo.velocity;
    }

    // Define acceleration
    if(spawnInfo.stochasticAcceleration.has_value())
    {
        spawnParameter.acceleration = CalculateAttributeValue(spawnInfo.stochasticAcceleration.value());
    }
    else
    {
        spawnParameter.acceleration = spawnInfo.acceleration.value_or(0.0);
    }

    if (spawnInfo.route.has_value())
    {
        spawnParameter.route = GetPredefinedRoute(spawnInfo.route.value());
    }
    else
    {
        spawnParameter.route = GetRandomRoute(spawnParameter);
    }

    return spawnParameter;
}

double SpawnerScenario::CalculateAttributeValue(const openScenario::StochasticAttribute &attribute)
{
    openpass::parameter::NormalDistribution distribution{attribute.mean, attribute.stdDeviation, attribute.lowerBoundary, attribute.upperBoundary};
    return Sampler::RollForStochasticAttribute(distribution,
                                               dependencies.stochastics);
}

Route SpawnerScenario::GetPredefinedRoute(const std::vector<RouteElement>& roads)
{
    if (roads.empty())
    {
        LogError("Route is empty");
    }

    auto [roadGraph , root] = GetWorld()->GetRoadGraph(roads.front(), std::max(MAX_DEPTH, roads.size()));

    RoadGraphVertex target = root;
    for (auto road = roads.begin() + 1; road != roads.end(); ++road)
    {
        bool foundSuccessor = false;
        for (auto [successor, successorsEnd] = adjacent_vertices(target, roadGraph); successor != successorsEnd; ++successor)
        {
            if (get(RouteElement(), roadGraph, *successor) == *road)
            {
                foundSuccessor = true;
                target = *successor;
                break;
            }
        }
        if (!foundSuccessor)
        {
            LogError("Invalid route defined in Scenario. Node " + road->roadId + " not reachable");
        }
    }

    return Route{roadGraph, root, target};
}

Route SpawnerScenario::GetRandomRoute(const SpawnParameter& spawnParameter)
{
    const auto& roadPositions = GetWorld()->WorldCoord2LaneCoord(spawnParameter.positionX, spawnParameter.positionY, spawnParameter.yawAngle);
    auto [roadGraph, root] = GetWorld()->GetRoadGraph(CommonHelper::GetRoadWithLowestHeading(roadPositions, *GetWorld()), MAX_DEPTH);
    std::map<RoadGraph::edge_descriptor, double> weights = GetWorld()->GetEdgeWeights(roadGraph);
    auto target = RouteCalculation::SampleRoute(roadGraph, root, weights, *dependencies
            .stochastics);

    return Route{roadGraph, root, target};
}

bool SpawnerScenario::IsInsideWorld(const SpawnParameter &spawnParameter)
{
    const auto& roadPositions = GetWorld()->WorldCoord2LaneCoord(spawnParameter.positionX, spawnParameter.positionY, spawnParameter.yawAngle);
    return !roadPositions.empty();
}


[[noreturn]] void SpawnerScenario::LogError(const std::string& message)
{
    std::stringstream log;
    log.str(std::string());
    log << COMPONENTNAME << " " << message;
    LOG(CbkLogLevel::Error, log.str());
    throw std::runtime_error(log.str());
}
