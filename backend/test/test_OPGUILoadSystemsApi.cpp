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

#include "test_OPGUILoadSystemsApi.h"
#include "OPGUICoreGlobalConfig.h"
#include "OPGUICore.h"
#include "OAIHelpers.h"
#include "test_helpers.h"
#include "OAIComponentUI.h"
#include "OAIError500.h"
#include "OAISystemUI.h"

QString componentSys1Xml = QStringLiteral(
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

QString componentSys2Xml = QStringLiteral(
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

QString systems1Xml = QStringLiteral(
    R"(<?xml version='1.0' encoding='UTF-8'?>
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
            <priority>30</priority>
            <offset>0</offset>
            <cycle>10</cycle>
            <response>0</response>
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
            <priority>1</priority>
            <offset>0</offset>
            <cycle>10</cycle>
            <response>0</response>
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
            <!--component title: Action_Test-->
            <component>1</component>
            <!--output title: VerticalForce-->
            <output>0</output>
            </source>
            <target>
            <!--component title: Algorithm_Test-->
            <component>0</component>
            <!--input title: Desired trajectory-->
            <input>0</input>
            </target>
        </connection>
        <connection>
            <id>1</id>
            <source>
            <!--component title: Algorithm_Test-->
            <component>0</component>
            <!--output title: Control-->
            <output>0</output>
            </source>
            <target>
            <!--component title: Action_Test-->
            <component>1</component>
            <!--input title: InertiaForce-->
            <input>0</input>
            </target>
        </connection>
        </connections>
        </system>
      </systems>)"
);

QString systems1JSON = QStringLiteral(
    R"([{
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
                "type": "AlgorithmTest"
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
        "file":"test_system.xml"
    }])"
);


void LOAD_SYSTEMS_TEST::SetUp()  {
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
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath, componentSys1Xml))<<"Could not save xml file as: "+filePath.toStdString();
    filePath = TestHelpers::joinPaths(this->testDirComponentsPath, "test_component_2.xml");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath, componentSys2Xml))<<"Could not save xml file as: "+filePath.toStdString();
}

void LOAD_SYSTEMS_TEST::TearDown()  {
    QDir dirCompTest(this->testDirComponentsPath);
    if(dirCompTest.exists()){
        ASSERT_TRUE(dirCompTest.removeRecursively()) << "Failed to remove components test directory: " << this->testDirComponentsPath.toStdString();
    }
    QDir dirSysTest(this->testDirSystemsPath);
    if(dirSysTest.exists()){
        ASSERT_TRUE(dirSysTest.removeRecursively()) << "Failed to remove systems test directory: " << this->testDirSystemsPath.toStdString();
    }
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_no_systems_dir_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    OPGUICoreGlobalConfig::getInstance().setSystemsFolder("not_existing_dir");

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load systems was succesfull but should be failed";

    ASSERT_EQ(errorMsg,"Directory does not exist.")<<"Error message was expected to be 'Directory does not exist.' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_no_systems_inside_dir_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load systems was succesfull but should be failed";

    ASSERT_EQ(errorMsg,"No system files found in directory.")<<"Error message was expected to be 'No system files found in directory.' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_empty_system_file_NEGATIVE) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath, ""))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load systems was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("System XML file is empty"))<<"Error message was expected to contain 'System XML file is empty' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_1_systems_loaded_POSITIVE) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath, systems1Xml))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_TRUE(result) << "Load Systems API call failed";

    QJsonDocument resDoc(::OpenAPI::toJsonValue(systems).toArray());

    QByteArray jsonBytes = resDoc.toJson(QJsonDocument::Indented);
    QString actualJsonString = QString(jsonBytes);

    QJsonDocument expectedDoc = QJsonDocument::fromJson(systems1JSON.toUtf8());
    QJsonDocument actualDoc = QJsonDocument::fromJson(actualJsonString.toUtf8());

    QString expectedJsonStr = QString(expectedDoc.toJson(QJsonDocument::Compact));
    QString actualJsonStr = QString(actualDoc.toJson(QJsonDocument::Compact));

    ASSERT_EQ(expectedJsonStr, actualJsonStr) 
        << "JSON objects are not equal. Expected: " << expectedJsonStr.toStdString() 
        << ", Actual: " << actualJsonStr.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_systems_file_is_empty_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath, ""))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("System XML file is empty"))<<"Error message was expected to contain 'System XML file is empty' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_systems_file_is_unparseable_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath, "some wrong xml>fewfe>wefwef"))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Invalid XML content in XML system file"))<<"Error message was expected to contain 'Invalid XML content in XML system file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_fail_original_components_load_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;
    
    QDir dirCompTest(this->testDirComponentsPath);
    ASSERT_TRUE(dirCompTest.removeRecursively()) << "Failed to remove components directory: " << this->testDirComponentsPath.toStdString();

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,systems1Xml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_has_no_title_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;
    
    QString modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"title");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_has_no_components_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;
    
    QString modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"component","system");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_component_missing_library_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;
    
    QString modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"library","components");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_component_missing_parameter_component_in_server_negative) {
    QFile::remove(TestHelpers::joinPaths(this->testDirComponentsPath,"test_component_1.xml"));

    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;
    
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,systems1Xml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_component_missing_priority_in_schedule_component_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    QString modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"priority","schedule");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_component_missing_position_x_in_component_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    QString modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"x","position");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_component_wrong_position_x_in_component_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    QString modifiedXml = TestHelpers::replaceXmlElementContent(systems1Xml,"x","not int","position");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_component_wrong_position_y_in_component_negative) {
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    QString modifiedXml = TestHelpers::replaceXmlElementContent(systems1Xml,"y","not int","position");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();

    bool result = OPGUICore::api_get_systems(systems, errorMsg);

    ASSERT_FALSE(result) << "Load Systems API call successed but should have failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_SYSTEMS_TEST, Load_systems_system_various_problems_with_connections_negative){
    QList<OpenAPI::OAISystemUI> systems;
    QString errorMsg;

    //connection id not present
    //QString modifiedXml = TestHelpers::replaceXmlElementContent(systems1Xml,"id","bad int","parameter");
    QString modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"id","connection");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();
    bool result = OPGUICore::api_get_systems(systems, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();

    //connection id not int
    modifiedXml = TestHelpers::replaceXmlElementContent(systems1Xml,"id","bad int","connection");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();
    result = OPGUICore::api_get_systems(systems, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();

    //connection source not present
    modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"source","connection");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();
    result = OPGUICore::api_get_systems(systems, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();

    //connection target not present
    modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"target","connection");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();
    result = OPGUICore::api_get_systems(systems, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();

    //source component not valid
    modifiedXml = TestHelpers::replaceXmlElementContent(systems1Xml,"component","bad int","source");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();
    result = OPGUICore::api_get_systems(systems, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();

    //target component not present
    modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"component","target");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();
    result = OPGUICore::api_get_systems(systems, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();

    //target input not valid
    modifiedXml = TestHelpers::replaceXmlElementContent(systems1Xml,"input","bad int","target");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();
    result = OPGUICore::api_get_systems(systems, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();

    //source output not present
    modifiedXml = TestHelpers::removeXmlElement(systems1Xml,"output","source");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(this->testSystemsFilePath,modifiedXml ))<<"Could not save xml file as: "+this->testSystemsFilePath.toStdString();
    result = OPGUICore::api_get_systems(systems, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML system from file"))<<"Error message was expected to contain 'Failed to parse XML system from file' but was "+errorMsg.toStdString();
}
