/********************************************************************************
 * Copyright (c) 2018-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/helper/importerHelper.h"
#include "simulationConfigImporter.h"
#include "simulationConfig.h"

using ::testing::SizeIs;
using ::testing::Eq;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;
using ::testing::EndsWith;
using ::testing::ElementsAre;

using namespace Importer;

TEST(SimulationConfigImporter_UnitTests, ImportSpawnersSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                           "<Spawner>"
                                               "<Library>Test_Library</Library>"
                                               "<Type>PreRun</Type>"
                                               "<Priority>1</Priority>"
                                           "</Spawner>"
                                           "<Spawner>"
                                               "<Library>Test_Library</Library>"
                                               "<Type>Runtime</Type>"
                                               "<Priority>0</Priority>"
                                               "<Profile>ExampleProfile</Profile>"
                                           "</Spawner>"
                                       "</root>"
                                    );

    Configuration::SimulationConfig simulationConfig;

    EXPECT_NO_THROW(SimulationConfigImporter::ImportSpawners(fakeDocumentRoot, simulationConfig));
    const SpawnPointLibraryInfoCollection& spawnPointsConfig = simulationConfig.GetSpawnPointsConfig();
    EXPECT_THAT(spawnPointsConfig, SizeIs(2));
    const auto resultSpawnPointLibraryInfo1 = spawnPointsConfig.at(0);
    const auto resultSpawnPointLibraryInfo2 = spawnPointsConfig.at(1);

    EXPECT_THAT(resultSpawnPointLibraryInfo1.libraryName, "Test_Library");
    EXPECT_THAT(resultSpawnPointLibraryInfo1.type, SpawnPointType::PreRun);
    EXPECT_THAT(resultSpawnPointLibraryInfo1.priority, 1);
    EXPECT_THAT(resultSpawnPointLibraryInfo1.profileName, std::nullopt);
    EXPECT_THAT(resultSpawnPointLibraryInfo2.libraryName, "Test_Library");
    EXPECT_THAT(resultSpawnPointLibraryInfo2.type, SpawnPointType::Runtime);
    EXPECT_THAT(resultSpawnPointLibraryInfo2.priority, 0);
    EXPECT_THAT(resultSpawnPointLibraryInfo2.profileName, "ExampleProfile");
}

TEST(SimulationConfigImporter_UnitTests, ImportSpawnersUnsuccessfully)
{
    QDomElement fakeDocumentRootMissingLibrary = documentRootFromString(
                                       "<root>"
                                           "<Spawner>"
                                               "<Type>PreRun</Type>"
                                               "<Priority>1</Priority>"
                                           "</Spawner>"
                                       "</root>"
                                    );
    QDomElement fakeDocumentRootMissingType = documentRootFromString(
                                       "<root>"
                                           "<Spawner>"
                                               "<Library>Test_Library</Library>"
                                               "<Priority>1</Priority>"
                                           "</Spawner>"
                                       "</root>"
                                    );
    QDomElement fakeDocumentRootMissingPriority = documentRootFromString(
                                       "<root>"
                                           "<Spawner>"
                                               "<Library>Test_Library</Library>"
                                               "<Type>PreRun</Type>"
                                           "</Spawner>"
                                       "</root>"
                                    );
    Configuration::SimulationConfig simulationConfig;

    ASSERT_THROW(SimulationConfigImporter::ImportSpawners(fakeDocumentRootMissingLibrary, simulationConfig), std::runtime_error);
    ASSERT_THROW(SimulationConfigImporter::ImportSpawners(fakeDocumentRootMissingType, simulationConfig), std::runtime_error);
    ASSERT_THROW(SimulationConfigImporter::ImportSpawners(fakeDocumentRootMissingPriority, simulationConfig), std::runtime_error);
}

TEST(SimulationConfigImporter_UnitTests, ImportObserversSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                           "<Observation>"
                                               "<Library>Test_Library</Library>"
                                               "<Parameters>"
                                               "</Parameters>"
                                           "</Observation>"
                                           "<Observation>"
                                               "<Library>Test_Library</Library>"
                                                "<Parameters>"
                                                    "<Int Key=\"TestParameter\" Value=\"1\" />"
                                                "</Parameters>"
                                           "</Observation>"
                                       "</root>"
                                    );

    Configuration::SimulationConfig simulationConfig;

    EXPECT_NO_THROW(SimulationConfigImporter::ImportObservations(fakeDocumentRoot, simulationConfig));
    const ObservationInstanceCollection& observations = simulationConfig.GetObservationConfig();
    EXPECT_THAT(observations, SizeIs(2));
    const auto observation1 = observations.at(0);
    const auto observation2 = observations.at(1);

    EXPECT_THAT(observation1.libraryName, "Test_Library");
    EXPECT_THAT(observation1.parameters, SizeIs(0));
    EXPECT_THAT(observation2.libraryName, "Test_Library");
    EXPECT_THAT(observation2.parameters, SizeIs(1));
    EXPECT_THAT(observation2.parameters.at(0).first, Eq("TestParameter"));
    EXPECT_THAT(std::get<int>(std::get<openpass::parameter::internal::ParameterValue>(observation2.parameters.at(0).second)), Eq(1));
}

TEST(SimulationConfigImporter_UnitTests, ImportExperimentConfigSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<ExperimentID>1337</ExperimentID>"
                                       "<NumberOfInvocations>5</NumberOfInvocations>"
                                       "<RandomSeed>12345</RandomSeed>"
                                       "</root>"
                                   );


    Configuration::SimulationConfig simulationConfig;

    EXPECT_NO_THROW(SimulationConfigImporter::ImportExperiment(fakeDocumentRoot, simulationConfig));
    const ExperimentConfig& experimentConfig = simulationConfig.GetExperimentConfig();

    EXPECT_THAT(experimentConfig.experimentId,        1337);
    EXPECT_THAT(experimentConfig.numberOfInvocations, 5);
    EXPECT_THAT(experimentConfig.randomSeed,          12345);
}

TEST(SimulationConfigImporter_UnitTests, ImportExperimentConfigUnsuccessfully)
{
    QDomElement fakeDocumentRootMissingRandomSeed = documentRootFromString(
                "<root>"
                "<ExperimentID>1337</ExperimentID>"
                "<NumberOfInvocations>5</NumberOfInvocations>"
                "</root>"
            );

    QDomElement fakeDocumentRootWrongDataType = documentRootFromString(
                "<root>"
                "<ExperimentID>1337</ExperimentID>"
                "<NumberOfInvocations>abc</NumberOfInvocations>"
                "<RandomSeed>12345</RandomSeed>"
                "</root>"
            );

    QDomElement fakeDocumentRootWrongTagName = documentRootFromString(
                "<root>"
                "<Experimentid>1337</Experimentid>"
                "<NumberOfInvocations>5</NumberOfInvocations>"
                "<RandomSeed>12345</RandomSeed>"
                "</root>"
            );

    Configuration::SimulationConfig simulationConfig;

    ASSERT_THROW(SimulationConfigImporter::ImportExperiment(fakeDocumentRootMissingRandomSeed, simulationConfig), std::runtime_error);
    ASSERT_THROW(SimulationConfigImporter::ImportExperiment(fakeDocumentRootWrongDataType, simulationConfig), std::runtime_error);
    ASSERT_THROW(SimulationConfigImporter::ImportExperiment(fakeDocumentRootWrongTagName, simulationConfig), std::runtime_error);
}

TEST(SimulationConfigImporter_UnitTests, ImportMissingLibraries_LoadsDefaults)
{
    QDomElement fakeDocumentRoot = documentRootFromString("<root/>");

    auto libraries = SimulationConfigImporter::ImportLibraries(fakeDocumentRoot);

    EXPECT_THAT(libraries["EventDetectorLibrary"], "EventDetector");
    EXPECT_THAT(libraries["ManipulatorLibrary"], "Manipulator");
    EXPECT_THAT(libraries["WorldLibrary"], "World");
    EXPECT_THAT(libraries["StochasticsLibrary"], "Stochastics");
}

TEST(SimulationConfigImporter_UnitTests, ImportCompleteLibraryList_ParsesSpecifiedValues)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "  <Libraries>"
                                       "    <EventDetectorLibrary>TestEventDetector</EventDetectorLibrary>"
                                       "    <ManipulatorLibrary>TestManipulator</ManipulatorLibrary>"
                                       "    <WorldLibrary>TestWorld</WorldLibrary>"
                                       "    <StochasticsLibrary>TestStochastics</StochasticsLibrary>"
                                       "  </Libraries>"
                                       "</root>"
                                   );

    auto libraries = SimulationConfigImporter::ImportLibraries(fakeDocumentRoot);

    EXPECT_THAT(libraries["EventDetectorLibrary"], "TestEventDetector");
    EXPECT_THAT(libraries["ManipulatorLibrary"], "TestManipulator");
    EXPECT_THAT(libraries["WorldLibrary"], "TestWorld");
    EXPECT_THAT(libraries["StochasticsLibrary"], "TestStochastics");
}

TEST(SimulationConfigImporter_UnitTests, ImportPartialLibraryList_ParsesSpecifiedValues)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "  <Libraries>"
                                       "    <WorldLibrary>TestWorld</WorldLibrary>"
                                       "    <EventDetectorLibrary>TestEventDetector</EventDetectorLibrary>"
                                       "  </Libraries>"
                                       "</root>"
                                   );

    auto libraries = SimulationConfigImporter::ImportLibraries(fakeDocumentRoot);

    EXPECT_THAT(libraries["WorldLibrary"], "TestWorld");
    EXPECT_THAT(libraries["EventDetectorLibrary"], "TestEventDetector");
    EXPECT_THAT(libraries["ManipulatorLibrary"], "Manipulator");
    EXPECT_THAT(libraries["StochasticsLibrary"], "Stochastics");
}

TEST(SimulationConfigImporter_UnitTests, ImportEmptyLibraryList_ParsesSpecifiedValues)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "</root>"
                                   );

    auto libraries = SimulationConfigImporter::ImportLibraries(fakeDocumentRoot);

    EXPECT_THAT(libraries["EventDetectorLibrary"], "EventDetector");
    EXPECT_THAT(libraries["ManipulatorLibrary"], "Manipulator");
    EXPECT_THAT(libraries["StochasticsLibrary"], "Stochastics");
    EXPECT_THAT(libraries["WorldLibrary"], "World");
}

TEST(SimulationConfigImporter_UnitTests, ImportScenarioConfigSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<OpenScenarioFile>scenarioFile.xml</OpenScenarioFile>"
                                       "</root>"
                                   );


    Configuration::SimulationConfig simulationConfig;

    EXPECT_NO_THROW(SimulationConfigImporter::ImportScenario(fakeDocumentRoot, "configs", simulationConfig));
    const ScenarioConfig& scenarioConfig = simulationConfig.GetScenarioConfig();
    ASSERT_THAT(scenarioConfig.scenarioPath, EndsWith("configs/scenarioFile.xml"));
}

TEST(SimulationConfigImporter_UnitTests, ImportScenarioConfigUnsuccessfullyMissingTag)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<OpenScenarioF>scenarioFile.xml</OpenScenarioF>"
                                       "</root>"
                                   );


    Configuration::SimulationConfig simulationConfig;

    ASSERT_THROW(SimulationConfigImporter::ImportScenario(fakeDocumentRoot, "configs", simulationConfig), std::runtime_error);
}

TEST(SimulationConfigImporter_UnitTests, ImportEnvironmentConfigSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<TimeOfDays>"
                                       "<TimeOfDay Value=\"12\" Probability=\"0.5\"/>"
                                       "<TimeOfDay Value=\"18\" Probability=\"0.5\"/>"
                                       "</TimeOfDays>"
                                       "<VisibilityDistances>"
                                       "<VisibilityDistance Value=\"100\" Probability=\"0.5\"/>"
                                       "<VisibilityDistance Value=\"200\" Probability=\"0.5\"/>"
                                       "</VisibilityDistances>"
                                       "<Frictions>"
                                       "<Friction Value=\"0.3\" Probability=\"0.5\"/>"
                                       "<Friction Value=\"0.7\" Probability=\"0.5\"/>"
                                       "</Frictions>"
                                       "<Weathers>"
                                       "<Weather Value=\"Rainy\" Probability=\"0.5\"/>"
                                       "<Weather Value=\"Snowy\" Probability=\"0.5\"/>"
                                       "</Weathers>"
                                       "<TrafficRules>Country</TrafficRules>"
                                       "</root>"
                                   );


    Configuration::SimulationConfig simulationConfig;

    EXPECT_NO_THROW(SimulationConfigImporter::ImportEnvironment(fakeDocumentRoot, simulationConfig));
    const EnvironmentConfig& environmentConfig = simulationConfig.GetEnvironmentConfig();

    ASSERT_THAT(environmentConfig.timeOfDays, ElementsAre(Pair("12", 0.5),
                                                          Pair("18", 0.5)));
    ASSERT_THAT(environmentConfig.visibilityDistances, ElementsAre(Pair(100, 0.5),
                                                                   Pair(200, 0.5)));
    ASSERT_THAT(environmentConfig.frictions, ElementsAre(Pair(0.3, 0.5),
                                                          Pair(0.7, 0.5)));
    ASSERT_THAT(environmentConfig.weathers, ElementsAre(Pair("Rainy", 0.5),
                                                          Pair("Snowy", 0.5)));
    ASSERT_THAT(environmentConfig.trafficRules, Eq("Country"));
}

TEST(SimulationConfigImporter_UnitTests, ImportEnvironmentConfigUnsuccessfully)
{
    QDomElement fakeDocumentRootWrongProbabilities = documentRootFromString(
                "<root>"
                "<TimeOfDays>"
                "<TimeOfDay Value=\"12\" Probability=\"0.5\"/>"
                "<TimeOfDay Value=\"18\" Probability=\"0.6\"/>"
                "</TimeOfDays>"
                "<VisibilityDistances>"
                "<VisibilityDistance Value=\"100\" Probability=\"0.5\"/>"
                "<VisibilityDistance Value=\"200\" Probability=\"0.5\"/>"
                "</VisibilityDistances>"
                "<Frictions>"
                "<Friction Value=\"0.3\" Probability=\"0.5\"/>"
                "<Friction Value=\"0.7\" Probability=\"0.5\"/>"
                "</Frictions>"
                "<Weathers>"
                "<Weather Value=\"Rainy\" Probability=\"0.5\"/>"
                "<Weather Value=\"Snowy\" Probability=\"0.5\"/>"
                "</Weathers>"
                "</root>"
            );

    QDomElement fakeDocumentRootMissingAtLeastOneElement = documentRootFromString(
                "<root>"
                "<TimeOfDays>"
                "<TimeOfDay Value=\"12\" Probability=\"0.5\"/>"
                "<TimeOfDay Value=\"18\" Probability=\"0.5\"/>"
                "</TimeOfDays>"
                "<VisibilityDistances>"
                "</VisibilityDistances>"
                "<Frictions>"
                "<Friction Value=\"0.3\" Probability=\"0.5\"/>"
                "<Friction Value=\"0.7\" Probability=\"0.5\"/>"
                "</Frictions>"
                "<Weathers>"
                "<Weather Value=\"Rainy\" Probability=\"0.5\"/>"
                "<Weather Value=\"Snowy\" Probability=\"0.5\"/>"
                "</Weathers>"
                "</root>"
            );

    QDomElement fakeDocumentRootMissingTag = documentRootFromString(
                "<root>"
                "<TimeOfDays>"
                "<TimeOfDay Value=\"12\" Probability=\"0.5\"/>"
                "<TimeOfDay Value=\"18\" Probability=\"0.5\"/>"
                "</TimeOfDays>"
                "<VisibilityDistances>"
                "<VisibilityDistance Value=\"100\" Probability=\"0.5\"/>"
                "<VisibilityDistance Value=\"200\" Probability=\"0.5\"/>"
                "</VisibilityDistances>"
                "<Frictions>"
                "<Friction Value=\"0.3\" Probability=\"0.5\"/>"
                "<Friction Value=\"0.7\" Probability=\"0.5\"/>"
                "</Frictions>"
                "</root>"
            );

    Configuration::SimulationConfig simulationConfig;

    ASSERT_THROW(SimulationConfigImporter::ImportEnvironment(fakeDocumentRootWrongProbabilities, simulationConfig),
                 std::runtime_error);
    ASSERT_THROW(SimulationConfigImporter::ImportEnvironment(fakeDocumentRootMissingAtLeastOneElement, simulationConfig),
                 std::runtime_error);
    ASSERT_THROW(SimulationConfigImporter::ImportEnvironment(fakeDocumentRootMissingTag, simulationConfig),
                 std::runtime_error);
}

bool operator==(const TurningRate& lhs, const TurningRate& rhs)
{
    return lhs.incoming == rhs.incoming
    && lhs.outgoing == rhs.outgoing
    && lhs.weight == rhs.weight;
}

TEST(SimulationConfigImporter_UnitTests, ImportTurningRatesSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                        "<TurningRate Incoming=\"RoadA\" Outgoing=\"RoadB\" Weight=\"2.0\"/>"
                                        "<TurningRate Incoming=\"RoadA\" Outgoing=\"RoadC\" Weight=\"3.0\"/>"
                                        "<TurningRate Incoming=\"RoadB\" Outgoing=\"RoadD\" Weight=\"4.0\"/>"
                                       "</root>"
                                   );


    TurningRates turningRates;

    EXPECT_NO_THROW(SimulationConfigImporter::ImportTurningRates(fakeDocumentRoot, turningRates));

    ASSERT_THAT(turningRates, UnorderedElementsAre(TurningRate{"RoadA", "RoadB", 2.0},
                                                   TurningRate{"RoadA", "RoadC", 3.0},
                                                   TurningRate{"RoadB", "RoadD", 4.0}));
}