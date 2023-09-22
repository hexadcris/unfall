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

#include "OPGUICoreGlobalConfig.h"
#include "test_OPGUISendPCMFile.h"
#include "OAIDefault200Response.h"
#include "test_helpers.h"
#include "OPGUICoreGlobalConfig.h"
#include "OPGUICore.h"

void SEND_PCM_FILE_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
    QString testFileName = QStringLiteral("test_scene.db");
    this->response200Expected.setSelectedExperiments(QList<QString>() << "1");
    this->destinationFilePathTestDB = TestHelpers::joinPaths(OPGUICoreGlobalConfig::getInstance().pathOpenpassCore(), testFileName);

    // Check if the source file exists
    ASSERT_TRUE(QFile::exists(testFileName)) 
        << "Test: Source .db file doesn't exist at build directory: " << testFileName.toStdString();

    // If test db file exists in core dir remove it
    if (QFile::exists(this->destinationFilePathTestDB)) {
        ASSERT_TRUE(QFile::remove(this->destinationFilePathTestDB)) 
            << "Test: Failed to remove pre-existing database file at: " << this->destinationFilePathTestDB.toStdString();
    }
    if (QFile::exists(this->destinationFilePathTestDB + QStringLiteral("_bk"))) {
        ASSERT_TRUE(QFile::remove(this->destinationFilePathTestDB + QStringLiteral("_bk"))) 
            << "Test: Failed to remove pre-existing database_bk file at: " << QString(this->destinationFilePathTestDB + QStringLiteral("_bk")).toStdString();
    }

    // Copy the file
    bool copiedSuccessfully = QFile::copy(testFileName, this->destinationFilePathTestDB);
    ASSERT_TRUE(copiedSuccessfully) 
        << "Test: Failed to copy the database file from " << testFileName.toStdString() 
        << " to " << this->destinationFilePathTestDB.toStdString();
}

void SEND_PCM_FILE_TEST::TearDown()  {
    // Remove db file from the core
    bool removedSuccessfully = QFile::remove(this->destinationFilePathTestDB);
    ASSERT_TRUE(removedSuccessfully) 
        << "TearDown: Failed to remove the copied database file in: " << this->destinationFilePathTestDB.toStdString();
}

TEST_F(SEND_PCM_FILE_TEST, req_has_empty_path_NEGATIVE) {
    QString errorMsg;
    OpenAPI::OAISelectedExperimentsRequest experiments;
    this->request.setPath("");
    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    //call path converted cases
    bool result = OPGUICore::api_send_PCM_file(validatedRequest, experiments,errorMsg);
    
    ASSERT_FALSE(result);
}

TEST_F(SEND_PCM_FILE_TEST, req_has_correct_path_sim_requirements_not_met_NEGATIVE) {
    QString errorMsg;
    OpenAPI::OAISelectedExperimentsRequest experiments;
    this->request.setPath(this->destinationFilePathTestDB);
    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    ASSERT_TRUE(QFile::exists(this->destinationFilePathTestDB)) 
        << "Test file doesn't exist at: " << this->destinationFilePathTestDB.toStdString();

    QString backupName = this->destinationFilePathTestDB + QStringLiteral("_bk");
    QFile backupFile(backupName);
    if (backupFile.exists()) {
        backupFile.remove();
    }

    ASSERT_TRUE(QFile::rename(this->destinationFilePathTestDB,backupName))
        << "Test:Failed to rename the original file to the back up"
        << "Attempted to rename from: " << this->destinationFilePathTestDB.toStdString() 
        << " to: " << backupName.toStdString();

    bool result = OPGUICore::api_send_PCM_file(validatedRequest, experiments,errorMsg);
    
    EXPECT_FALSE(result)<<"Test: Result of api_send_PCM was expected to be false";;

    ASSERT_TRUE(QFile::rename(backupName, this->destinationFilePathTestDB))
        << "Test: Failed to rename the test file back to its original name after the test. "
        << "Attempted to rename from: " << backupName.toStdString() 
        << " to: " << this->destinationFilePathTestDB.toStdString();
}

TEST_F(SEND_PCM_FILE_TEST, req_sim_requirements_met_empty_db_file_NEGATIVE) {
    QString errorMsg;
    OpenAPI::OAISelectedExperimentsRequest experiments;

    this->request.setPath(this->destinationFilePathTestDB);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QString backupName = this->destinationFilePathTestDB + QStringLiteral("_bk");
    ASSERT_TRUE(QFile::exists(this->destinationFilePathTestDB)) 
        << "Test file doesn't exist at: " << this->destinationFilePathTestDB.toStdString();

    QFile backupFile(backupName);
    if (backupFile.exists()) {
        backupFile.remove();
    }

    ASSERT_TRUE(QFile::rename(this->destinationFilePathTestDB,backupName))
        << "Test:Failed to rename the original file to the back up"
        << "Attempted to rename from: " << this->destinationFilePathTestDB.toStdString() 
        << " to: " << backupName.toStdString();

    QFile emptyFile(this->destinationFilePathTestDB);
    ASSERT_TRUE(emptyFile.open(QIODevice::WriteOnly))
        << "Failed to create an empty file for the test.";
    emptyFile.close();
    
    bool result = OPGUICore::api_send_PCM_file(validatedRequest, experiments,errorMsg);
    EXPECT_FALSE(result) << "Test: Result of api_send_PCM was expected to be false";

    ASSERT_TRUE(emptyFile.remove())
        << "Failed to remove the empty file after the test.";

    ASSERT_TRUE(QFile::rename(backupName, this->destinationFilePathTestDB))
        << "Test: Failed to rename the test file back to its original name after the test. "
        << "Attempted to rename from: " << backupName.toStdString() 
        << " to: " << this->destinationFilePathTestDB.toStdString();
}


TEST_F(SEND_PCM_FILE_TEST, test_requirements_met_load_successfull_POSITIVE) {
    QString errorMsg;
    OpenAPI::OAISelectedExperimentsRequest experiments;
    this->request.setPath(this->destinationFilePathTestDB);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);
    
    bool result = OPGUICore::api_send_PCM_file(validatedRequest, experiments,errorMsg);

    ASSERT_TRUE(result)<<"Test: Result of api_send_PCM was expected to be true";

    ASSERT_EQ(experiments.getSelectedExperiments(), this->response200Expected.getSelectedExperiments())
        << "Test: experiments array does not have the expected value. Expected value is "
        << this->response200Expected.getSelectedExperiments().join(", ").toStdString() 
        << ", but got " 
        << experiments.getSelectedExperiments().join(", ").toStdString();
}




