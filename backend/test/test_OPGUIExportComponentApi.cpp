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

#include "test_OPGUIExportComponentApi.h"
#include "OPGUICoreGlobalConfig.h"
#include "OAIHelpers.h"
#include "test_helpers.h"
#include "OAIComponentUI.h"

QString componentExp1Xml = QStringLiteral(
    R"(<component>
      <type>Algorithm</type>
      <title>Algorithm_Test</title>
      <schedule>
        <offset>0</offset>
        <cycle>10</cycle>
        <response>0</response>
      </schedule>
      <parameters>
        <parameter>
			<title>Driver aggressiveness</title>
			<id>0</id>
			<type>double</type>
			<unit></unit>
			<value>1</value>
		</parameter>
		<parameter>
			<title>Max. engine power</title>
			<id>1</id>
			<type>double</type>
			<unit>W</unit>
			<value>100000</value>
		</parameter>
		<parameter>
			<title>Min. brake torque</title>
			<id>2</id>
			<type>double</type>
			<unit>Nm</unit>
			<value>-10000</value>
		</parameter>
		<parameter>
			<title>Kp of the pedals PID control</title>
			<id>3</id>
			<type>double</type>
			<unit></unit>
			<value>-0.5</value>
		</parameter>
		<parameter>
			<title>Ki of the pedals PID control</title>
			<id>4</id>
			<type>double</type>
			<unit></unit>
			<value>-0.4</value>
		</parameter>
		<parameter>
			<title>Kd of the pedals PID control</title>
			<id>5</id>
			<type>double</type>
			<unit></unit>
			<value>0.0</value>
		</parameter>
		<parameter>
			<title>Kp of the steering PID control</title>
			<id>6</id>
			<type>double</type>
			<unit></unit>
			<value>-18</value>
		</parameter>
		<parameter>
			<title>Ki of the steering PID control</title>
			<id>7</id>
			<type>double</type>
			<unit></unit>
			<value>-0.6</value>
		</parameter>
		<parameter>
			<title>Kd of the steering PID control</title>
			<id>8</id>
			<type>double</type>
			<unit></unit>
			<value>0.0</value>
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

QString componentExp1JSON = QStringLiteral(
    R"({
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
        "schedule": {
            "cycle": 10,
            "offset": 0,
            "response": 0
        },
        "title": "Algorithm_Test",
        "type": "Algorithm"
       })"
);

bool EXPORT_COMPONENT_TEST::parseComponentJson(const QString& jsonStr, OpenAPI::OAIComponentUI& oai_component) {
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

    OpenAPI::fromJsonValue(oai_component, jsonObj);

    return true;
}

void EXPORT_COMPONENT_TEST::SetUp() {}

void EXPORT_COMPONENT_TEST::TearDown() {}

TEST_F(EXPORT_COMPONENT_TEST, Export_component_positive) {
    OpenAPI::OAIComponentUI componentReq;
    QString errorMsg;

    QByteArray byteArray;
    QXmlStreamWriter xmlWriter(&byteArray);

    ASSERT_TRUE(parseComponentJson(componentExp1JSON, componentReq))<< "Failed preparing the test, unable parse the systems JSON for the test";

    bool result = this->systemEditor.exportComponentToXml(xmlWriter, componentReq, false);

    ASSERT_TRUE(result) << "Export component failed";

    QString generatedXML = QString::fromUtf8(byteArray);

    ASSERT_EQ(
    TestHelpers::removeSpacesBetweenTags(componentExp1Xml.simplified()),
    TestHelpers::removeSpacesBetweenTags(generatedXML.simplified())
    ) << "Test: contents of expected and generated systems xml file differ.\n"
        << "Expected XML:\n" << TestHelpers::removeSpacesBetweenTags(componentExp1Xml.simplified()).toStdString() << "\n"
        << "Actual XML:\n" << TestHelpers::removeSpacesBetweenTags(generatedXML.simplified()).toStdString();
        
}

TEST_F(EXPORT_COMPONENT_TEST, Export_component_no_title_negative) {
    OpenAPI::OAIComponentUI componentReq;
    QString errorMsg;

    QByteArray byteArray;
    QXmlStreamWriter xmlWriter(&byteArray);

    QString modifiedJSON = TestHelpers::removeJsonElement(componentExp1JSON,"title");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseComponentJson(modifiedJSON, componentReq))<< "Failed preparing the test, unable parse the components JSON for the test";

    bool result = this->systemEditor.exportComponentToXml(xmlWriter, componentReq, false);

    ASSERT_FALSE(result) << "Export component succeed but fail was expected";  
}


TEST_F(EXPORT_COMPONENT_TEST, Export_component_wrong_schedule_negative) {
    OpenAPI::OAIComponentUI componentReq;
    QString errorMsg;

    QByteArray byteArray;
    QXmlStreamWriter xmlWriter(&byteArray);

    QString modifiedJSON = TestHelpers::removeJsonElement(componentExp1JSON,"offset");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseComponentJson(modifiedJSON, componentReq))<< "Failed preparing the test, unable parse the components JSON for the test";

    bool result = this->systemEditor.exportComponentToXml(xmlWriter, componentReq, false);

    ASSERT_FALSE(result) << "Export component succeed but fail was expected";  
}

TEST_F(EXPORT_COMPONENT_TEST, Export_component_wrong_parameter_negative) {
    OpenAPI::OAIComponentUI componentReq;
    QString errorMsg;

    QByteArray byteArray;
    QXmlStreamWriter xmlWriter(&byteArray);

    QString modifiedJSON = TestHelpers::removeJsonElement(componentExp1JSON,"unit");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseComponentJson(modifiedJSON, componentReq))<< "Failed preparing the test, unable parse the components JSON for the test";

    bool result = this->systemEditor.exportComponentToXml(xmlWriter, componentReq, false);

    ASSERT_FALSE(result) << "Export component succeed but fail was expected";  
}

TEST_F(EXPORT_COMPONENT_TEST, Export_component_wrong_input_negative) {
    OpenAPI::OAIComponentUI componentReq;
    QString errorMsg;

    QByteArray byteArray;
    QXmlStreamWriter xmlWriter(&byteArray);

    QString modifiedJSON = TestHelpers::removeJsonElement(componentExp1JSON,"cardinality");
    ASSERT_FALSE(modifiedJSON.isEmpty())<<"JSON element removal delivered empty JSON string";

    ASSERT_TRUE(parseComponentJson(modifiedJSON, componentReq))<< "Failed preparing the test, unable parse the components JSON for the test";

    bool result = this->systemEditor.exportComponentToXml(xmlWriter, componentReq, false);

    ASSERT_FALSE(result) << "Export component succeed but fail was expected";  
}

TEST_F(EXPORT_COMPONENT_TEST, Export_component_wrong_output_negative) {
    OpenAPI::OAIOutput output;
    QString errorMsg;

    QByteArray byteArray;
    QXmlStreamWriter xmlWriter(&byteArray);

    output.setTitle("test");

    bool result = this->systemEditor.exportOutputToXml(xmlWriter, output);

    ASSERT_FALSE(result) << "Export component succeed but fail was expected";  
}