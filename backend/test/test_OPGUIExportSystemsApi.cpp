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

#pragma once

#include "gtest/gtest.h"
#include <QString>
#include <QDebug>
#include <QDateTime>

#include "test_OPGUIExportSystemsApi.h"
#include "OPGUICoreGlobalConfig.h"
#include "OPGUICore.h"
#include "OAIHelpers.h"
#include "test_helpers.h"
#include "OAIComponentUI.h"
#include "OAIError500.h"
#include "OAISystemUI.h"

QString componentSysExp1Xml = QStringLiteral(
    R"(<component>
      <type>AlgorithmTest</type>
      <title>Algorithm_Test</title>
      <schedule>
        <offset>0</offset>
        <cycle>10</cycle>
        <response>0</response>
      </schedule>
      <parameters>
        <parameter>
          <id>0</id>
          <type>double</type>
          <title>Driver aggressiveness</title>
          <unit></unit>
          <value>1.0</value>
        </parameter>
        <parameter>
          <id>1</id>
          <type>double</type>
          <title>Max. engine power</title>
          <unit>W</unit>
          <value>100000</value>
        </parameter>
        <parameter>
          <id>2</id>
          <type>int</type>
          <title>Min. brake torque</title>
          <unit>Nm</unit>
          <value>100</value>
        </parameter>
        <parameter>
          <id>3</id>
          <type>bool</type>
          <title>Kp of the pedals PID control</title>
          <unit></unit>
          <value>true</value>
        </parameter>
        <parameter>
          <id>4</id>
          <type>string</type>
          <title>Ki of the pedals PID control</title>
          <unit></unit>
          <value>string param</value>
        </parameter>
      </parameters>
      <inputs>
        <input>
          <id>0</id>
          <type>TrajectoryEvent</type>
          <title>Desired trajectory</title>
          <unit></unit>
          <cardinality>1</cardinality>
        </input>
      </inputs>
      <outputs>
        <output>
          <id>0</id>
          <type>ControlData</type>
          <title>Control</title>
          <unit></unit>
        </output>
      </outputs>
    </component>)"
);

QString componentSysExp2Xml = QStringLiteral(
    R"(<component>
    <type>Action</type>
    <title>Action_Test</title>
    <schedule>
        <offset>0</offset>
        <cycle>10</cycle>
        <response>0</response>
    </schedule>
    <parameters>
        <parameter>
        <id>0</id>
        <type>double</type>
        <title>Spring coefficient</title>
        <unit></unit>
        <value>1200000</value>
        </parameter>
        <parameter>
        <id>1</id>
        <type>double</type>
        <title>Damper coefficient</title>
        <unit></unit>
        <value>12000</value>
        </parameter>
    </parameters>
    <inputs>
        <input>
        <id>0</id>
        <type>VectorDouble</type>
        <title>InertiaForce</title>
        <unit>N</unit>
        <cardinality>1</cardinality>
        </input>
    </inputs>
    <outputs>
        <output>
        <id>0</id>
        <type>VectorDouble</type>
        <title>VerticalForce</title>
        <unit>N</unit>
        </output>
    </outputs>
    </component>)"
);

QString systemsExp1Xml = QStringLiteral(
    R"(<?xml version="1.0" encoding="UTF-8"?>
       <systems>
        <system>
        <id>0</id>
        <title>System_test</title>
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
      </systems>)"
);

QString systemsExp1JSON = QStringLiteral(
    R"([
    {
        "components": [
            {
                "id": 0,
                "inputs": [
                    {
                        "cardinality": 1,
                        "id": 0,
                        "title": "Desired trajectory",
                        "type": "TrajectoryEvent",
                        "unit": ""
                    }
                ],
                "library": "Algorithm_Test",
                "outputs": [
                    {
                        "id": 0,
                        "title": "Control",
                        "type": "ControlData",
                        "unit": ""
                    }
                ],
                "parameters": [
                    {
                        "id": 0,
                        "title": "Driver aggressiveness",
                        "type": "double",
                        "unit": "",
                        "value": "1"
                    },
                    {
                        "id": 1,
                        "title": "Max. engine power",
                        "type": "double",
                        "unit": "W",
                        "value": "100000"
                    },
                    {
                        "id": 2,
                        "title": "Min. brake torque",
                        "type": "double",
                        "unit": "Nm",
                        "value": "-10000"
                    },
                    {
                        "id": 3,
                        "title": "Kp of the pedals PID control",
                        "type": "double",
                        "unit": "",
                        "value": "-0.5"
                    },
                    {
                        "id": 4,
                        "title": "Ki of the pedals PID control",
                        "type": "double",
                        "unit": "",
                        "value": "-0.4"
                    },
                    {
                        "id": 5,
                        "title": "Kd of the pedals PID control",
                        "type": "double",
                        "unit": "",
                        "value": "0.0"
                    },
                    {
                        "id": 6,
                        "title": "Kp of the steering PID control",
                        "type": "double",
                        "unit": "",
                        "value": "-18"
                    },
                    {
                        "id": 7,
                        "title": "Ki of the steering PID control",
                        "type": "double",
                        "unit": "",
                        "value": "-0.6"
                    },
                    {
                        "id": 8,
                        "title": "Kd of the steering PID control",
                        "type": "double",
                        "unit": "",
                        "value": "0.0"
                    }
                ],
                "position": {
                    "x": 296,
                    "y": 20
                },
                "schedule": {
                    "cycle": 10,
                    "offset": 0,
                    "priority": 30,
                    "response": 0
                },
                "title": "Algorithm_Test",
                "type": "Algorithm"
            },
            {
                "id": 1,
                "inputs": [
                    {
                        "cardinality": 1,
                        "id": 0,
                        "title": "InertiaForce",
                        "type": "VectorDouble",
                        "unit": "N"
                    }
                ],
                "library": "Action_Test",
                "outputs": [
                    {
                        "id": 0,
                        "title": "VerticalForce",
                        "type": "VectorDouble",
                        "unit": "N"
                    }
                ],
                "parameters": [
                    {
                        "id": 0,
                        "title": "Spring coefficient",
                        "type": "double",
                        "unit": "",
                        "value": "1.2e+06"
                    },
                    {
                        "id": 1,
                        "title": "Damper coefficient",
                        "type": "double",
                        "unit": "",
                        "value": "12000"
                    }
                ],
                "position": {
                    "x": 815,
                    "y": 692
                },
                "schedule": {
                    "cycle": 10,
                    "offset": 0,
                    "priority": 1,
                    "response": 0
                },
                "title": "Action_Test",
                "type": "Action"
            }
        ],
        "connections": [
            {
                "id": 0,
                "source": {
                    "component": 1,
                    "output": 0
                },
                "target": {
                    "component": 0,
                    "input": 0
                }
            },
            {
                "id": 1,
                "source": {
                    "component": 0,
                    "output": 0
                },
                "target": {
                    "component": 1,
                    "input": 0
                }
            }
        ],
        "id": 0,
        "priority": 0,
        "title": "System_test",
        "deleted":false,
        "file":"test_system.xml"
    }
])"
);

bool EXPORT_SYSTEMS_TEST::parseSystemsJson(const QString& jsonStr, QList<OpenAPI::OAISystemUI>& oai_system_ui_list) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());

    if (doc.isNull()) {
        qDebug() << "Failed to create JSON doc.";
        return false;
    }

    if (!doc.isArray()) {
        qDebug() << "JSON doc is not an array.";
        return false;
    }

    QJsonArray jsonArray = doc.array();

    foreach (QJsonValue obj, jsonArray) {
        OpenAPI::OAISystemUI o;
        OpenAPI::fromJsonValue(o, obj);
        oai_system_ui_list.append(o);
    }

    return true;
}

void EXPORT_SYSTEMS_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";

    //ceck path to core exists
    QDir dirCore(OPGUICoreGlobalConfig::getInstance().pathOpenpassCore());
    ASSERT_TRUE(dirCore.exists())<<"Core directory not found under:"+OPGUICoreGlobalConfig::getInstance().pathOpenpassCore().toStdString();

    //ceck path to workspace exists
    QDir dirWkspc(OPGUICoreGlobalConfig::getInstance().pathOpenpassCore());
    ASSERT_TRUE(dirWkspc.exists())<<"Core directory not found under:"+OPGUICoreGlobalConfig::getInstance().workspace().toStdString();
    
    QString testDirComponents = QStringLiteral("test_components");
    OPGUICoreGlobalConfig::getInstance().setComponentsFolder("test_components");
    this->testDirComponentsPath = TestHelpers::joinPaths(OPGUICoreGlobalConfig::getInstance().pathOpenpassCore(),testDirComponents);

    QString testDirSystems = QStringLiteral("test_systems");
    OPGUICoreGlobalConfig::getInstance().setSystemsFolder("test_systems");
    this->testDirSystemsPath = TestHelpers::joinPaths(OPGUICoreGlobalConfig::getInstance().workspace(),testDirSystems);
    this->testSystemsFilePath = TestHelpers::joinPaths(this->testDirSystemsPath,"test_system.xml");

    // If test directories exists, remove them.
    QDir dirCompTest(this->testDirComponentsPath);
    if(dirCompTest.exists()){
        ASSERT_TRUE(dirCompTest.removeRecursively()) << "Failed to remove existing components test directory: " << this->testDirComponentsPath.toStdString();
    }
    QDir dirSysTest(this->testDirSystemsPath);
    if(dirSysTest.exists()){
        ASSERT_TRUE(dirSysTest.removeRecursively()) << "Failed to remove existing systems test directory: " << this->testDirSystemsPath.toStdString();
    }

    ASSERT_TRUE(dirCompTest.mkpath(this->testDirComponentsPath)) << "Failed to create components test directory at: " << this->testDirComponentsPath.toStdString();
    ASSERT_TRUE(dirSysTest.mkpath(this->testDirSystemsPath)) << "Failed to create systems test directory at: " << this->testDirSystemsPath.toStdString();
    
    //add the two used components used by the system in the test folder
    QString filePath = TestHelpers::joinPaths(this->testDirComponentsPath, "test_component_1.xml");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath, componentSysExp1Xml))<<"Could not save xml file as: "+filePath.toStdString();
    filePath = TestHelpers::joinPaths(this->testDirComponentsPath, "test_component_2.xml");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath, componentSysExp2Xml))<<"Could not save xml file as: "+filePath.toStdString();
}

void EXPORT_SYSTEMS_TEST::TearDown()  {
    QDir dirCompTest(this->testDirComponentsPath);
    if(dirCompTest.exists()){
        ASSERT_TRUE(dirCompTest.removeRecursively()) << "Failed to remove components test directory: " << this->testDirComponentsPath.toStdString();
    }
    QDir dirSysTest(this->testDirSystemsPath);
    if(dirSysTest.exists()){
        ASSERT_TRUE(dirSysTest.removeRecursively()) << "Failed to remove systems test directory: " << this->testDirSystemsPath.toStdString();
    }
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_1_systems_exported_POSITIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    ASSERT_TRUE(parseSystemsJson(systemsExp1JSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_TRUE(result) << "Export Systems API call failed";

    ASSERT_TRUE(this->response.getResponse().contains("Systems correctly exported."))<<"Error message was expected to contain 'Systems correctly exported.' but was "+this->response.getResponse().toStdString();

    //read exported file and compare with the one obtained
    // Read the content of the generated file
    QString actualXml = TestHelpers::readFile(this->testSystemsFilePath);

    ASSERT_EQ(
    TestHelpers::removeSpacesBetweenTags(systemsExp1Xml.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated systems xml file differ.\n"
        << "Expected XML:\n" << systemsExp1Xml.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();   
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_1_systems_exported_old_system_overwritten_POSITIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    ASSERT_TRUE(parseSystemsJson(systemsExp1JSON, systemsReq))
        << "Failed preparing the test, unable to parse the systems JSON for the test";

    // First operation to generate the initial file
    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response, errorMsg);
    ASSERT_TRUE(result) << "Export Systems API call failed";
    ASSERT_TRUE(this->response.getResponse().contains("Systems correctly exported."))
        << "Error message was expected to contain 'Systems correctly exported.' but was " + this->response.getResponse().toStdString();
    QString actualXml = TestHelpers::readFile(this->testSystemsFilePath);
    ASSERT_EQ(
        TestHelpers::removeSpacesBetweenTags(systemsExp1Xml.simplified()),
        TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated systems xml file differ.\n"
        << "Expected XML:\n" << systemsExp1Xml.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();

    // Temporarily modify the file to ensure it gets overwritten
    QString temporaryModification = "temporary modification to file";
    QFile file(this->testSystemsFilePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << temporaryModification;
        file.close();
    }

    // Perform the operation again to overwrite the file
    result = OPGUICore::api_export_systems_config(systemsReq, this->response, errorMsg);
    ASSERT_TRUE(result) << "Export Systems API call failed";
    ASSERT_TRUE(this->response.getResponse().contains("Systems correctly exported."))
        << "Error message was expected to contain 'Systems correctly exported.' but was " + this->response.getResponse().toStdString();

    // Read and compare the file again
    actualXml = TestHelpers::readFile(this->testSystemsFilePath);
    ASSERT_EQ(
        TestHelpers::removeSpacesBetweenTags(systemsExp1Xml.simplified()),
        TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated systems xml file differ.\n"
        << "Expected XML:\n" << systemsExp1Xml.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();

    // Make sure our temporary modification is gone, confirming the file was overwritten
    ASSERT_FALSE(actualXml.contains(temporaryModification))
        << "The temporary modification is still present, indicating the file was not overwritten.";
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_1_systems_exported_backup_level_1_system_POSITIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    ASSERT_TRUE(parseSystemsJson(systemsExp1JSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);
    ASSERT_TRUE(result) << "Export Systems API call failed";
    ASSERT_TRUE(this->response.getResponse().contains("Systems correctly exported."))<<"Error message was expected to contain 'Systems correctly exported.' but was "+this->response.getResponse().toStdString();
    QString actualXml = TestHelpers::readFile(this->testSystemsFilePath);
    ASSERT_EQ(
    TestHelpers::removeSpacesBetweenTags(systemsExp1Xml.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated systems xml file differ.\n"
        << "Expected XML:\n" << systemsExp1Xml.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();  

    //now do the same and check that file is overwritten
    result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);
    ASSERT_TRUE(result) << "Export Systems API call failed";
    ASSERT_TRUE(this->response.getResponse().contains("Systems correctly exported."))<<"Error message was expected to contain 'Systems correctly exported.' but was "+this->response.getResponse().toStdString();
    actualXml = TestHelpers::readFile(this->testSystemsFilePath);
    ASSERT_EQ(
    TestHelpers::removeSpacesBetweenTags(systemsExp1Xml.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated systems xml file differ.\n"
        << "Expected XML:\n" << systemsExp1Xml.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString(); 

    // Checking if backup was created
    QString originalPath = this->testSystemsFilePath;
    QString backupPath = QFileInfo(originalPath).absolutePath() + "/bk/" + QFileInfo(originalPath).fileName() + ".bk1";

    ASSERT_TRUE(QFileInfo(backupPath).exists()) << "Backup level 1 file was not created. Expected: " << backupPath.toStdString();

    //create system again so it is overwritten and backup level 2 is created
    result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);
    ASSERT_TRUE(result) << "Export Systems API call failed";
    ASSERT_TRUE(this->response.getResponse().contains("Systems correctly exported."))<<"Error message was expected to contain 'Systems correctly exported.' but was "+this->response.getResponse().toStdString();
    actualXml = TestHelpers::readFile(this->testSystemsFilePath);
    ASSERT_EQ(
    TestHelpers::removeSpacesBetweenTags(systemsExp1Xml.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated systems xml file differ.\n"
        << "Expected XML:\n" << systemsExp1Xml.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString(); 

    // Checking if backup level 2was created
    backupPath = QFileInfo(originalPath).absolutePath() + "/bk/" + QFileInfo(originalPath).fileName() + ".bk2";

    ASSERT_TRUE(QFileInfo(backupPath).exists()) << "Backup level 2 file was not created. Expected: " << backupPath.toStdString();
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_1_systems_exported_old_system_deleted_POSITIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    ASSERT_TRUE(parseSystemsJson(systemsExp1JSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);
    ASSERT_TRUE(result) << "Export Systems API call failed";
    ASSERT_TRUE(this->response.getResponse().contains("Systems correctly exported."))<<"Error message was expected to contain 'Systems correctly exported.' but was "+this->response.getResponse().toStdString();
    QString actualXml = TestHelpers::readFile(this->testSystemsFilePath);
    ASSERT_EQ(
    TestHelpers::removeSpacesBetweenTags(systemsExp1Xml.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated systems xml file differ.\n"
        << "Expected XML:\n" << systemsExp1Xml.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();  

    //now do the same and check that file is overwritten
    OpenAPI::OAISystemUI &firstSystem = systemsReq.first(); // Reference to the first element
    firstSystem.setDeleted(true);
    result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);
    ASSERT_TRUE(result) << "Export Systems API call failed";
    ASSERT_TRUE(this->response.getResponse().contains("Systems correctly exported."))<<"Error message was expected to contain 'Systems correctly exported.' but was "+this->response.getResponse().toStdString();

    // Checking that file was deleted
    QFileInfo fileSys(this->testSystemsFilePath);
    ASSERT_FALSE(fileSys.exists()) << "The system file was not deleted (modification time not changed).";
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_systems_list_empty_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_FALSE(result) << "Export of systems was succesfull but it was expected to fail";
    ASSERT_TRUE(errorMsg.contains("Received JSON list of systems is empty"))<<"Error message was expected to contain 'Received JSON list of systems is empty' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_system_no_id_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    QString modifiedJSON = TestHelpers::removeJsonElement(systemsExp1JSON,"id");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseSystemsJson(modifiedJSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_FALSE(result) << "Export of systems was succesfull but it was expected to fail";
    ASSERT_TRUE(errorMsg.contains("Failed to export system"))<<"Error message was expected to contain 'Failed to export system' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_system_has_no_components_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    QString modifiedJSON = TestHelpers::emptyJsonArray(systemsExp1JSON,"components");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseSystemsJson(modifiedJSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_FALSE(result) << "Export of systems was succesfull but it was expected to fail";
    ASSERT_TRUE(errorMsg.contains("Failed to export system"))<<"Error message was expected to contain 'Failed to export system' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_component_invalid_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    QString modifiedJSON = TestHelpers::removeJsonElement(systemsExp1JSON,"library");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseSystemsJson(modifiedJSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_FALSE(result) << "Export of systems was succesfull but it was expected to fail";
    ASSERT_TRUE(errorMsg.contains("Failed to export system"))<<"Error message was expected to contain 'Failed to export system' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_component_schedule_invalid_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    QString modifiedJSON = TestHelpers::removeJsonElement(systemsExp1JSON,"offset");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseSystemsJson(modifiedJSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_FALSE(result) << "Export of systems was succesfull but it was expected to fail";
    ASSERT_TRUE(errorMsg.contains("Failed to export system"))<<"Error message was expected to contain 'Failed to export system' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_system_component_parameter_invalid_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    QString modifiedJSON = TestHelpers::removeJsonElement(systemsExp1JSON,"unit");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseSystemsJson(modifiedJSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_FALSE(result) << "Export of systems was succesfull but it was expected to fail";
    ASSERT_TRUE(errorMsg.contains("Failed to export system"))<<"Error message was expected to contain 'Failed to export system' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_system_component_position_invalid_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    QString modifiedJSON = TestHelpers::removeJsonElement(systemsExp1JSON,"x");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseSystemsJson(modifiedJSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_FALSE(result) << "Export of systems was succesfull but it was expected to fail";
    ASSERT_TRUE(errorMsg.contains("Failed to export system"))<<"Error message was expected to contain 'Failed to export system' but was "+errorMsg.toStdString();
}

TEST_F(EXPORT_SYSTEMS_TEST, Export_systems_system_connections_invalid_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systemsReq;
    QString errorMsg;

    QString modifiedJSON = TestHelpers::removeJsonElement(systemsExp1JSON,"output");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseSystemsJson(modifiedJSON, systemsReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = OPGUICore::api_export_systems_config(systemsReq, this->response,errorMsg);

    ASSERT_FALSE(result) << "Export of systems was succesfull but it was expected to fail";
    ASSERT_TRUE(errorMsg.contains("Failed to export system"))<<"Error message was expected to contain 'Failed to export system' but was "+errorMsg.toStdString();
}
