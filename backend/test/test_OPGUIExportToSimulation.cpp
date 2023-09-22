/*
 * Copyright (c) 2023 Hexad GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 */
#include <QDir>
#include <QDebug>
#include <QJsonObject>

#include "test_OPGUIExportToSimulation.h"
#include "OPGUICoreGlobalConfig.h"
#include "test_helpers.h"
#include "OPGUIPCMSimulation.h"
#include "OPGUICore.h"


QString expectedXml = QStringLiteral(
        R"(<?xml version="1.0" encoding="UTF-8"?>
        <opSimulationManager>
            <logLevel>3</logLevel>
            <logFileSimulationManager>$$TEST_DIR$$opSimulationManager.log</logFileSimulationManager>
            <libraries>modules</libraries>
            <simulationConfigs>
                <simulationConfig>
                    <configurations>$$PATH_TO_EXPERIMENT$$configs</configurations>
                    <logFileSimulation>$$PATH_TO_EXPERIMENT$$opSimulation.log</logFileSimulation>
                    <results>$$PATH_TO_EXPERIMENT$$results</results>
                </simulationConfig>
            </simulationConfigs>
        </opSimulationManager>)"
);

void EXPORT_TO_SIMULATION_TEST::InitSimulation(const QString &pcm_db_path) {
    QString errorMsg;
    OPGUIPCMSimulation::PCMSimulation *sim = new OPGUIPCMSimulation::PCMSimulation();

    bool simInited = sim->Init(pcm_db_path,config->fullPathModulesFolder(),config->pathConvertedCases(),config->fullPathOpSimulationExe(),errorMsg);

    OPGUICore::setSimulation(sim);
}

void EXPORT_TO_SIMULATION_TEST::DeleteSimulation() {
    OPGUIPCMSimulation::PCMSimulation *sim = OPGUICore::getSimulation(); // Assuming you have a getter

    if (sim) {
        delete sim;
        OPGUICore::setSimulation(nullptr); // Resetting it to a nullptr after deletion.
    }
}

void EXPORT_TO_SIMULATION_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
    config = &OPGUICoreGlobalConfig::getInstance();
    QString testFileName = QStringLiteral("test_scene.db");
    this->destinationFilePathTestDB = TestHelpers::joinPaths(config->pathOpenpassCore(),testFileName);

    //create temporary directory for the test
    testDir = QStringLiteral("test");
    this->workSpacePath = config->workspace();
    this->testDirFullPath = TestHelpers::joinPaths(this->workSpacePath,testDir);
    QDir dir(workSpacePath);
    ASSERT_TRUE(dir.exists());

    //remove test dir if exists
    QDir dirTest(this->testDirFullPath);
    if(dirTest.exists()){
        ASSERT_TRUE(dirTest.removeRecursively());
    }
    //remove test db file if exists
    bool removedSuccessfully = QFile::remove(this->destinationFilePathTestDB);

    ASSERT_TRUE(dir.mkpath(this->testDirFullPath));
    config->setPathConvertedCases(this->testDirFullPath);

    // Check if the source file exists
    ASSERT_TRUE(QFile::exists(testFileName)) << "Test: Source .db file doesn't exist at build directory: " << testFileName.toStdString();

    // Copy the file
    bool copiedSuccessfully = QFile::copy(testFileName, destinationFilePathTestDB);
    ASSERT_TRUE(copiedSuccessfully) << "Test: Failed to copy the database file to the target directory.";
}

void EXPORT_TO_SIMULATION_TEST::TearDown()  {
    config = &OPGUICoreGlobalConfig::getInstance();
    //remove db file from the core
    bool removedSuccessfully = QFile::remove(this->destinationFilePathTestDB);
    this->DeleteSimulation();
    EXPECT_TRUE(removedSuccessfully) << "Failed to remove the copied database file in: "+this->destinationFilePathTestDB.toStdString();

    QDir dir(this->testDirFullPath);
    if(dir.exists()){
        EXPECT_TRUE(dir.removeRecursively());
    }

    config->setPathConvertedCases(config->workspace());
}

TEST_F(EXPORT_TO_SIMULATION_TEST, export_to_simulations_list_of_experiments_empty_NEGATIVE) {
    QString errorMsg;
    this->request.setSelectedExperiments(QList<QString>());
    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAISelectedExperimentsRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    //call convert to configs
    bool result = OPGUICore::api_export_to_simulations(validatedRequest, this->response,errorMsg);
    
    ASSERT_FALSE(result)<<"Test: Result of api_export_to_simulations was expected to be false";

    ASSERT_TRUE(errorMsg.contains("List of experiments for config conversion is invalid / empty."))<<"Error message was expected to contain 'List of experiments for config conversion is invalid / empty.' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_TO_SIMULATION_TEST, export_to_simulations_simulation_not_inited_NEGATIVE) {
    QString errorMsg;
    this->request.setSelectedExperiments(QList<QString>() << "UNUSED");
    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAISelectedExperimentsRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    //as simulation not initialised, expect failed
    bool result = OPGUICore::api_export_to_simulations(validatedRequest, this->response,errorMsg);

    EXPECT_FALSE(result)<<"Test: Result of api_export_to_simulations was expected to be false";

    ASSERT_TRUE(errorMsg.contains("PCM Plugin is not initialised."))<<"Error message was expected to contain 'PCM Plugin is not initialised.' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_TO_SIMULATION_TEST, export_to_simulations_simulation_export_to_simulations_fail_NEGATIVE) {
    QString errorMsg;
    this->request.setSelectedExperiments(QList<QString>() << "UNUSED");
    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAISelectedExperimentsRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    this->InitSimulation(this->destinationFilePathTestDB);

    //expect fail as config reader is not inited
    bool result = OPGUICore::api_export_to_simulations(validatedRequest, this->response,errorMsg);

    EXPECT_FALSE(result)<<"Test: Result of api_export_to_simulations was expected to be false";

    ASSERT_TRUE(errorMsg.contains("Config Generator was not initialised."))<<"Error message was expected to contain 'Config Generator was not initialised.' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_TO_SIMULATION_TEST, export_to_simulations_simulation_inited_POSITIVE) {
    QString errorMsg;
    this->request.setSelectedExperiments(QList<QString>() << "1");
    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAISelectedExperimentsRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    this->InitSimulation(this->destinationFilePathTestDB);
    //init config reader with list of experiments from send pcm file
    QString err;
    QStringList listExperiments =QList<QString>() << "1";

    //set dummy path for agents so search is not failed
    QList<QString> pathWithSomeRealFile = {this->destinationFilePathTestDB};
    OPGUICore::getSimulation()->setAgentsCar1(pathWithSomeRealFile);
    OPGUICore::getSimulation()->setAgentsCar2(pathWithSomeRealFile);
    OPGUICore::getSimulation()->setAgentsOther(pathWithSomeRealFile);

    OPGUICore::getSimulation()->UIGenerateConfigOfPCMCases(listExperiments,err);

    bool result = OPGUICore::api_export_to_simulations(validatedRequest, this->response,errorMsg);

    QString simConfigFile = OPGUICore::getSimulation()->getPathGeneratedOPSimulationManagerConfig();

    this->response200Expected.setResponse("Simulation Manager Config File created as:"+simConfigFile);

    ASSERT_TRUE(result)<<"Test: Result of export_to_simulation was expected to be true";
    EXPECT_EQ(this->response.getResponse(), this->response200Expected.getResponse())
    << "Test: Response was not the expected. Got: " << this->response.getResponse().toStdString() 
    << ", Expected: " << this->response200Expected.getResponse().toStdString() ;
    
    // Check that the XML file has been created
    EXPECT_TRUE(QFile::exists(OPGUICore::getSimulation()->getPathGeneratedOPSimulationManagerConfig())) << "Test: exportToXML did not created any file";

    expectedXml.replace("$$PATH_TO_EXPERIMENT$$", this->testDirFullPath + "/1/0-0-0/" );
    expectedXml.replace("$$TEST_DIR$$", this->testDirFullPath + QStringLiteral("/"));

    QString actualXml = TestHelpers::readFile(simConfigFile);

    EXPECT_EQ(
    TestHelpers::removeSpacesBetweenTags(expectedXml.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated xml file differ.\n"
        << "Expected XML:\n" << expectedXml.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString(); 
}









