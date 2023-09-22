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

#include "test_OPGUILoadComponentsApi.h"
#include "OPGUICoreGlobalConfig.h"
#include "OPGUICore.h"
#include "OAIHelpers.h"
#include "test_helpers.h"
#include "OAIComponentUI.h"
#include "OAIError500.h"

QString component1Xml = QStringLiteral(
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

QString component1JSON = QStringLiteral(
    R"([{"id": 0,
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
                "value": "1.0"
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
                "type": "int",
                "unit": "Nm",
                "value": "100"
            },
            {
                "id": 3,
                "title": "Kp of the pedals PID control",
                "type": "bool",
                "unit": "",
                "value": "true"
            },
            {
                "id": 4,
                "title": "Ki of the pedals PID control",
                "type": "string",
                "unit": "",
                "value": "string param"
            }
        ],
        "position": {
            "x": 0,
            "y": 0
        },
        "schedule": {
            "cycle": 10,
            "offset": 0,
            "response": 0,
            "priority": 0
        },
        "title": "Algorithm_Test",
        "type": "AlgorithmTest"
    }])"
);


void LOAD_COMPONENTS_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
    //ceck path to core exists
    QDir dirCore(OPGUICoreGlobalConfig::getInstance().pathOpenpassCore());
    ASSERT_TRUE(dirCore.exists())<<"Core directory not found under:"+OPGUICoreGlobalConfig::getInstance().pathOpenpassCore().toStdString();
    
    testDir = QStringLiteral("test_path_components");
    this->testDirFullPath = TestHelpers::joinPaths(OPGUICoreGlobalConfig::getInstance().pathOpenpassCore(),testDir);

    // If test directory exists, remove it.
    QDir dirTest(this->testDirFullPath);
    if(dirTest.exists()){
        ASSERT_TRUE(dirTest.removeRecursively()) << "Failed to remove existing test directory: " << this->testDirFullPath.toStdString();
    }

    OPGUICoreGlobalConfig::getInstance().setComponentsFolder(testDir);

    ASSERT_TRUE(dirTest.mkpath(this->testDirFullPath)) << "Failed to create test directory at: " << this->testDirFullPath.toStdString();
}

void LOAD_COMPONENTS_TEST::TearDown()  {
    QDir dirTest(this->testDirFullPath);
    if(dirTest.exists()){
        ASSERT_TRUE(dirTest.removeRecursively()) << "Failed to remove test directory: " << this->testDirFullPath.toStdString();
    }
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_no_components_dir_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    OPGUICoreGlobalConfig::getInstance().setComponentsFolder("/tmp/not_existing_dir");

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_EQ(errorMsg,"Directory does not exist.")<<"Error message was expected to be 'Directory does not exist.' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_no_components_inside_dir_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_EQ(errorMsg,"No components files found in directory.")<<"Error message was expected to be 'No components files found in directory.' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_empty_component_file_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath, ""))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Component XML file is empty"))<<"Error message was expected to contain 'Component XML file is empty' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_empty_bad_xml_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath, "CONTENT NOT VALID XML"))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Invalid XML content in XML component file"))<<"Error message was expected to contain 'Invalid XML content in XML component file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_no_title_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"title");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_no_type_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"type");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_no_parameters_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"parameters");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_no_parameter_title_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"title","parameter");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_no_parameter_id_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"id","parameter");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_no_inputs_field_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"inputs");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_no_outputs_field_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"inputs");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_invalid_input_field_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"id","input");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_invalid_output_field_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"id","output");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_invalid_schedule_field_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    QString modifiedXml = TestHelpers::removeXmlElement(component1Xml,"offset","schedule");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";

    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_component_has_several_invalid_sub_fields_NEGATIVE) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    //parameter id not int
    QString modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"id","bad int","parameter");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    bool result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //parameter type int but value not convertible
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"type","int","parameter");
    modifiedXml = TestHelpers::replaceXmlElementContent(modifiedXml,"value","bad int","parameter");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //parameter type bool but value not convertible
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"type","bool","parameter");
    modifiedXml = TestHelpers::replaceXmlElementContent(modifiedXml,"value","bad bool","parameter");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //parameter type float but value not convertible
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"type","float","parameter");
    modifiedXml = TestHelpers::replaceXmlElementContent(modifiedXml,"value","bad float","parameter");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //parameter type unnknown
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"type","unknown","parameter");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //input id not int
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"id","bad int","input");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //input cardinality not int
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"cardinality","bad int","input");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //output id not int
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"id","bad int","output");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //schedule offset not int
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"offset","bad int","schedule");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //schedule cycle not int
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"cycle","bad int","schedule");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();

    //schedule response not int
    modifiedXml = TestHelpers::replaceXmlElementContent(component1Xml,"response","bad int","schedule");
    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath,modifiedXml))<<"Could not save xml file as: "+filePath.toStdString();
    result = OPGUICore::api_get_components(components, errorMsg);
    ASSERT_FALSE(result) << "Load components was succesfull but should be failed";
    ASSERT_TRUE(errorMsg.contains("Failed to parse XML component from file"))<<"Error message was expected to contain 'Failed to parse XML component from file' but was "+errorMsg.toStdString();
}

TEST_F(LOAD_COMPONENTS_TEST, Load_components_1_component_loaded_positive) {
    QList<OpenAPI::OAIComponentUI> components;
    QString errorMsg;

    QString filePath = TestHelpers::joinPaths(this->testDirFullPath, "test_component.xml");

    ASSERT_TRUE(TestHelpers::createAndCheckFile(filePath, component1Xml))<<"Could not save xml file as: "+filePath.toStdString();

    bool result = OPGUICore::api_get_components(components, errorMsg);

    ASSERT_TRUE(result) << "Load components API call failed";

    QJsonDocument resDoc(::OpenAPI::toJsonValue(components).toArray());

    QByteArray jsonBytes = resDoc.toJson(QJsonDocument::Indented);
    QString actualJsonString = QString(jsonBytes);

    QJsonDocument expectedDoc = QJsonDocument::fromJson(component1JSON.toUtf8());
    QJsonDocument actualDoc = QJsonDocument::fromJson(actualJsonString.toUtf8());

    QString expectedJsonStr = QString(expectedDoc.toJson(QJsonDocument::Compact));
    QString actualJsonStr = QString(actualDoc.toJson(QJsonDocument::Compact));

    ASSERT_EQ(expectedJsonStr, actualJsonStr) 
        << "JSON objects are not equal. Expected: " << expectedJsonStr.toStdString() 
        << ", Actual: " << actualJsonStr.toStdString();
}






