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

#include "test_OPGUIPathToConvCases.h"
#include "test_helpers.h"
#include "OPGUICore.h"
#include "OPGUICoreGlobalConfig.h"

//TODO: ADD TEST FOR SIM OBJECT STARTED

QString PATH_TO_CONVERTED_CASES_TEST::getConvertedCasesPath(const QString &configFilePath)
{
    QFile file(configFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return "";  // Return an empty string if the file can't be opened
    }

    QByteArray data = file.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(data));
    QJsonObject jsonObj = jsonDoc.object();

    // If PATH_CONVERTED_CASES exists, retrieve its value, otherwise return an empty string
    return jsonObj.value("PATH_CONVERTED_CASES").toString("");
}

bool PATH_TO_CONVERTED_CASES_TEST::removeConvertedCasesPath(const QString &configFilePath)
{
    QFile file(configFilePath);
    if (!file.open(QIODevice::ReadWrite))
    {
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(data));
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("PATH_CONVERTED_CASES"))
    {
        jsonObj.remove("PATH_CONVERTED_CASES");
        jsonDoc.setObject(jsonObj);

        file.close();  // Close the file to later overwrite it

        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))  // Open it for writing only and truncate its content
        {
            qDebug() << "Couldn't open configuration file for writing.";
            return false;
        }

        file.write(jsonDoc.toJson());
        return true;
    }
    return false;
}


void PATH_TO_CONVERTED_CASES_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
    this->pathToConfigFile = OPGUICoreGlobalConfig::getInstance().pathConfigFile();
    response200Expected.setResponse("PATH_CONVERTED_CASES has been correctly set.");
    //check workspace path exist
    testDir = QStringLiteral("test_path_converted_cases");
    this->testDirFullPath = TestHelpers::joinPaths(OPGUICoreGlobalConfig::getInstance().workspace(),testDir);
    this->pathToConvertedCasesOriginal = getConvertedCasesPath(this->pathToConfigFile);

    removeConvertedCasesPath(this->pathToConfigFile);

    OPGUICoreGlobalConfig::getInstance().setPathConvertedCases(QStringLiteral(""));
}

void PATH_TO_CONVERTED_CASES_TEST::TearDown()  {
    removeConvertedCasesPath(this->pathToConfigFile);
    if(!this->pathToConvertedCasesOriginal.isEmpty()){
        OPGUICoreGlobalConfig::getInstance().modifyOrAddValueToConfigFile("PATH_CONVERTED_CASES",this->pathToConvertedCasesOriginal);
        OPGUICoreGlobalConfig::getInstance().setPathConvertedCases(this->pathToConvertedCasesOriginal);
    }
    else{
        OPGUICoreGlobalConfig::getInstance().setPathConvertedCases(OPGUICoreGlobalConfig::getInstance().workspace());
    }
}

TEST_F(PATH_TO_CONVERTED_CASES_TEST, Path_to_converted_cases_variable_does_not_exist_in_config_JSON_POSITIVE) {
    QString errorMsg;
    this->request.setPath(this->testDirFullPath);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    //call path converted cases
    bool result = OPGUICore::api_path_to_converted_cases(validatedRequest, this->response,errorMsg);
    
    ASSERT_TRUE(result);
    EXPECT_EQ(this->response.getResponse(),this->response200Expected.getResponse()) << "Test: Path to converted cases did not return expected response";

    EXPECT_EQ(OPGUICoreGlobalConfig::getInstance().pathConvertedCases(),this->testDirFullPath) << "Test: New Path to converted cases was not setted in config object, current value is:"+OPGUICoreGlobalConfig::getInstance().pathConvertedCases().toStdString();
    EXPECT_EQ(getConvertedCasesPath(this->pathToConfigFile),this->testDirFullPath) << "Test: New Path to converted cases was not setted in config file, current value is:"+getConvertedCasesPath(this->pathToConfigFile).toStdString();
}

TEST_F(PATH_TO_CONVERTED_CASES_TEST, Path_to_converted_cases_empty_path_NEGATIVE) {
    QString errorMsg;
    this->request.setPath(QStringLiteral(""));

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    //call path converted cases
    bool result = OPGUICore::api_path_to_converted_cases(validatedRequest, this->response,errorMsg);
    
    ASSERT_FALSE(result);
    ASSERT_TRUE(errorMsg.contains("Error in setting path of converted cases. Path is empty / invalid."))<<"Error message was expected to contain 'Error in setting path of converted cases. Path is empty / invalid.' but was "+errorMsg.toStdString();
}

TEST_F(PATH_TO_CONVERTED_CASES_TEST, Path_to_converted_cases_no_config_file_NEGATIVE) {
    QString errorMsg;
    this->request.setPath(this->testDirFullPath);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    //Rename backend config file temporary
    QFile file(this->pathToConfigFile);
    if (!file.exists()) {
        FAIL() << "Test: Original config file does not exist:" + this->pathToConfigFile.toStdString();
    }
    QString backupFilePath = this->pathToConfigFile + "_bk";
    if (!file.rename(backupFilePath)) {
        FAIL() << "Test: Failed to rename the config file to its backup:"+ backupFilePath.toStdString();
    }

     //call path converted cases
    bool result = OPGUICore::api_path_to_converted_cases(validatedRequest, this->response,errorMsg);

    // Rename the backup file back to the original file name
    QFile backupFile(backupFilePath);
    if (!backupFile.rename(this->pathToConfigFile)) {
        FAIL() << "Test: Failed to rename the backup file back to the original name:" + this->pathToConfigFile.toStdString();
    }

    ASSERT_FALSE(result) ;
    ASSERT_TRUE(errorMsg.contains("Failed to set PATH_CONVERTED_CASES in configuration file as"))<<"Error message was expected to contain 'Failed to set PATH_CONVERTED_CASES in configuration file as' but was "+errorMsg.toStdString();
}


