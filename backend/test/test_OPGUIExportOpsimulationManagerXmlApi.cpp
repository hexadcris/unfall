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
#include <QString>
#include <QList>
#include <QFile>

#include "test_OPGUIExportOpsimulationManagerXmlApi.h"
#include "test_helpers.h"
#include "OPGUICore.h"
#include "OPGUICoreGlobalConfig.h"
#include "OAISimulationConfig.h"

QString expectedXmlExample1 = QStringLiteral(
        R"(<?xml version="1.0" encoding="UTF-8"?>
        <opSimulationManager>
            <logLevel>2</logLevel>
            <simulation>opSimulation</simulation>
            <logFileSimulationManager>$$CORE_DIR$$opSimulationManager.log</logFileSimulationManager>
            <libraries>modules</libraries>
            <simulationConfigs>
                <simulationConfig>
                    <configurations>$$CORE_DIR$$configurations</configurations>
                    <logFileSimulation>$$CORE_DIR$$logFileSimulation.log</logFileSimulation>
                    <results>$$CORE_DIR$$results</results>
                </simulationConfig>
            </simulationConfigs>
        </opSimulationManager>)"
);

OpenAPI::OAIOpSimulationManagerXmlRequest getRequestExample1(QString &coreDir) {

        OpenAPI::OAIOpSimulationManagerXmlRequest request;
        request.setLogLevel(2);
        request.setLogFileSimulationManager("opSimulationManager.log");
        request.setSimulation("opSimulation");
        request.setLibraries("modules");

        OpenAPI::OAISimulationConfig simConfig;
        simConfig.setNumberOfExecutions(5);
        simConfig.setLogFileSimulation(coreDir+"/"+"logFileSimulation.log");
        simConfig.setConfigurations(coreDir+"/"+"configurations");
        simConfig.setResults(coreDir+"/"+"results");

        QList<OpenAPI::OAISimulationConfig> configs;
        configs.append(simConfig);
        request.setSimulationConfigs(configs);

        return request;
}

// Example data
QString expectedXmlExample2 = QString::fromStdString(
        R"(<?xml version="1.0" encoding="UTF-8"?>
        <opSimulationManager>
        <logLevel>2</logLevel>
        <simulation>opSimulation</simulation>
        <logFileSimulationManager>$$CORE_DIR$$opSimulationManager.log</logFileSimulationManager>
        <libraries>modules</libraries>
            <simulationConfigs>
                <simulationConfig>
                    <configurations>$$CORE_DIR$$configs1</configurations>
                    <logFileSimulation>$$CORE_DIR$$opSimulation1.log</logFileSimulation>
                    <results>$$CORE_DIR$$results1</results>
                </simulationConfig>
                <simulationConfig>
                    <configurations>$$CORE_DIR$$configs2</configurations>
                    <logFileSimulation>$$CORE_DIR$$opSimulation2.log</logFileSimulation>
                    <results>$$CORE_DIR$$results2</results>
                </simulationConfig>
            </simulationConfigs>
        </opSimulationManager>)"
);

OpenAPI::OAIOpSimulationManagerXmlRequest getRequestExample2(QString &coreDir) {
        OpenAPI::OAIOpSimulationManagerXmlRequest request;
        request.setLogLevel(2);
        request.setLogFileSimulationManager("opSimulationManager.log");
        request.setSimulation("opSimulation");
        request.setLibraries("modules");

        OpenAPI::OAISimulationConfig simConfig1;
        simConfig1.setNumberOfExecutions(1);
        simConfig1.setLogFileSimulation(coreDir+"/"+"opSimulation1.log");
        simConfig1.setConfigurations(coreDir+"/"+"configs1");
        simConfig1.setResults(coreDir+"/"+"results1");

        OpenAPI::OAISimulationConfig simConfig2;
        simConfig2.setNumberOfExecutions(2);
        simConfig2.setLogFileSimulation(coreDir+"/"+"opSimulation2.log");
        simConfig2.setConfigurations(coreDir+"/"+"configs2");
        simConfig2.setResults(coreDir+"/"+"results2");

        QList<OpenAPI::OAISimulationConfig> configs;
        configs.append(simConfig1);
        configs.append(simConfig2);
        request.setSimulationConfigs(configs);

        return request;
}
   
void XML_EXPORT_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
    response200Expected.setResponse("opsimulation manager data exported correctly!");
    // Prepare the file paths
    this->openPassCoreDir = OPGUICoreGlobalConfig::getInstance().pathOpenpassCore();

    this->configFileFullPath =  OPGUICoreGlobalConfig::getInstance().fullPathOpSimulationManagerXml();
    this->backupFilePath = this->configFileFullPath + QStringLiteral("_bk");

    // Check core path exist
    QDir dir(this->openPassCoreDir);
    ASSERT_TRUE(dir.exists()) 
        << "The directory does not exist: " << this->openPassCoreDir.toStdString();

    // Just in case teardown not executed...
    QFile backupFile(this->backupFilePath);
    if (backupFile.exists()) {
        bool fileRenamed = backupFile.rename(this->configFileFullPath);
        EXPECT_TRUE(fileRenamed) 
            << "SetUp: Failed to rename the backuped config file from " 
            << this->backupFilePath.toStdString() 
            << " to " << this->configFileFullPath.toStdString();
    }

    // Check if the file already exists and if yes, rename it
    QFile file(this->configFileFullPath);
    bool fileExists = file.exists();
    if (fileExists) {
        bool fileRenamed = file.rename(backupFilePath);
        EXPECT_TRUE(fileRenamed) 
            << "SetUp: Failed to rename config file from " 
            << this->configFileFullPath.toStdString() 
            << " to " << this->backupFilePath.toStdString();
    }
}

void XML_EXPORT_TEST::TearDown()  {
    bool removed = QFile::remove(configFileFullPath);
    EXPECT_TRUE(removed) 
        << "TearDown: Failed to remove the newly created file: " 
        << configFileFullPath.toStdString();

    // Rename the backup file back to its original name
    QFile backupFile(this->backupFilePath);
    if (backupFile.exists()) {
        bool fileRenamed = backupFile.rename(this->configFileFullPath);
        EXPECT_TRUE(fileRenamed) 
            << "SetUp: Failed to rename the backuped config file: " 
            << this->backupFilePath.toStdString() 
            << " to " << this->configFileFullPath.toStdString();
}

}


TEST_F(XML_EXPORT_TEST, TestExportToXML_OneValueInArray_Positive) {
    QString errorMsg;
    this->request = getRequestExample1(this->openPassCoreDir);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIOpSimulationManagerXmlRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    bool result = OPGUICore::api_export_opSimulationManager_xml(validatedRequest,this->response,errorMsg);  
    this->response.setResponse(this->response200Expected.getResponse());

    // Check that the XML file has been created
    ASSERT_TRUE(result) << "Test: exportToXML function returned failed";
    ASSERT_TRUE(this->response.getResponse() == this->response200Expected.getResponse())
        << "Test: exportToXML function did not return expected value. Got: "
        << this->response.getResponse().toStdString()
        << ", Expected: " << this->response200Expected.getResponse().toStdString();
    ASSERT_TRUE(QFile::exists(this->configFileFullPath))
        << "Test: exportToXML did not create any file at path: " << this->configFileFullPath.toStdString();

    // Read the content of the generated file
    QString actualXml = TestHelpers::readFile(this->configFileFullPath);

    expectedXmlExample1.replace("$$CORE_DIR$$", this->openPassCoreDir + QStringLiteral("/"));

    EXPECT_EQ(
    TestHelpers::removeSpacesBetweenTags(expectedXmlExample1.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated xml file differ.\n"
        << "Expected XML:\n" << expectedXmlExample1.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();
}

TEST_F(XML_EXPORT_TEST, TestExportToXML_TwoValuesInArray_Positive) {
    QString errorMsg;
    this->request = getRequestExample2(this->openPassCoreDir);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIOpSimulationManagerXmlRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    bool result = OPGUICore::api_export_opSimulationManager_xml(validatedRequest,this->response,errorMsg); 
    
    // Check that the XML file has been created
    ASSERT_TRUE(result) << "Test: exportToXML function returned failed";
    ASSERT_TRUE(this->response.getResponse() == this->response200Expected.getResponse())
        << "Test: exportToXML function did not return expected value. Got: "
        << this->response.getResponse().toStdString()
        << ", Expected: " << this->response200Expected.getResponse().toStdString();
    ASSERT_TRUE(QFile::exists(this->configFileFullPath))
        << "Test: exportToXML did not create any file at path: " << this->configFileFullPath.toStdString();
    // Read the content of the generated file
    QString actualXml = TestHelpers::readFile(this->configFileFullPath);

    expectedXmlExample2.replace("$$CORE_DIR$$", this->openPassCoreDir + QStringLiteral("/"));

    EXPECT_EQ(
    TestHelpers::removeSpacesBetweenTags(expectedXmlExample2.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated xml file differ.\n"
        << "Expected XML:\n" << expectedXmlExample2.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();
}

TEST_F(XML_EXPORT_TEST, TestExportToXML_DifferentObjectToCompare_Negative) {
    QString errorMsg;
    this->request = getRequestExample1(this->openPassCoreDir);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIOpSimulationManagerXmlRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    // Call XML export function
    bool result = OPGUICore::api_export_opSimulationManager_xml(validatedRequest,this->response,errorMsg);  
    
    // Check that the XML file has been created
    ASSERT_TRUE(result) << "Test: exportToXML function returned failed";
    ASSERT_TRUE(this->response.getResponse() == this->response200Expected.getResponse())
        << "Test: exportToXML function did not return expected value. Got: "
        << this->response.getResponse().toStdString()
        << ", Expected: " << this->response200Expected.getResponse().toStdString();
    ASSERT_TRUE(QFile::exists(this->configFileFullPath))
    << "Test: exportToXML did not create any file at path: " << this->configFileFullPath.toStdString();

    // Read the content of the generated file
    QString actualXml = TestHelpers::readFile(this->configFileFullPath);

    EXPECT_NE(
    TestHelpers::removeSpacesBetweenTags(expectedXmlExample2.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated xml file differ.\n"
        << "Expected XML:\n" << expectedXmlExample2.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();
}

TEST_F(XML_EXPORT_TEST, TestExportToXML_DifferentObjectToCompare_Negative_2) {
    QString errorMsg;
    // Retrieve the test parameters
    this->request = getRequestExample2(this->openPassCoreDir);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIOpSimulationManagerXmlRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    // Call XML export function
    bool result = OPGUICore::api_export_opSimulationManager_xml(validatedRequest,this->response,errorMsg);
    
    // Check that the XML file has been created
    ASSERT_TRUE(result) << "Test: exportToXML function returned failed";
    ASSERT_TRUE(this->response.getResponse() == this->response200Expected.getResponse())
        << "Test: exportToXML function did not return expected value. Got: "
        << this->response.getResponse().toStdString()
        << ", Expected: " << this->response200Expected.getResponse().toStdString();
    ASSERT_TRUE(QFile::exists(this->configFileFullPath))
    << "Test: exportToXML did not create any file at path: " << this->configFileFullPath.toStdString();

    // Read the content of the generated file
    QString actualXml = TestHelpers::readFile(this->configFileFullPath);

    EXPECT_NE(
    TestHelpers::removeSpacesBetweenTags(expectedXmlExample1.simplified()),
    TestHelpers::removeSpacesBetweenTags(actualXml.simplified())
    ) << "Test: contents of expected and generated xml file differ.\n"
        << "Expected XML:\n" << expectedXmlExample1.simplified().toStdString() << "\n"
        << "Actual XML:\n" << actualXml.simplified().toStdString();
}



