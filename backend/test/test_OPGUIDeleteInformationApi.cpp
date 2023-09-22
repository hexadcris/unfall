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
#include <QJsonObject>
#include <QDir>

#include "test_helpers.h"
#include "test_OPGUIDeleteInformationApi.h"
#include "OPGUICoreGlobalConfig.h"


void DELETE_INFORMATION_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
    // Check workspace exist
    response200Expected.setResponse("true");
    testDir = QStringLiteral("test");
    this->workSpacePath = OPGUICoreGlobalConfig::getInstance().workspace();
    this->testDirFullPath = TestHelpers::joinPaths(this->workSpacePath, testDir);
    QDir dir(this->workSpacePath);

    ASSERT_TRUE(dir.exists()) 
        << "The workspace directory does not exist: " << this->workSpacePath.toStdString();

    // If test dir exists, remove it
    QDir dirTest(this->testDirFullPath);
    if (dirTest.exists()) {
        ASSERT_TRUE(dirTest.removeRecursively()) 
            << "Failed to remove the test directory: " << this->testDirFullPath.toStdString();
    }

    ASSERT_TRUE(dir.mkpath(this->testDirFullPath)) 
        << "Failed to create directory path: " << this->testDirFullPath.toStdString();
}

void DELETE_INFORMATION_TEST::TearDown()  {
    // If test dir exists, remove it
    QDir dirTest(this->testDirFullPath);
    if (dirTest.exists()) {
        ASSERT_TRUE(dirTest.removeRecursively()) 
            << "Failed to remove the test directory: " << this->testDirFullPath.toStdString();
    }
}

TEST_F(DELETE_INFORMATION_TEST, Delete_Info_Containing_Folders_Positive) {
    QString errorMsg;
    QString subfolder1 = QStringLiteral("test_sub1");
    QString subfolder2 = QStringLiteral("test_sub2");
    this->request.setPath(this->testDirFullPath);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QDir dir(this->testDirFullPath);

    // Check if the subdirectories were successfully created
    ASSERT_TRUE(dir.mkpath(TestHelpers::joinPaths(this->testDirFullPath, subfolder1)))
        << "Failed to create subdirectory: " << TestHelpers::joinPaths(this->testDirFullPath, subfolder1).toStdString();

    ASSERT_TRUE(dir.mkpath(TestHelpers::joinPaths(this->testDirFullPath, subfolder2)))
        << "Failed to create subdirectory: " << TestHelpers::joinPaths(this->testDirFullPath, subfolder2).toStdString();

    bool result = OPGUICore::api_delete_information(validatedRequest, this->response,errorMsg);

    EXPECT_TRUE(result);
    EXPECT_EQ(this->response.getResponse(), this->response200Expected.getResponse())
        << "Test: Delete information function did not return expected response";

    // Check if the directory is empty after the delete operation
    ASSERT_TRUE(dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty())
        << "The directory is not empty after the delete operation: " << this->testDirFullPath.toStdString();

}

TEST_F(DELETE_INFORMATION_TEST, Delete_Info_Containing_Files_Positive) {
    QString errorMsg;
    QString file1Name = QStringLiteral("test_file1.xml");
    QString file2Name = QStringLiteral("test_file2.txt");
    this->request.setPath(this->testDirFullPath);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QDir dir(this->testDirFullPath);

    // Ensure file1 is created successfully
    QFile file1(dir.filePath(file1Name));
    ASSERT_TRUE(file1.open(QIODevice::WriteOnly))
        << "Failed to create and open file: " << file1Name.toStdString();
    file1.close();

    // Ensure file2 is created successfully
    QFile file2(dir.filePath(file2Name));
    ASSERT_TRUE(file2.open(QIODevice::WriteOnly))
        << "Failed to create and open file: " << file2Name.toStdString();
    file2.close();

    bool result = OPGUICore::api_delete_information(validatedRequest, this->response,errorMsg);

    EXPECT_TRUE(result);
    EXPECT_EQ(this->response.getResponse(), this->response200Expected.getResponse())
        << "Test: Delete information function did not return expected response";

    // Check if the directory is empty after the delete operation
    ASSERT_TRUE(dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty())
        << "The directory is not empty after the delete operation: " << this->testDirFullPath.toStdString();
}

TEST_F(DELETE_INFORMATION_TEST, Delete_Info_Containing_Folders_and_Files_Positive) {
    QString errorMsg;
    QString subfolder = QStringLiteral("test_sub1");
    QString fileName = QStringLiteral("test_file.xml");
    this->request.setPath(this->testDirFullPath);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QDir dir(this->testDirFullPath);

    // Check if subfolder is created successfully
    ASSERT_TRUE(dir.mkpath(TestHelpers::joinPaths(this->testDirFullPath, subfolder)))
        << "Failed to create subfolder: " << subfolder.toStdString();

    // Ensure file is created successfully
    QFile file(dir.filePath(fileName));
    ASSERT_TRUE(file.open(QIODevice::WriteOnly))
        << "Failed to create and open file: " << fileName.toStdString();
    file.close();

    bool result = OPGUICore::api_delete_information(validatedRequest, this->response,errorMsg);

    EXPECT_TRUE(result);
    EXPECT_EQ(this->response.getResponse(), this->response200Expected.getResponse())
        << "Test: Delete information function did not return expected response";

    // Check if the directory is empty after the delete operation
    ASSERT_TRUE(dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty())
        << "The directory is not empty after the delete operation: " << this->testDirFullPath.toStdString();

}

TEST_F(DELETE_INFORMATION_TEST, Delete_Info_Empty_Folder_Positive) {
    QString errorMsg;
    this->request.setPath(this->testDirFullPath);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QDir dir(this->testDirFullPath);

    bool result = OPGUICore::api_delete_information(validatedRequest, this->response,errorMsg);

    EXPECT_TRUE(result)
        << "Delete information function call failed for path: " << this->testDirFullPath.toStdString();

    EXPECT_EQ(this->response.getResponse(), this->response200Expected.getResponse())
        << "Test: Delete information function did not return expected response. Expected: " 
        << this->response200Expected.getResponse().toStdString() << " but got: " 
        << this->response.getResponse().toStdString();

    // Check if the directory is empty after the delete operation
    ASSERT_TRUE(dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty())
        << "The directory is not empty after the delete operation: " << this->testDirFullPath.toStdString();
}

TEST_F(DELETE_INFORMATION_TEST, Delete_Non_Existing_Folder_Negative) {
    QString errorMsg;
    this->request.setPath("not_existing_wrong_path");

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    bool result = OPGUICore::api_delete_information(validatedRequest, this->response,errorMsg);

    EXPECT_FALSE(result)
        << "Delete information function call unexpectedly succeeded for non-existing path: "
        << "not_existing_wrong_path";

    ASSERT_TRUE(errorMsg.contains("Invalid Path, Path does not exist"))<<"Error message was expected to contain 'Invalid Path, Path does not exist' but was "+errorMsg.toStdString();
}


