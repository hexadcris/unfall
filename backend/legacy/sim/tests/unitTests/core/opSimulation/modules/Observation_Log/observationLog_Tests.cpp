/********************************************************************************
 * Copyright (c) 2018-2022 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <variant>

#include "observationCyclics.h"
#include "observation_logImplementation.h"
#include "runStatisticCalculation.h"

#include "fakeAgent.h"
#include "fakeDataBuffer.h"
#include "fakeWorld.h"
#include "fakeRunResult.h"

using ::testing::ByMove;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Not;
using ::testing::Optional;
using ::testing::Pair;
using ::testing::Return;
using ::testing::SizeIs;

TEST(ObservationLog_GetAgentValues, AgentWithoutMatchingKeyInBuffer_DoesntCrash)
{
    const Keys fakeAgentIds{"0"};
    const Key key{"theKey"};
    const Key otherKey{"otherKey"};

    std::unique_ptr<NiceMock<FakeCyclicResult>> emptyCyclicResult = std::make_unique<NiceMock<FakeCyclicResult>>();
    ON_CALL(*emptyCyclicResult, size()).WillByDefault(Return(0));

    NiceMock<FakeDataBuffer> buffer;
    EXPECT_CALL(buffer, GetKeys("Statics/Agents")).WillRepeatedly(Return(fakeAgentIds));
    ON_CALL(buffer, GetCyclic(Optional(0), Not(key))).WillByDefault(Return(ByMove(std::move(emptyCyclicResult))));

    std::map<std::string, double> result;

    ASSERT_NO_THROW(GetAgentValues(otherKey, &buffer, result));
    ASSERT_THAT(result, SizeIs(0));
}

TEST(ObservationLog_GetAgentValues, MultipleAgentsInBuffer_StoresCorrectResult)
{
    const Keys fakeAgentIds{"0", "1"};
    const Key key{"theKey"};

    const CyclicRow row0{0, key, 1.0};
    const CyclicRow row1{1, key, 2.0};
    const CyclicRowRefs rowRefs0{row0};
    const CyclicRowRefs rowRefs1{row1};

    std::unique_ptr<NiceMock<FakeCyclicResult>> cyclicResult0 = std::make_unique<NiceMock<FakeCyclicResult>>();
    std::unique_ptr<NiceMock<FakeCyclicResult>> cyclicResult1 = std::make_unique<NiceMock<FakeCyclicResult>>();
    ON_CALL(*cyclicResult0, size()).WillByDefault(Return(1));
    ON_CALL(*cyclicResult1, size()).WillByDefault(Return(1));
    ON_CALL(*cyclicResult0, begin()).WillByDefault(Return(rowRefs0.cbegin()));
    ON_CALL(*cyclicResult1, begin()).WillByDefault(Return(rowRefs1.cbegin()));

    NiceMock<FakeDataBuffer> buffer;
    EXPECT_CALL(buffer, GetKeys("Statics/Agents")).WillRepeatedly(Return(fakeAgentIds));
    ON_CALL(buffer, GetCyclic(Optional(0), key)).WillByDefault(Return(ByMove(std::move(cyclicResult0))));
    ON_CALL(buffer, GetCyclic(Optional(1), key)).WillByDefault(Return(ByMove(std::move(cyclicResult1))));

    std::map<std::string, double> result;

    GetAgentValues(key, &buffer, result);

    ASSERT_THAT(result, SizeIs(2));
    EXPECT_THAT(result, ElementsAre(Pair("0", 1.0), Pair("1", 2.0)));
}

TEST(ObservationLog_GetAgentValues, OtherDatatypeThanDoubleInStore_Throws)
{
    const Keys fakeAgentIds{"0"};
    const Key key{"theKey"};

    const CyclicRow row{0, key, "other data"};
    const CyclicRowRefs rowRefs{row};

    std::unique_ptr<NiceMock<FakeCyclicResult>> cyclicResult = std::make_unique<NiceMock<FakeCyclicResult>>();
    ON_CALL(*cyclicResult, size()).WillByDefault(Return(1));
    ON_CALL(*cyclicResult, begin()).WillByDefault(Return(rowRefs.cbegin()));

    NiceMock<FakeDataBuffer> buffer;
    EXPECT_CALL(buffer, GetKeys("Statics/Agents")).WillRepeatedly(Return(fakeAgentIds));
    ON_CALL(buffer, GetCyclic(Optional(0), key)).WillByDefault(Return(ByMove(std::move(cyclicResult))));

    std::map<std::string, double> result;

    ASSERT_THROW(GetAgentValues(key, &buffer, result), std::bad_variant_access);
}

TEST(ObservationCyclics_Test, GetHeader_ReturnsCorrectHeader)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(100, "ParameterA", "345");
    cyclics.Insert(100, "ParameterB", "456");

    std::string header = cyclics.GetHeader();
    ASSERT_THAT(header, Eq("ParameterA, ParameterB, ParameterC"));
}

TEST(ObservationCyclics_Test, GetTimeSteps_ReturnsCorrectTimesteps)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(100, "ParameterA", "345");
    cyclics.Insert(150, "ParameterB", "456");

    const auto& timesteps = cyclics.GetTimeSteps();
    ASSERT_THAT(timesteps, ElementsAre(0, 100, 150));
}

TEST(ObservationCyclics_Test, GetSamplesLineAllSamplesExisting_ReturnsCorrectLine)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(0, "ParameterB", "345");
    cyclics.Insert(100, "ParameterA", "456");
    cyclics.Insert(100, "ParameterC", "567");
    cyclics.Insert(100, "ParameterB", "678");

    std::string samplesLine = cyclics.GetSamplesLine(0);
    ASSERT_THAT(samplesLine, Eq("123, 345, 234"));
    samplesLine = cyclics.GetSamplesLine(1);
    ASSERT_THAT(samplesLine, Eq("456, 678, 567"));
}

TEST(ObservationCyclics_Test, GetSamplesLineSamplesAddedAfter_ReturnsLineWithEmptyString)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(100, "ParameterA", "234");
    cyclics.Insert(100, "ParameterC", "345");
    cyclics.Insert(200, "ParameterA", "456");
    cyclics.Insert(200, "ParameterC", "567");
    cyclics.Insert(200, "ParameterB", "678");

    std::string samplesLine = cyclics.GetSamplesLine(0);
    ASSERT_THAT(samplesLine, Eq("123, , "));
    samplesLine = cyclics.GetSamplesLine(1);
    ASSERT_THAT(samplesLine, Eq("234, , 345"));
    samplesLine = cyclics.GetSamplesLine(2);
    ASSERT_THAT(samplesLine, Eq("456, 678, 567"));
}

TEST(ObservationCyclics_Test, GetSamplesLineSamplesMissingInBetween_ReturnsLineWithEmptyString)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(0, "ParameterB", "345");
    cyclics.Insert(100, "ParameterA", "456");
    cyclics.Insert(200, "ParameterA", "567");
    cyclics.Insert(200, "ParameterC", "678");
    cyclics.Insert(200, "ParameterB", "789");

    std::string samplesLine = cyclics.GetSamplesLine(1);
    ASSERT_THAT(samplesLine, Eq("456, , "));
}

TEST(ObservationCyclics_Test, GetSamplesLineSamplesNotUntilEnd_ReturnsLineWithEmptyString)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(0, "ParameterB", "345");
    cyclics.Insert(100, "ParameterA", "456");
    cyclics.Insert(100, "ParameterB", "678");
    cyclics.Insert(200, "ParameterB", "789");

    std::string samplesLine = cyclics.GetSamplesLine(0);
    ASSERT_THAT(samplesLine, Eq("123, 345, 234"));
    samplesLine = cyclics.GetSamplesLine(1);
    ASSERT_THAT(samplesLine, Eq("456, 678, "));
    samplesLine = cyclics.GetSamplesLine(2);
    ASSERT_THAT(samplesLine, Eq(", 789, "));
}

TEST(RunStatisticCalculation_Test, DetermineEgoCollisionWithEgoCollision_SetsEgoCollisionTrue)
{
    NiceMock<FakeWorld> fakeWorld;

    NiceMock<FakeAgent> fakeEgo;
    ON_CALL(fakeEgo, GetId()).WillByDefault(Return(0));

    ON_CALL(fakeWorld, GetEgoAgent()).WillByDefault(Return(&fakeEgo));

    NiceMock<FakeRunResult> runResult;

    std::vector<int> collisionIds{ {0, 1} };
    ON_CALL(runResult, GetCollisionIds()).WillByDefault(Return(&collisionIds));

    RunStatistic runStatistic{0};

    RunStatisticCalculation::DetermineEgoCollision(runStatistic, runResult, &fakeWorld);

    ASSERT_THAT(runStatistic.EgoCollision, Eq(true));
}

TEST(RunStatisticCalculation_Test, DetermineEgoCollisionWithoutEgoCollision_SetsEgoCollisionFalse)
{
    NiceMock<FakeWorld> fakeWorld;

    NiceMock<FakeAgent> fakeEgo;
    ON_CALL(fakeEgo, GetId()).WillByDefault(Return(0));

    ON_CALL(fakeWorld, GetEgoAgent()).WillByDefault(Return(&fakeEgo));

    NiceMock<FakeRunResult> runResult;

    std::vector<int> collisionIds{ {1, 2, 3} };
    ON_CALL(runResult, GetCollisionIds()).WillByDefault(Return(&collisionIds));

    RunStatistic runStatistic{0};

    RunStatisticCalculation::DetermineEgoCollision(runStatistic, runResult, &fakeWorld);

    ASSERT_THAT(runStatistic.EgoCollision, Eq(false));
}
