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
#include <QDateTime>

#include "test_OPGUIConvertToConfigs.h"
#include "OPGUICoreGlobalConfig.h"
#include "OAIHelpers.h"
#include "test_helpers.h"
#include "OPGUIPCMSimulation.h"
#include "OPGUICore.h"

QString systemConfigExp1Xml = QStringLiteral(
    R"(<?xml version="1.0" encoding="UTF-8"?>
       <systems>
        <system>
        <id>0</id>
        <title>agent_1_test</title>
        <priority>0</priority>
        <components>
        <component>
            <id>0</id>
            <library>Algorithm_Test</library>
            <title>Algorithm_Test</title>
            <schedule>
            <offset>0</offset>
            <cycle>10</cycle>
            <response>0</response>
            <priority>30</priority>
            </schedule>
            <parameters>
            <parameter>
            <!--parameter's title: Driver aggressiveness-->
            <id>0</id>
            <type>double</type>
            <unit/>
            <value>1</value>
            </parameter>
            <parameter>
            <!--parameter's title: Max. engine power-->
            <id>1</id>
            <type>double</type>
            <unit>W</unit>
            <value>100000</value>
            </parameter>
            <parameter>
            <!--parameter's title: Min. brake torque-->
            <id>2</id>
            <type>double</type>
            <unit>Nm</unit>
            <value>-10000</value>
            </parameter>
            <parameter>
            <!--parameter's title: Kp of the pedals PID control-->
            <id>3</id>
            <type>double</type>
            <unit/>
            <value>-0.5</value>
            </parameter>
            <parameter>
            <!--parameter's title: Ki of the pedals PID control-->
            <id>4</id>
            <type>double</type>
            <unit/>
            <value>-0.4</value>
            </parameter>
            <parameter>
            <!--parameter's title: Kd of the pedals PID control-->
            <id>5</id>
            <type>double</type>
            <unit/>
            <value>0.0</value>
            </parameter>
            <parameter>
            <!--parameter's title: Kp of the steering PID control-->
            <id>6</id>
            <type>double</type>
            <unit/>
            <value>-18</value>
            </parameter>
            <parameter>
            <!--parameter's title: Ki of the steering PID control-->
            <id>7</id>
            <type>double</type>
            <unit/>
            <value>-0.6</value>
            </parameter>
            <parameter>
            <!--parameter's title: Kd of the steering PID control-->
            <id>8</id>
            <type>double</type>
            <unit/>
            <value>0.0</value>
            </parameter>
            </parameters>
            <position>
            <x>296</x>
            <y>20</y>
            </position>
        </component>
        <component>
            <id>1</id>
            <library>Action_Test</library>
            <title>Action_Test</title>
            <schedule>
            <offset>0</offset>
            <cycle>10</cycle>
            <response>0</response>
            <priority>1</priority>
            </schedule>
            <parameters>
            <parameter>
            <!--parameter's title: Spring coefficient-->
            <id>0</id>
            <type>double</type>
            <unit/>
            <value>1.2e+06</value>
            </parameter>
            <parameter>
            <!--parameter's title: Damper coefficient-->
            <id>1</id>
            <type>double</type>
            <unit/>
            <value>12000</value>
            </parameter>
            </parameters>
            <position>
            <x>815</x>
            <y>692</y>
            </position>
        </component>
        </components>
        <connections>
        <connection>
            <id>0</id>
            <source>
            <component>1</component>
            <output>0</output>
            </source>
            <target>
            <component>0</component>
            <input>0</input>
            </target>
        </connection>
        <connection>
            <id>1</id>
            <source>
            <component>0</component>
            <output>0</output>
            </source>
            <target>
            <component>1</component>
            <input>0</input>
            </target>
        </connection>
        </connections>
        </system>
       <systems>)"
);

QString systemConfigExp2Xml = QStringLiteral(
    R"(<?xml version="1.0" encoding="UTF-8"?>
      <systems>
        <system>
        <id>0</id>
        <title>agent_2_test</title>
        <priority>0</priority>
        <components>
        <component>
            <id>0</id>
            <library>Algorithm_Test</library>
            <title>Algorithm_Test</title>
            <schedule>
            <offset>0</offset>
            <cycle>10</cycle>
            <response>0</response>
            <priority>30</priority>
            </schedule>
            <parameters>
            <parameter>
            <!--parameter's title: Driver aggressiveness-->
            <id>0</id>
            <type>double</type>
            <unit/>
            <value>1</value>
            </parameter>
            <parameter>
            <!--parameter's title: Max. engine power-->
            <id>1</id>
            <type>double</type>
            <unit>W</unit>
            <value>100000</value>
            </parameter>
            <parameter>
            <!--parameter's title: Min. brake torque-->
            <id>2</id>
            <type>double</type>
            <unit>Nm</unit>
            <value>-10000</value>
            </parameter>
            <parameter>
            <!--parameter's title: Kp of the pedals PID control-->
            <id>3</id>
            <type>double</type>
            <unit/>
            <value>-0.5</value>
            </parameter>
            <parameter>
            <!--parameter's title: Ki of the pedals PID control-->
            <id>4</id>
            <type>double</type>
            <unit/>
            <value>-0.4</value>
            </parameter>
            <parameter>
            <!--parameter's title: Kd of the pedals PID control-->
            <id>5</id>
            <type>double</type>
            <unit/>
            <value>0.0</value>
            </parameter>
            <parameter>
            <!--parameter's title: Kp of the steering PID control-->
            <id>6</id>
            <type>double</type>
            <unit/>
            <value>-18</value>
            </parameter>
            <parameter>
            <!--parameter's title: Ki of the steering PID control-->
            <id>7</id>
            <type>double</type>
            <unit/>
            <value>-0.6</value>
            </parameter>
            <parameter>
            <!--parameter's title: Kd of the steering PID control-->
            <id>8</id>
            <type>double</type>
            <unit/>
            <value>0.0</value>
            </parameter>
            </parameters>
            <position>
            <x>296</x>
            <y>20</y>
            </position>
        </component>
        <component>
            <id>1</id>
            <library>Action_Test</library>
            <title>Action_Test</title>
            <schedule>
            <offset>0</offset>
            <cycle>10</cycle>
            <response>0</response>
            <priority>1</priority>
            </schedule>
            <parameters>
            <parameter>
            <!--parameter's title: Spring coefficient-->
            <id>0</id>
            <type>double</type>
            <unit/>
            <value>1.2e+06</value>
            </parameter>
            <parameter>
            <!--parameter's title: Damper coefficient-->
            <id>1</id>
            <type>double</type>
            <unit/>
            <value>12000</value>
            </parameter>
            </parameters>
            <position>
            <x>815</x>
            <y>692</y>
            </position>
        </component>
        </components>
        <connections>
        <connection>
            <id>0</id>
            <source>
            <component>1</component>
            <output>0</output>
            </source>
            <target>
            <component>0</component>
            <input>0</input>
            </target>
        </connection>
        <connection>
            <id>1</id>
            <source>
            <component>0</component>
            <output>0</output>
            </source>
            <target>
            <component>1</component>
            <input>0</input>
            </target>
        </connection>
        </connections>
        </system>
       <systems>)"
);

QString configsReqJSON = QStringLiteral(
    R"({
  "selectedSystems": [
    {
      "name": "agent 1",
      "file": "/workspace/systems/agent_1.xml"
    },
    {
      "name": "agent 2",
      "file": "/workspace/systems/agent_2.xml"
    },
    {
      "name": "other",
      "file": "/workspace/systems/agent_2.xml"
    }
  ],
  "selectedExperiments": [
    "1"
  ]
})");

bool CONVERT_TO_CONFIGS_TEST::parseConfigsReqJson(const QString& jsonStr, OpenAPI::OAIConfigsRequest& oai_configs_request) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());

    if (doc.isNull()) {
        qDebug() << "Failed to create JSON doc.";
        return false;
    }

    if (!doc.isObject()) {
        qDebug() << "JSON doc is not an object.";
        return false;
    }

    QJsonObject jsonObj = doc.object();

    OpenAPI::fromJsonValue(oai_configs_request, jsonObj);

    return true;
}

void CONVERT_TO_CONFIGS_TEST::InitSimulation(const QString &pcm_db_path) {
    OPGUIPCMSimulation::PCMSimulation *sim = new OPGUIPCMSimulation::PCMSimulation();
    QString errorMsg;

    bool simInited = sim->Init(pcm_db_path,config->fullPathModulesFolder(),config->pathConvertedCases(),config->fullPathOpSimulationExe(),errorMsg);

    OPGUICore::setSimulation(sim);
}

void CONVERT_TO_CONFIGS_TEST::DeleteSimulation() {
    OPGUIPCMSimulation::PCMSimulation *sim = OPGUICore::getSimulation(); 

    if (sim) {
        delete sim;
        OPGUICore::setSimulation(nullptr);
    }
}

void CONVERT_TO_CONFIGS_TEST::verifyConfigsDirectoryStructure(const QString &basePath) {
    QDir base(basePath);

    // Check if base directory exists
    ASSERT_TRUE(base.exists()) << "Base directory does not exist: " + basePath.toStdString();

    // Check sub-directory "1" exists inside basePath
    QDir dir1(base.absoluteFilePath("1"));
    ASSERT_TRUE(dir1.exists()) << "Directory '1' not found inside the base directory.";

    // Check sub-directory "0-0-0" inside "1"
    QDir subDir0(dir1.absoluteFilePath("0-0-0"));
    ASSERT_TRUE(subDir0.exists()) << "'0-0-0' directory not found inside '1'.";

    // Check sub-directory "configs" under "0-0-0"
    QDir configsDir(subDir0.absoluteFilePath("configs"));
    ASSERT_TRUE(configsDir.exists()) << "'configs' directory not found inside '0-0-0'.";

    QStringList configsFiles = {"VehicleModelsCatalog.xosc", "ProfilesCatalog.xml", "Scenario.xosc", 
                                "SceneryConfiguration.xodr", "simulationConfig.xml", "SystemConfig.xml"};

    // Check that each file exists in "configs" directory
    for (const QString &filename : configsFiles) {
        ASSERT_TRUE(QFileInfo::exists(configsDir.absoluteFilePath(filename)))
            << "File " + filename.toStdString() + " not found inside 'configs' directory.";
    }

    // Check sub-directory "results" under "0-0-0" and ensure it's empty
    QDir resultsDir(subDir0.absoluteFilePath("results"));
    ASSERT_TRUE(resultsDir.exists()) << "'results' directory not found inside '0-0-0'.";
    ASSERT_TRUE(resultsDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty()) 
        << "'results' directory is not empty.";
}


void CONVERT_TO_CONFIGS_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
            
    config = &OPGUICoreGlobalConfig::getInstance();
    QString testFileName = QStringLiteral("test_scene.db");
    this->destinationFilePathTestDB = TestHelpers::joinPaths(config->pathOpenpassCore(),testFileName);

    //create temporary directory for the test
    this->testDirFullPath = TestHelpers::joinPaths(config->workspace(),"test");
    QDir dir(config->workspace());
    ASSERT_TRUE(dir.exists())<<"Workspace not found";

    //if test dir exists remove it
    QDir dirTest(this->testDirFullPath);
    if(dirTest.exists()){
        ASSERT_TRUE(dirTest.removeRecursively());
    }

    ASSERT_TRUE(dir.mkpath(this->testDirFullPath))<<"Could not create test dir under:"<<this->testDirFullPath.toStdString();
    config->setPathConvertedCases(this->testDirFullPath);

    this->response200Expected.setResponse("Succesfully created config files under:"+this->testDirFullPath);
    // Check if the source file exists
    ASSERT_TRUE(QFile::exists(testFileName)) << "Test: Source .db file doesn't exist at build directory: " << testFileName.toStdString();

    // Copy the db file removing any old one if was there
    QFile::remove(this->destinationFilePathTestDB);
    ASSERT_TRUE(QFile::copy(testFileName, this->destinationFilePathTestDB)) << "Test: Failed to copy the database file from " 
                << testFileName.toStdString() 
                << " to " 
                << this->destinationFilePathTestDB.toStdString() 
                << ".";

    // create systems folder for test and add the system xml files
    QString testDirSystems = QStringLiteral("test_systems");
    OPGUICoreGlobalConfig::getInstance().setSystemsFolder("test_systems");
    this->testDirSystemsPath = TestHelpers::joinPaths(config->workspace(),testDirSystems);
    QDir dirSysTest(this->testDirSystemsPath);
    if(dirSysTest.exists()){
        ASSERT_TRUE(dirSysTest.removeRecursively()) << "Failed to remove existing systems test directory: " << this->testDirSystemsPath.toStdString();
    }
    ASSERT_TRUE(dirSysTest.mkpath(this->testDirSystemsPath)) << "Failed to create systems test directory at: " << this->testDirSystemsPath.toStdString();
    //add the two systems
    this->filePathSys1 = TestHelpers::joinPaths(this->testDirSystemsPath,"agent_1.xml");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePathSys1, systemConfigExp1Xml))<<"Could not save xml file as: "+filePathSys1.toStdString();
    this->filePathSys2 = TestHelpers::joinPaths(this->testDirSystemsPath,"agent_2.xml");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePathSys2, systemConfigExp2Xml))<<"Could not save xml file as: "+filePathSys2.toStdString();
}

void CONVERT_TO_CONFIGS_TEST::TearDown()  {
    config = &OPGUICoreGlobalConfig::getInstance();
    //remove db file from the core
    EXPECT_TRUE(QFile::remove(this->destinationFilePathTestDB)) << "Failed to remove the copied database file in: "+this->destinationFilePathTestDB.toStdString();
    this->DeleteSimulation();
    
    QDir dir(this->testDirFullPath);
    if(dir.exists()){
        EXPECT_TRUE(dir.removeRecursively()) << "Test: Failed to remove the directory: " 
                    << this->testDirFullPath.toStdString() 
                    << ".";
    }
    QDir dirSysTest(this->testDirSystemsPath);
    if(dirSysTest.exists()){
        ASSERT_TRUE(dirSysTest.removeRecursively()) << "Failed to remove systems test directory: " << this->testDirSystemsPath.toStdString();
    }
    config->setPathConvertedCases(config->workspace());  
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_invalid_request_empty_NEGATIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    //call convert to configs
    bool result = OPGUICore::api_convert_to_configs(configsReq,this->response,errorMsg);

    ASSERT_FALSE(result)<<"Test: Result of api_send_PCM was expected to be false";

    ASSERT_TRUE(errorMsg.contains("Invalid JSON request"))<<"Error message was expected to contain 'Invalid JSON request' but was "+errorMsg.toStdString();
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_list_of_experiments_empty_NEGATIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    ASSERT_TRUE(parseConfigsReqJson(configsReqJSON, configsReq))<< "Failed preparing the test, unable parse the config request object JSON for the test";

    configsReq.setSelectedExperiments(QList<QString>());

    //call convert to configs
    bool result = OPGUICore::api_convert_to_configs(configsReq,this->response,errorMsg);

    ASSERT_FALSE(result)<<"Test: Result of api_send_PCM was expected to be false";

    ASSERT_TRUE(errorMsg.contains("Config generation failed as no experiments where selected"))<<"Error message was expected to contain 'Config generation failed as no experiments where selected' but was "+errorMsg.toStdString();
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_list_of_systems_empty_NEGATIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    ASSERT_TRUE(parseConfigsReqJson(configsReqJSON, configsReq))<< "Failed preparing the test, unable parse the config request object JSON for the test";

    configsReq.setSelectedSystems(QList<OpenAPI::OAISystemRef>());

    //call convert to configs
    bool result = OPGUICore::api_convert_to_configs(configsReq,this->response,errorMsg);

    ASSERT_FALSE(result)<<"Test: Result of api_send_PCM was expected to be false";

    ASSERT_TRUE(errorMsg.contains("Config generation failed since list of agents not properly set"))<<"Error message was expected to contain 'Config generation failed since list of agents not properly set' but was "+errorMsg.toStdString();
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_simulation_not_inited_NEGATIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    ASSERT_TRUE(parseConfigsReqJson(configsReqJSON, configsReq))<< "Failed preparing the test, unable parse the config request object JSON for the test";

    bool result = OPGUICore::api_convert_to_configs(configsReq, this->response,errorMsg);

    EXPECT_FALSE(result)<<"Test: Result of api_send_PCM was expected to be false";

    ASSERT_TRUE(errorMsg.contains("Config generation failed since PCM plugin was not initialised"))<<"Error message was expected to contain 'Config generation failed since PCM plugin was not initialised' but was "+errorMsg.toStdString();
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_simulation_inited_no_agent_file_1_NEGATIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    ASSERT_TRUE(parseConfigsReqJson(configsReqJSON, configsReq))<< "Failed preparing the test, unable parse the config request object JSON for the test";

    QList<OpenAPI::OAISystemRef> systems = configsReq.getSelectedSystems();
    OpenAPI::OAISystemRef &systemToModify = systems[0];
    systemToModify.setFile("/some/not_existing_path/no_file.xml");
    configsReq.setSelectedSystems(systems);

    this->InitSimulation(this->destinationFilePathTestDB);
    //expect config conversion error as experiment does not exist
    bool result = OPGUICore::api_convert_to_configs(configsReq, this->response,errorMsg);

    EXPECT_FALSE(result)<<"Test: Result of api_convert_to_configs was expected to be false";

    ASSERT_TRUE(errorMsg.contains("Agent Car 1 file path empty / invalid"))<<"Error message was expected to contain 'Agent Car 1 file path empty / invalid' but was "+errorMsg.toStdString();
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_simulation_inited_no_agent_file_2_NEGATIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    ASSERT_TRUE(parseConfigsReqJson(configsReqJSON, configsReq))<< "Failed preparing the test, unable parse the config request object JSON for the test";

    QList<OpenAPI::OAISystemRef> systems = configsReq.getSelectedSystems();
    OpenAPI::OAISystemRef &systemToModify1 = systems[0];
    systemToModify1.setFile(this->filePathSys1);
    OpenAPI::OAISystemRef &systemToModify2 = systems[1];
    systemToModify2.setFile("/some/not_existing_path/no_file.xml");
    configsReq.setSelectedSystems(systems);

    this->InitSimulation(this->destinationFilePathTestDB);
    //expect config conversion error as experiment does not exist
    bool result = OPGUICore::api_convert_to_configs(configsReq, this->response,errorMsg);

    EXPECT_FALSE(result)<<"Test: Result of api_convert_to_configs was expected to be false";

    ASSERT_TRUE(errorMsg.contains("Agent Car 2 file path empty / invalid"))<<"Error message was expected to contain 'Agent Car 2 file path empty / invalid' but was "+errorMsg.toStdString();
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_simulation_inited_no_agent_other_NEGATIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    ASSERT_TRUE(parseConfigsReqJson(configsReqJSON, configsReq))<< "Failed preparing the test, unable parse the config request object JSON for the test";

    QList<OpenAPI::OAISystemRef> systems = configsReq.getSelectedSystems();
    OpenAPI::OAISystemRef &systemToModify1 = systems[0];
    systemToModify1.setFile(this->filePathSys1);
    OpenAPI::OAISystemRef &systemToModify2 = systems[1];
    systemToModify2.setFile(this->filePathSys2);
    OpenAPI::OAISystemRef &systemToModify3 = systems[2];
    systemToModify3.setFile("/some/not_existing_path/no_file.xml");
    configsReq.setSelectedSystems(systems);

    this->InitSimulation(this->destinationFilePathTestDB);
    //expect config conversion error as experiment does not exist
    bool result = OPGUICore::api_convert_to_configs(configsReq, this->response,errorMsg);

    EXPECT_FALSE(result)<<"Test: Result of api_convert_to_configs was expected to be false";

    ASSERT_TRUE(errorMsg.contains("Agent Car Other file path empty / invalid"))<<"Error message was expected to contain 'Agent Car Other file path empty / invalid' but was "+errorMsg.toStdString();
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_simulation_inited_wrong_experiment_NEGATIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    ASSERT_TRUE(parseConfigsReqJson(configsReqJSON, configsReq))<< "Failed preparing the test, unable parse the config request object JSON for the test";

    configsReq.setSelectedExperiments(QList<QString>() << "XXXX");

    this->InitSimulation(this->destinationFilePathTestDB);
    //expect config conversion error as experiment does not exist
    bool result = OPGUICore::api_convert_to_configs(configsReq, this->response,errorMsg);

    EXPECT_FALSE(result)<<"Test: Result of api_convert_to_configs was expected to be false";
}

TEST_F(CONVERT_TO_CONFIGS_TEST, convert_to_configs_simulation_inited_POSITIVE) {
    OpenAPI::OAIConfigsRequest configsReq;
    QString errorMsg;

    ASSERT_TRUE(parseConfigsReqJson(configsReqJSON, configsReq))<< "Failed preparing the test, unable parse the config request object JSON for the test";

    configsReq.setSelectedExperiments(QList<QString>() << "1");

    QList<OpenAPI::OAISystemRef> systems = configsReq.getSelectedSystems();
    OpenAPI::OAISystemRef &systemToModify1 = systems[0];
    systemToModify1.setFile(this->filePathSys1);
    OpenAPI::OAISystemRef &systemToModify2 = systems[1];
    systemToModify2.setFile(this->filePathSys2);
    OpenAPI::OAISystemRef &systemToModify3 = systems[2];
    systemToModify3.setFile(this->filePathSys2);
    configsReq.setSelectedSystems(systems);

    this->InitSimulation(this->destinationFilePathTestDB);
    
    bool result = OPGUICore::api_convert_to_configs(configsReq, this->response,errorMsg);

    ASSERT_TRUE(result)<<"Test: Result of api_convert_to_configs was expected to be true";
    EXPECT_EQ(this->response.getResponse(), this->response200Expected.getResponse())
    << "Test: Response was not the expected. Got: " << this->response.getResponse().toStdString() 
    << ", Expected: " << this->response200Expected.getResponse().toStdString() ;
    
    //check that config folders where created
    verifyConfigsDirectoryStructure(this->testDirFullPath);

}











