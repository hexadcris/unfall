/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/commonTools.h"
#include "common/worldDefinitions.h"
#include "include/agentInterface.h"
#include "include/egoAgentInterface.h"
#include "include/stochasticsInterface.h"
#include "include/worldInterface.h"

namespace RouteCalculation
{
//! This function is called by FilterRoadGraphByStartPosition and adds the next vertex to the tree,
//! then recursively calls itself for all successors of the current element
//! This function is not intended to be called by any other function then FilterRoadGraphByStartPosition
//!
//! \param roadGraph            entire road network
//! \param current              vertex to add in this step
//! \param maxDepth             maximum depth of subtree of this node
//! \param inDrivingDirection   build tree in or against driving direction
//! \param filteredGraph        tree that is currently being built
//! \return newly added vertex
static RoadGraphVertex FilterRoadGraphByStartPositionRecursive(const RoadGraph& roadGraph, RoadGraphVertex current, int maxDepth, bool inDrivingDirection, RoadGraph& filteredGraph)
{
    const auto& routeElement = get(RouteElement(), roadGraph, current);
    auto newVertex = add_vertex(routeElement, filteredGraph);
    if (maxDepth > 1)
    {
        if (inDrivingDirection)
        {
            for (auto [successor, successorsEnd] = adjacent_vertices(current, roadGraph); successor != successorsEnd; successor++)
            {
                auto successorVertex = FilterRoadGraphByStartPositionRecursive(roadGraph, *successor, maxDepth - 1, inDrivingDirection, filteredGraph);
                add_edge(newVertex, successorVertex, filteredGraph);
            }
        }
        else
        {
            for (auto [predecessor, predecessorsEnd] = inv_adjacent_vertices(current, roadGraph); predecessor != predecessorsEnd; predecessor++)
            {
                auto successorVertex = FilterRoadGraphByStartPositionRecursive(roadGraph, *predecessor, maxDepth - 1, inDrivingDirection, filteredGraph);
                add_edge(newVertex, successorVertex, filteredGraph);
            }
        }
    }
    return newVertex;
}

//! Returns the road graph as a tree with defined maximum depth relative to a given start position.
//! The same route element can appear multiple times in the result, if there are multiple paths to it from the start position
//!
//! \param roadGraph    entire road network
//! \param start        start position in the network
//! \param maxDepth     maximum depth of resulting tree
//! \param inDrivingDirection   build tree in or against driving direction
//! \return road network as tree with given root
static std::pair<RoadGraph, RoadGraphVertex> FilterRoadGraphByStartPosition(const RoadGraph& roadGraph, RoadGraphVertex start, int maxDepth, bool inDrivingDirection)
{
    RoadGraph filteredGraph;
    auto root = FilterRoadGraphByStartPositionRecursive(roadGraph, start, maxDepth, inDrivingDirection, filteredGraph);
    return {filteredGraph, root};
}

//! Random draws a target leaf in the given road graph tree based on the propability of each edge
//!
//! \param roadGraph    tree of road network starting at the agents current position
//! \param root         root vertex of the roadGraph
//! \param weights      weight map of all edges of the graph
//! \param stochastics  stochastics module
//! \return sampled target vertex
static RoadGraphVertex SampleRoute(const RoadGraph& roadGraph,
                                   RoadGraphVertex root,
                                   const std::map<RoadGraphEdge, double>& weights,
                                   StochasticsInterface& stochastics)
{
    auto current = root;
    bool reachedEnd = false;
    while (!reachedEnd)
    {
        if (out_degree(current, roadGraph) == 0)
        {
            reachedEnd = true;
            break;
        }
        double weightSum = 0.0;
        for (auto [successor, successorEnd] = out_edges(current, roadGraph); successor != successorEnd; successor++)
        {
            weightSum += weights.at(*successor);
        }
        auto roll = stochastics.GetUniformDistributed(0, weightSum);
        double probalitySum = 0.0;
        for (auto [successor, successorEnd] = out_edges(current, roadGraph); successor != successorEnd; successor++)
        {
            probalitySum += weights.at(*successor);
            if (roll <= probalitySum)
            {
                current = target(*successor, roadGraph);
                break;
            }
        }
    }
    return current;
}

static std::optional<RoadGraphVertex> FindPosition(const GlobalRoadPositions& position,const RoadGraph& roadGraph, const RoadGraphVertex start)
{
    const auto roadId = get(RouteElement(), roadGraph, start).roadId;
    if (position.find(roadId) != position.cend())
    {
        return start;
    }
    for (auto [successor, successorsEnd] = adjacent_vertices(start, roadGraph); successor != successorsEnd; successor++)
    {
        auto result = FindPosition(position, roadGraph, *successor);
        if (result.has_value())
        {
            return result;
        }
    }
    return std::nullopt;
}

static bool GetNewRoute(AgentInterface &agent, const WorldInterface &world, StochasticsInterface &stochastics, const std::optional<RouteElement> &previousReferencePointRoad)
{
    if (agent.GetRoadPosition(ObjectPointPredefined::FrontCenter).empty())
    {
        return false;
    }
    constexpr int maxDepth = 10;
    std::vector<std::pair<RoadGraph, RoadGraphVertex>> roadGraphs{};
    if (previousReferencePointRoad.has_value())
    {
        roadGraphs.push_back(world.GetRoadGraph(previousReferencePointRoad.value(), maxDepth));
    }
    else
    {
        for (const auto &[roadId, roadPosition] : agent.GetRoadPosition(ObjectPointPredefined::Reference))
        {
            bool direction = std::abs(roadPosition.roadPosition.hdg) <= M_PI_2;
            if (world.IsDirectionalRoadExisting(roadId, direction))
            {
                roadGraphs.push_back(world.GetRoadGraph({roadId, direction}, maxDepth));
            }
        }
    }
    std::optional<RoadGraph> selectedRoadGraph;
    RoadGraphVertex selectedStart;
    RoadGraphVertex selectedMainLocatorVertex;
    double minHeading{std::numeric_limits<double>::max()};
    for (const auto&[roadGraph, start] : roadGraphs)
    {
        const auto mainLocatorVertex = FindPosition(agent.GetRoadPosition(ObjectPointPredefined::FrontCenter), roadGraph, start);
        if (mainLocatorVertex.has_value())
        {
            const auto mainLocateRouteElement = get(RouteElement(), roadGraph, mainLocatorVertex.value());
            auto absHeading = std::abs(CommonHelper::SetAngleToValidRange(agent.GetRoadPosition(ObjectPointPredefined::FrontCenter).at(mainLocateRouteElement.roadId).roadPosition.hdg + mainLocateRouteElement.inOdDirection ? 0 : M_PI));
            if (absHeading < minHeading)
            {
                minHeading = absHeading;
                selectedRoadGraph = roadGraph;
                selectedStart = start;
                selectedMainLocatorVertex = mainLocatorVertex.value();
            }
        }
    }
    if (!selectedRoadGraph.has_value())
    {
        const auto [roadGraph, start] = world.GetRoadGraph(CommonHelper::GetRoadWithLowestHeading(agent.GetRoadPosition(ObjectPointPredefined::FrontCenter), world), maxDepth);
        selectedRoadGraph = roadGraph;
        selectedStart = start;
        selectedMainLocatorVertex = start;
    }
    std::map<RoadGraph::edge_descriptor, double> weights = world.GetEdgeWeights(selectedRoadGraph.value());
    auto target = RouteCalculation::SampleRoute(selectedRoadGraph.value(), selectedMainLocatorVertex, weights, stochastics);
    agent.GetEgoAgent().SetRoadGraph(std::move(selectedRoadGraph.value()), selectedStart, target);
    return true;
}
}
