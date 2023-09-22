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
#include <QThread>
#include <QRegularExpression>
#include <QString>
#include <QFile>
#include <QVector>
#include <QDomElement>

#include "test_helpers.h"
#include "OPGUICoreGlobalConfig.h"
#include "test_OPGUIRunOpSimulationManagerApi.h"

QString configXml1Simulation = QStringLiteral(
        R"(<?xml version="1.0" encoding="UTF-8"?>
        <opSimulationManager>
            <logLevel>5</logLevel>
            <simulation>opSimulation</simulation>
            <logFileSimulationManager>$$CORE_DIR$$opSimulationManager.log</logFileSimulationManager>
            <libraries>modules_TEST_TEMP</libraries>
            <simulationConfigs>
                <simulationConfig>
                    <configurations>$$CORE_DIR$$configs1</configurations>
                    <logFileSimulation>$$CORE_DIR$$log_TEST_TEMP_1.log</logFileSimulation>
                    <results>$$CORE_DIR$$results_TEST_TEMP_1</results>
                </simulationConfig>
            </simulationConfigs>
        </opSimulationManager>)"
);

QString configXml2Simulation = QStringLiteral(
        R"(<?xml version="1.0" encoding="UTF-8"?>
        <opSimulationManager>
            <logLevel>5</logLevel>
            <simulation>opSimulation</simulation>
            <logFileSimulationManager>$$CORE_DIR$$opSimulationManager.log</logFileSimulationManager>
            <libraries>modules_TEST_TEMP</libraries>
            <simulationConfigs>
                <simulationConfig>
                    <configurations>$$CORE_DIR$$configs1</configurations>
                    <logFileSimulation>$$CORE_DIR$$log_TEST_TEMP_1.log</logFileSimulation>
                    <results>$$CORE_DIR$$results_TEST_TEMP_1</results>
                </simulationConfig>
                <simulationConfig>
                    <configurations>$$CORE_DIR$$configs2</configurations>
                    <logFileSimulation>$$CORE_DIR$$log_TEST_TEMP_2.log</logFileSimulation>
                    <results>$$CORE_DIR$$results_TEST_TEMP_2</results>
                </simulationConfig>
            </simulationConfigs>
        </opSimulationManager>)"
);
QString configXml3Simulation = QStringLiteral(
        R"(<?xml version="1.0" encoding="UTF-8"?>
        <opSimulationManager>
            <logLevel>5</logLevel>
            <simulation>opSimulation</simulation>
            <logFileSimulationManager>$$CORE_DIR$$opSimulationManager_TEST_TEMP.log</logFileSimulationManager>
            <libraries>modules_TEST_TEMP</libraries>
            <simulationConfigs>
                <simulationConfig>
                    <configurations>$$CORE_DIR$$configs</configurations>
                    <logFileSimulation>$$CORE_DIR$$log_TEST_TEMP_1.log</logFileSimulation>
                    <results>$$CORE_DIR$$results_TEST_TEMP</results>
                </simulationConfig>
            </simulationConfigs>
        </opSimulationManager>)"
);

QString configXmlSimulationWrong = QStringLiteral(
        R"(<?xml version="1.0" encoding="UTF-8"?>
        <opSimulationnager>
        <logLevel>2</logLevel>
        <logFileSimulationMa leSimulationManager>
        <simulation> mulation</simulation>
        </opSimlationManager>)"
);


void RUN_OP_SIMULATION_MANAGER_TEST::SetUp() {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";

    response200Expected.setResponse("Simulation started correctly");

    // Prepare the file paths
    this->opSimulationExe = OPGUICoreGlobalConfig::getInstance().opSimulationExe();
    this->openPassCoreDir = OPGUICoreGlobalConfig::getInstance().pathOpenpassCore();
    this->configFileFullPath = OPGUICoreGlobalConfig::getInstance().fullPathOpSimulationManagerXml();
    this->backupFilePath = this->configFileFullPath + QString::fromStdString("_bk");

    // Check core path existence
    QDir dir(this->openPassCoreDir);
    ASSERT_TRUE(dir.exists()) << "SetUp: PATH_OPENPASS_CORE not existing: " + this->openPassCoreDir.toStdString();

    // Rename the backup file back to its original name if it exists
    QFile backupFile(this->backupFilePath);
    if (backupFile.exists()) {
        bool fileRenamed = backupFile.rename(this->configFileFullPath);
        ASSERT_TRUE(fileRenamed) << "SetUp: Failed to rename " + backupFilePath.toStdString() + " back to " + configFileFullPath.toStdString();
    }

    // Check if the file already exists and if yes, rename it
    QFile file(this->configFileFullPath);
    bool fileExists = file.exists();
    if (fileExists) {
        bool fileRenamed = file.rename(this->backupFilePath);
        ASSERT_TRUE(fileRenamed) << "SetUp: Failed to rename " + configFileFullPath.toStdString() + " to " + backupFilePath.toStdString();
    }
}

void RUN_OP_SIMULATION_MANAGER_TEST::TearDown() {
    QThread::sleep(2);

    // Delete the newly created file
    bool removed = QFile::remove(this->configFileFullPath);
    ASSERT_TRUE(removed) << "TearDown: Failed to remove the test config file: " + this->configFileFullPath.toStdString();

    QDir dir(this->openPassCoreDir);
    foreach(QString file, dir.entryList(QDir::Files)) {
        QString filePath = dir.absoluteFilePath(file);
        if (filePath.contains("TEST_TEMP")) {
            removed = QFile::remove(filePath);
            ASSERT_TRUE(removed) << "TearDown: Failed to remove the test file: " + filePath.toStdString();
        }
    }

    foreach(QString directory, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString innerDir = dir.absoluteFilePath(directory);
        if (innerDir.contains("TEST_TEMP")) {
            removed = QDir(innerDir).removeRecursively();
            ASSERT_TRUE(removed) << "TearDown: Failed to recursively remove the folder: " + innerDir.toStdString();
        }
    }

    // Rename the backup file back to its original name
    QFile backupFile(this->backupFilePath);
    if (backupFile.exists()) {
        bool fileRenamed = backupFile.rename(this->configFileFullPath);
        ASSERT_TRUE(fileRenamed) << "TearDown: Failed to rename " + backupFilePath.toStdString() + " back to " + configFileFullPath.toStdString();
    }
}

void RUN_OP_SIMULATION_MANAGER_TEST::checkLogValues(const QString& logContent,const QString& expectedLogLevel,const QString& expectedLogFile,
                        const QString& expectedLibrary,const QString& expectedNumSimulations,const QString& expectedSimulationExec) 
{
    ASSERT_FALSE(logContent.isEmpty()) << "Test: generated log file has no contents";

    struct LogCheck {
        QString expectedValue;
        QRegularExpression regex;
        QString description;
    };

    QVector<LogCheck> checks = {
        {expectedLogLevel, QRegularExpression("log level: (\\d+)"), "log level"},
        {expectedLogFile, QRegularExpression("log file opSimulationManager: ([^\\n]+)"), "log file"},
        {expectedLibrary, QRegularExpression("libraries: ([^\\n]+)"), "libraries"},
        {expectedNumSimulations, QRegularExpression("number of simulations: (\\d+)"), "number of simulations"},
        {expectedSimulationExec, QRegularExpression("simulation: ([^\\n]+)"), "simulation execution path"}
    };

    for (const auto& check : checks) {
        auto match = check.regex.match(logContent);
        ASSERT_EQ(match.captured(1), check.expectedValue) << QString("Test: %1 - captured: %2, expected: %3")
            .arg(check.description).arg(match.captured(1)).arg(check.expectedValue).toStdString();
    }
}

TEST_F(RUN_OP_SIMULATION_MANAGER_TEST, Run_Op_Simulation_Manager_1_Simulation_Positive) {
    QString errorMsg;
    // Create XML config file
    configXml1Simulation.replace("$$CORE_DIR$$", this->openPassCoreDir + QStringLiteral(""));
    bool fileCreated = TestHelpers::createAndCheckFile(this->configFileFullPath,configXml1Simulation);
    ASSERT_TRUE(fileCreated) << "Test: " << this->configFileFullPath.toStdString() << " test sim config file could not be created";

    bool result = OPGUICore::api_run_opSimulationManager(this->response,errorMsg); 

    // Check that the response is ok
    ASSERT_TRUE(result) << "Test: runOpSimulationManager returned failed";
    ASSERT_EQ(this->response.getResponse(), this->response200Expected.getResponse()) 
        << "Test: run op simulation function did not return expected 200 response. "
        << "Expected: " << this->response200Expected.getResponse().toStdString() 
        << ", Got: " << this->response.getResponse().toStdString();

    //check if the log file of the simulation manager exists
    QThread::sleep(1);
    QFile simLogFile(TestHelpers::joinPaths(this->openPassCoreDir,"opSimulationManager.log"));
    ASSERT_TRUE(simLogFile.exists()) << "Test: Sim simulation manager log file was not created:" + simLogFile.fileName().toStdString();

    QDomElement root = TestHelpers::parseXMLString(configXml1Simulation);
    QDomElement simulationConfigs = root.firstChildElement("simulationConfigs");
    QDomElement simulationConfig = simulationConfigs.firstChildElement("simulationConfig");
    
    int countSimulations = 0;

    while(!simulationConfig.isNull()) {
        QString resultsDir = simulationConfig.firstChildElement("results").text();

        //check if results folder for the specific sim exists
        QThread::sleep(1);
        QDir dir(resultsDir);
        ASSERT_TRUE(dir.exists()) << "Test: No results folder created:" + resultsDir.toStdString();

        countSimulations++;

        // Move to the next simulationConfig element
        simulationConfig = simulationConfig.nextSiblingElement("simulationConfig");
    }
    QThread::sleep(1);
    QString logContents=TestHelpers::readFile(TestHelpers::joinPaths(this->openPassCoreDir,"opSimulationManager.log"));

    //check if log file contains errors

    checkLogValues(logContents
                    ,"5"
                    ,TestHelpers::joinPaths(this->openPassCoreDir,"opSimulationManager.log")
                    ,"modules_TEST_TEMP"
                    ,QString::number(countSimulations)
                    ,this->opSimulationExe);
}

TEST_F(RUN_OP_SIMULATION_MANAGER_TEST, Run_Op_Simulation_Manager_2_Simulations_Positive) {
    QString errorMsg;
    // Create XML config file
    configXml2Simulation.replace("$$CORE_DIR$$", this->openPassCoreDir + QStringLiteral(""));
    bool fileCreated = TestHelpers::createAndCheckFile(this->configFileFullPath,configXml2Simulation);
    ASSERT_TRUE(fileCreated) << "Test: " << this->configFileFullPath.toStdString() << " test sim config file could not be created";

    bool result = OPGUICore::api_run_opSimulationManager(this->response,errorMsg);

    // Check that the response is ok
    ASSERT_TRUE(result) << "Test: runOpSimulationManager returned failed";
    ASSERT_EQ(this->response.getResponse(), this->response200Expected.getResponse()) 
        << "Test: run op simulation function did not return expected 200 response. "
        << "Expected: " << this->response200Expected.getResponse().toStdString() 
        << ", Got: " << this->response.getResponse().toStdString();
    
    //check if the log file of the simulation manager exists
    QThread::sleep(1);
    QFile simLogFile(TestHelpers::joinPaths(this->openPassCoreDir,"opSimulationManager.log"));
    ASSERT_TRUE(simLogFile.exists()) << "Test: Sim simulation manager log file was not created:" + simLogFile.fileName().toStdString();

    QDomElement root = TestHelpers::parseXMLString(configXml2Simulation);
    QDomElement simulationConfigs = root.firstChildElement("simulationConfigs");
    QDomElement simulationConfig = simulationConfigs.firstChildElement("simulationConfig");
    
    int countSimulations = 0;

    while(!simulationConfig.isNull()) {
        QString resultsDir = simulationConfig.firstChildElement("results").text();

        //check if results folder for the specific sim exists
        QThread::sleep(1);
        QDir dir(resultsDir);
        ASSERT_TRUE(dir.exists()) << "Test: No results folder created:" + resultsDir.toStdString();

        countSimulations++;

        // Move to the next simulationConfig element
        simulationConfig = simulationConfig.nextSiblingElement("simulationConfig");
    }
    QThread::sleep(1);
    QString logContents=TestHelpers::readFile(TestHelpers::joinPaths(this->openPassCoreDir,"opSimulationManager.log"));

    //check if log file contains errors

    checkLogValues(logContents
                    ,"5"
                    ,TestHelpers::joinPaths(this->openPassCoreDir,"opSimulationManager.log")
                    ,"modules_TEST_TEMP"
                    ,QString::number(countSimulations)
                    ,this->opSimulationExe);
}


TEST_F(RUN_OP_SIMULATION_MANAGER_TEST, Run_Op_Simulation_Manager_Negative_No_exe) {
    QString errorMsg;
    QString managerExeFullPath = OPGUICoreGlobalConfig::getInstance().fullPathOpSimulationManagerExe();
    configXmlSimulationWrong.replace("$$CORE_DIR$$", this->openPassCoreDir + QStringLiteral(""));
    
    // Ensure that the simulation config file is correctly created.
    bool fileCreated = TestHelpers::createAndCheckFile(this->configFileFullPath, configXmlSimulationWrong);
    ASSERT_TRUE(fileCreated) << "Test: " << this->configFileFullPath.toStdString() << " test sim config file could not be created";

    QFile execFile(managerExeFullPath);
    ASSERT_TRUE(execFile.exists()) << "Test: " << managerExeFullPath.toStdString() << " exec file does not exist";

    QString backupPath = managerExeFullPath + QStringLiteral("_bk");

    // Rename the original exe to its backup name.
    ASSERT_TRUE(execFile.rename(backupPath)) 
        << "Test: Failed to rename the opSimulationManager exe file from" 
        << managerExeFullPath.toStdString() << " to its backup name " 
        << backupPath.toStdString();

    bool result = OPGUICore::api_run_opSimulationManager(this->response,errorMsg);
    
    // Expect that process can not be started as the exec is not found.
    EXPECT_FALSE(result) << "Test: runOpSimulationManager returned ok but should return fail";

    // Rename the backed-up exe back to its original name.
    ASSERT_TRUE(execFile.rename(managerExeFullPath))
        << "Test: Failed to rename the backed up opSimulationManager exe file from " 
        << backupPath.toStdString() << " back to " 
        << managerExeFullPath.toStdString();
}

TEST_F(RUN_OP_SIMULATION_MANAGER_TEST, Run_Op_Simulation_Manager_Negative_No_opSimulation_exe) {
    QString errorMsg;
    QString exeFullPath = OPGUICoreGlobalConfig::getInstance().fullPathOpSimulationExe();
    configXmlSimulationWrong.replace("$$CORE_DIR$$", this->openPassCoreDir + QStringLiteral(""));
    
    // Ensure that the simulation config file is correctly created.
    bool fileCreated = TestHelpers::createAndCheckFile(this->configFileFullPath, configXmlSimulationWrong);
    ASSERT_TRUE(fileCreated) << "Test: " << this->configFileFullPath.toStdString() << " test sim config file could not be created";

    QFile execFile(exeFullPath);
    ASSERT_TRUE(execFile.exists()) << "Test: " << exeFullPath.toStdString() << " exec file does not exist";

    QString backupPath = exeFullPath + QStringLiteral("_bk");

    // Rename the original exe to its backup name.
    ASSERT_TRUE(execFile.rename(backupPath)) 
        << "Test: Failed to rename the opSimulation exe file from" 
        << exeFullPath.toStdString() << " to its backup name " 
        << backupPath.toStdString();

    bool result = OPGUICore::api_run_opSimulationManager(this->response,errorMsg);
    
    // Expect that process can not be started as the exec is not found.
    EXPECT_FALSE(result) << "Test: runOpSimulationManager returned ok but should return fail";

    // Rename the backed-up exe back to its original name.
    ASSERT_TRUE(execFile.rename(exeFullPath))
        << "Test: Failed to rename the backed up opSimulation exe file from " 
        << backupPath.toStdString() << " back to " 
        << exeFullPath.toStdString();
}
