/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/RoutePlanning/RouteCalculation.h"
#include "fakeWorld.h"
#include "fakeAgent.h"
#include "fakeEgoAgent.h"
#include "fakeStochastics.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::VariantWith;

TEST(RouteCalculation, NoPreviousRoad_BothPointsOnSameRoad)
{
    FakeAgent agent;
    FakeEgoAgent egoAgent;
    FakeWorld world;
    FakeStochastics stochastics;

    ON_CALL(agent, GetEgoAgent()).WillByDefault(ReturnRef(egoAgent));
    GlobalRoadPositions referencePoint = {{"RoadA", GlobalRoadPosition{"RoadA", -2, 10, 0, 0.2}},{"RoadB", GlobalRoadPosition{"RoadB", -2, 10, 0, 0.1}}};
    GlobalRoadPositions mainLocatePoint = {{"RoadA", GlobalRoadPosition{"RoadA", -2, 10, 0, 0}}};
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::Reference))).WillByDefault(ReturnRef(referencePoint));
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::FrontCenter))).WillByDefault(ReturnRef(mainLocatePoint));

    RoadGraph roadGraphA;
    RoadGraphVertex nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraphA);
    RoadGraphVertex nodeC = add_vertex(RouteElement{"RoadC", true}, roadGraphA);
    add_edge(nodeA, nodeC, roadGraphA);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadA", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphA, nodeA)));
    RoadGraph roadGraphB;
    RoadGraphVertex nodeB = add_vertex(RouteElement{"RoadB", true}, roadGraphB);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadB", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphB, nodeB)));
    ON_CALL(world, GetEdgeWeights(_)).WillByDefault([](const RoadGraph& roadGraph)
    {
    std::map<RoadGraphEdge, double> weights;
    for (auto [edge, edgesEnd] = edges(roadGraph); edge != edgesEnd; ++edge)
    {
        weights[*edge] = 1;
    }
    return weights;
    });
    ON_CALL(world, IsDirectionalRoadExisting(_, _)).WillByDefault(Return(true));

    RoadGraph resultGraph;
    RoadGraphVertex resultCurrent;
    RoadGraphVertex resultTarget;
    ON_CALL(egoAgent, SetRoadGraph(_,_,_)).WillByDefault([&](const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target)
    {
        resultGraph = roadGraph;
        resultCurrent = current;
        resultTarget = target;
    });

    RouteCalculation::GetNewRoute(agent, world, stochastics, std::nullopt);

    ASSERT_THAT(resultGraph.vertex_set(), Eq(roadGraphA.vertex_set()));
    ASSERT_THAT(resultCurrent, Eq(nodeA));
    ASSERT_THAT(resultTarget, Eq(nodeC));
}

TEST(RouteCalculation, WithPreviousRoad_BothPointsOnSameRoad)
{
    FakeAgent agent;
    FakeEgoAgent egoAgent;
    FakeWorld world;
    FakeStochastics stochastics;

    ON_CALL(agent, GetEgoAgent()).WillByDefault(ReturnRef(egoAgent));
    GlobalRoadPositions referencePoint = {{"RoadA", GlobalRoadPosition{"RoadA", -2, 10, 0, 0.2}},{"RoadB", GlobalRoadPosition{"RoadB", -2, 10, 0, 0.1}}};
    GlobalRoadPositions mainLocatePoint = {{"RoadA", GlobalRoadPosition{"RoadA", -2, 10, 0, 0}}};
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::Reference))).WillByDefault(ReturnRef(referencePoint));
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::FrontCenter))).WillByDefault(ReturnRef(mainLocatePoint));

    RoadGraph roadGraphA;
    RoadGraphVertex nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraphA);
    RoadGraphVertex nodeC = add_vertex(RouteElement{"RoadC", true}, roadGraphA);
    add_edge(nodeA, nodeC, roadGraphA);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadA", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphA, nodeA)));
    ON_CALL(world, GetEdgeWeights(_)).WillByDefault([](const RoadGraph& roadGraph)
    {
    std::map<RoadGraphEdge, double> weights;
    for (auto [edge, edgesEnd] = edges(roadGraph); edge != edgesEnd; ++edge)
    {
        weights[*edge] = 1;
    }
    return weights;
    });

    RoadGraph resultGraph;
    RoadGraphVertex resultCurrent;
    RoadGraphVertex resultTarget;
    ON_CALL(egoAgent, SetRoadGraph(_,_,_)).WillByDefault([&](const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target)
    {
        resultGraph = roadGraph;
        resultCurrent = current;
        resultTarget = target;
    });

    RouteCalculation::GetNewRoute(agent, world, stochastics, RouteElement{"RoadA", true});

    ASSERT_THAT(resultGraph.vertex_set(), Eq(roadGraphA.vertex_set()));
    ASSERT_THAT(resultCurrent, Eq(nodeA));
    ASSERT_THAT(resultTarget, Eq(nodeC));
}

TEST(RouteCalculation, NoPreviousRoad_MainLocatorOnNextRoad)
{
    FakeAgent agent;
    FakeEgoAgent egoAgent;
    FakeWorld world;
    FakeStochastics stochastics;

    ON_CALL(agent, GetEgoAgent()).WillByDefault(ReturnRef(egoAgent));
    GlobalRoadPositions referencePoint = {{"RoadA", GlobalRoadPosition{"RoadA", -2, 10, 0, 0.2}},{"RoadB", GlobalRoadPosition{"RoadB", -2, 10, 0, 0.1}}};
    GlobalRoadPositions mainLocatePoint = {{"RoadD", GlobalRoadPosition{"RoadD", -2, 10, 0, 0}}};
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::Reference))).WillByDefault(ReturnRef(referencePoint));
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::FrontCenter))).WillByDefault(ReturnRef(mainLocatePoint));

    RoadGraph roadGraphA;
    RoadGraphVertex nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraphA);
    RoadGraphVertex nodeC = add_vertex(RouteElement{"RoadC", true}, roadGraphA);
    RoadGraphVertex nodeD = add_vertex(RouteElement{"RoadD", true}, roadGraphA);
    RoadGraphVertex nodeE = add_vertex(RouteElement{"RoadE", true}, roadGraphA);
    add_edge(nodeA, nodeC, roadGraphA);
    add_edge(nodeA, nodeD, roadGraphA);
    add_edge(nodeD, nodeE, roadGraphA);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadA", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphA, nodeA)));
    RoadGraph roadGraphB;
    RoadGraphVertex nodeB = add_vertex(RouteElement{"RoadB", true}, roadGraphB);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadB", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphB, nodeB)));
    ON_CALL(world, GetEdgeWeights(_)).WillByDefault([](const RoadGraph& roadGraph)
    {
    std::map<RoadGraphEdge, double> weights;
    for (auto [edge, edgesEnd] = edges(roadGraph); edge != edgesEnd; ++edge)
    {
        weights[*edge] = 1;
    }
    return weights;
    });
    ON_CALL(world, IsDirectionalRoadExisting(_, _)).WillByDefault(Return(true));

    RoadGraph resultGraph;
    RoadGraphVertex resultCurrent;
    RoadGraphVertex resultTarget;
    ON_CALL(egoAgent, SetRoadGraph(_,_,_)).WillByDefault([&](const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target)
    {
        resultGraph = roadGraph;
        resultCurrent = current;
        resultTarget = target;
    });

    RouteCalculation::GetNewRoute(agent, world, stochastics, std::nullopt);

    ASSERT_THAT(resultGraph.vertex_set(), Eq(roadGraphA.vertex_set()));
    ASSERT_THAT(resultCurrent, Eq(nodeA));
    ASSERT_THAT(resultTarget, Eq(nodeE));
}

TEST(RouteCalculation, WithPreviousRoad_MainLocatorOnNextRoad)
{
    FakeAgent agent;
    FakeEgoAgent egoAgent;
    FakeWorld world;
    FakeStochastics stochastics;

    ON_CALL(agent, GetEgoAgent()).WillByDefault(ReturnRef(egoAgent));
    GlobalRoadPositions referencePoint = {{"RoadA", GlobalRoadPosition{"RoadA", -2, 10, 0, 0.2}},{"RoadB", GlobalRoadPosition{"RoadB", -2, 10, 0, 0.1}}};
    GlobalRoadPositions mainLocatePoint = {{"RoadD", GlobalRoadPosition{"RoadD", -2, 10, 0, 0}}};
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::Reference))).WillByDefault(ReturnRef(referencePoint));
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::FrontCenter))).WillByDefault(ReturnRef(mainLocatePoint));

    RoadGraph roadGraphA;
    RoadGraphVertex nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraphA);
    RoadGraphVertex nodeC = add_vertex(RouteElement{"RoadC", true}, roadGraphA);
    RoadGraphVertex nodeD = add_vertex(RouteElement{"RoadD", true}, roadGraphA);
    RoadGraphVertex nodeE = add_vertex(RouteElement{"RoadE", true}, roadGraphA);
    add_edge(nodeA, nodeC, roadGraphA);
    add_edge(nodeA, nodeD, roadGraphA);
    add_edge(nodeD, nodeE, roadGraphA);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadA", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphA, nodeA)));
    ON_CALL(world, GetEdgeWeights(_)).WillByDefault([](const RoadGraph& roadGraph)
    {
    std::map<RoadGraphEdge, double> weights;
    for (auto [edge, edgesEnd] = edges(roadGraph); edge != edgesEnd; ++edge)
    {
        weights[*edge] = 1;
    }
    return weights;
    });

    RoadGraph resultGraph;
    RoadGraphVertex resultCurrent;
    RoadGraphVertex resultTarget;
    ON_CALL(egoAgent, SetRoadGraph(_,_,_)).WillByDefault([&](const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target)
    {
        resultGraph = roadGraph;
        resultCurrent = current;
        resultTarget = target;
    });

    RouteCalculation::GetNewRoute(agent, world, stochastics, RouteElement{"RoadA", true});

    ASSERT_THAT(resultGraph.vertex_set(), Eq(roadGraphA.vertex_set()));
    ASSERT_THAT(resultCurrent, Eq(nodeA));
    ASSERT_THAT(resultTarget, Eq(nodeE));
}

TEST(RouteCalculation, PointsOnUnconnectedRoads)
{
    FakeAgent agent;
    FakeEgoAgent egoAgent;
    FakeWorld world;
    FakeStochastics stochastics;

    ON_CALL(agent, GetEgoAgent()).WillByDefault(ReturnRef(egoAgent));
    GlobalRoadPositions referencePoint = {{"RoadA", GlobalRoadPosition{"RoadA", -2, 10, 0, 0.2}},{"RoadB", GlobalRoadPosition{"RoadB", -2, 10, 0, 0.1}}};
    GlobalRoadPositions mainLocatePoint = {{"RoadC", GlobalRoadPosition{"RoadC", -2, 10, 0, 0}}};
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::Reference))).WillByDefault(ReturnRef(referencePoint));
    ON_CALL(agent, GetRoadPosition(VariantWith<ObjectPointPredefined>(ObjectPointPredefined::FrontCenter))).WillByDefault(ReturnRef(mainLocatePoint));

    RoadGraph roadGraphA;
    RoadGraphVertex nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraphA);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadA", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphA, nodeA)));
    RoadGraph roadGraphB;
    RoadGraphVertex nodeB = add_vertex(RouteElement{"RoadB", true}, roadGraphB);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadB", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphB, nodeB)));
    RoadGraph roadGraphC;
    RoadGraphVertex nodeC = add_vertex(RouteElement{"RoadC", true}, roadGraphC);
    RoadGraphVertex nodeD = add_vertex(RouteElement{"RoadD", true}, roadGraphC);
    add_edge(nodeC, nodeD, roadGraphC);
    ON_CALL(world, GetRoadGraph(RouteElement{"RoadC", true}, _, true)).WillByDefault(Return(std::make_pair(roadGraphC, nodeB)));
    ON_CALL(world, GetEdgeWeights(_)).WillByDefault([](const RoadGraph& roadGraph)
    {
    std::map<RoadGraphEdge, double> weights;
    for (auto [edge, edgesEnd] = edges(roadGraph); edge != edgesEnd; ++edge)
    {
        weights[*edge] = 1;
    }
    return weights;
    });
    ON_CALL(world, IsDirectionalRoadExisting("RoadC", _)).WillByDefault(Return(true));

    RoadGraph resultGraph;
    RoadGraphVertex resultCurrent;
    RoadGraphVertex resultTarget;
    ON_CALL(egoAgent, SetRoadGraph(_,_,_)).WillByDefault([&](const RoadGraph&& roadGraph, RoadGraphVertex current, RoadGraphVertex target)
    {
        resultGraph = roadGraph;
        resultCurrent = current;
        resultTarget = target;
    });

    RouteCalculation::GetNewRoute(agent, world, stochastics, std::nullopt);

    ASSERT_THAT(resultGraph.vertex_set(), Eq(roadGraphC.vertex_set()));
    ASSERT_THAT(resultCurrent, Eq(nodeC));
    ASSERT_THAT(resultTarget, Eq(nodeD));
}

//! Check that two RoadGraph trees are equal (i.e. the graphs are equal including the RouteElements)
//! Note: This function only terminates if the graph are trees.
bool CheckTreeEquality(const RoadGraph& roadGraph1, const RoadGraphVertex& root1, const RoadGraph& roadGraph2, const RoadGraphVertex& root2)
{
    if (!(get(RouteElement{}, roadGraph1, root1) == get(RouteElement{}, roadGraph2, root2)))
    {
        return false;
    }
    if (out_degree(root1, roadGraph1) != out_degree(root2, roadGraph2))
    {
        return false;
    }
    if (out_degree(root1, roadGraph1) == 0)
    {
        return true;
    }
    for (auto [successor1, successors1End] = adjacent_vertices(root1, roadGraph1); successor1 != successors1End; successor1++)
    {
        bool foundMatchingSubtree = false;
        for (auto [successor2, successors2End] = adjacent_vertices(root2, roadGraph2); successor2 != successors2End; successor2++)
        {
            if (CheckTreeEquality(roadGraph1, *successor1, roadGraph2, *successor2))
            {
                foundMatchingSubtree = true;
                break;
            }
        }
        if (!foundMatchingSubtree)
        {
            return false;
        }
    }
    return true;
}

TEST(RouteCalculation_FilterRoadGraphByStartPosition, InDrivingDirection)
{
    RoadGraph roadGraph{};
    auto nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraph);
    auto nodeB = add_vertex(RouteElement{"RoadB", true}, roadGraph);
    auto nodeC = add_vertex(RouteElement{"RoadC", true}, roadGraph);
    auto nodeD = add_vertex(RouteElement{"RoadD", true}, roadGraph);
    auto nodeE = add_vertex(RouteElement{"RoadE", true}, roadGraph);
    auto nodeF = add_vertex(RouteElement{"RoadF", true}, roadGraph);
    add_edge(nodeA, nodeB, roadGraph);
    add_edge(nodeB, nodeC, roadGraph);
    add_edge(nodeB, nodeD, roadGraph);
    add_edge(nodeC, nodeE, roadGraph);
    add_edge(nodeC, nodeF, roadGraph);
    add_edge(nodeD, nodeF, roadGraph);

    const auto [resultGraph, resultRoot] = RouteCalculation::FilterRoadGraphByStartPosition(roadGraph, nodeB, 10, true);

    RoadGraph expectedRoadGraph{};
    auto resultNodeB = add_vertex(RouteElement{"RoadB", true}, expectedRoadGraph);
    auto resultNodeC = add_vertex(RouteElement{"RoadC", true}, expectedRoadGraph);
    auto resultNodeD = add_vertex(RouteElement{"RoadD", true}, expectedRoadGraph);
    auto resultNodeE = add_vertex(RouteElement{"RoadE", true}, expectedRoadGraph);
    auto resultNodeF1 = add_vertex(RouteElement{"RoadF", true}, expectedRoadGraph);
    auto resultNodeF2 = add_vertex(RouteElement{"RoadF", true}, expectedRoadGraph);
    add_edge(resultNodeB, resultNodeC, expectedRoadGraph);
    add_edge(resultNodeB, resultNodeD, expectedRoadGraph);
    add_edge(resultNodeC, resultNodeE, expectedRoadGraph);
    add_edge(resultNodeC, resultNodeF1, expectedRoadGraph);
    add_edge(resultNodeD, resultNodeF2, expectedRoadGraph);

    EXPECT_THAT(CheckTreeEquality(resultGraph, resultRoot, expectedRoadGraph, resultNodeB), true);
}

TEST(RouteCalculation_FilterRoadGraphByStartPosition, AgainstDrivingDirection)
{
    RoadGraph roadGraph{};
    auto nodeA = add_vertex(RouteElement{"RoadA", true}, roadGraph);
    auto nodeB = add_vertex(RouteElement{"RoadB", true}, roadGraph);
    auto nodeC = add_vertex(RouteElement{"RoadC", true}, roadGraph);
    auto nodeD = add_vertex(RouteElement{"RoadD", true}, roadGraph);
    auto nodeE = add_vertex(RouteElement{"RoadE", true}, roadGraph);
    auto nodeF = add_vertex(RouteElement{"RoadF", true}, roadGraph);
    add_edge(nodeB, nodeA, roadGraph);
    add_edge(nodeC, nodeB, roadGraph);
    add_edge(nodeD, nodeB, roadGraph);
    add_edge(nodeE, nodeC, roadGraph);
    add_edge(nodeF, nodeC, roadGraph);
    add_edge(nodeF, nodeD, roadGraph);

    const auto [resultGraph, resultRoot] = RouteCalculation::FilterRoadGraphByStartPosition(roadGraph, nodeB, 10, false);

    RoadGraph expectedRoadGraph{};
    auto resultNodeB = add_vertex(RouteElement{"RoadB", true}, expectedRoadGraph);
    auto resultNodeC = add_vertex(RouteElement{"RoadC", true}, expectedRoadGraph);
    auto resultNodeD = add_vertex(RouteElement{"RoadD", true}, expectedRoadGraph);
    auto resultNodeE = add_vertex(RouteElement{"RoadE", true}, expectedRoadGraph);
    auto resultNodeF1 = add_vertex(RouteElement{"RoadF", true}, expectedRoadGraph);
    auto resultNodeF2 = add_vertex(RouteElement{"RoadF", true}, expectedRoadGraph);
    add_edge(resultNodeB, resultNodeC, expectedRoadGraph);
    add_edge(resultNodeB, resultNodeD, expectedRoadGraph);
    add_edge(resultNodeC, resultNodeE, expectedRoadGraph);
    add_edge(resultNodeC, resultNodeF1, expectedRoadGraph);
    add_edge(resultNodeD, resultNodeF2, expectedRoadGraph);

    EXPECT_THAT(CheckTreeEquality(resultGraph, resultRoot, expectedRoadGraph, resultNodeB), true);
}