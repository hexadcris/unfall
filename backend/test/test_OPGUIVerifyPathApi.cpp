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
#include <QFile>

#include "test_helpers.h"
#include "OPGUICore.h"
#include "test_OPGUIVerifyPathApi.h"
#include "OPGUICoreGlobalConfig.h"
    
void VERIFY_PATH_TEST::SetUp()  {
    ASSERT_TRUE(OPGUICoreGlobalConfig::getInstance().isInitializationSuccessful())
            << "Failed to initialize the global configuration.";
    // Check if workspace exists.
    testDir = QStringLiteral("test");
    this->workSpacePath = OPGUICoreGlobalConfig::getInstance().workspace();
    this->testDirFullPath = TestHelpers::joinPaths(this->workSpacePath, testDir);
    QDir dir(this->workSpacePath);
    ASSERT_TRUE(dir.exists()) << "Workspace directory doesn't exist: " << this->workSpacePath.toStdString();

    // If test directory exists, remove it.
    QDir dirTest(this->testDirFullPath);
    if(dirTest.exists()){
        ASSERT_TRUE(dirTest.removeRecursively()) << "Failed to remove existing test directory: " << this->testDirFullPath.toStdString();
    }

    ASSERT_TRUE(dir.mkpath(this->testDirFullPath)) << "Failed to create test directory at: " << this->testDirFullPath.toStdString();
}

void VERIFY_PATH_TEST::TearDown()  {
    QDir dir(this->testDirFullPath);
    if(dir.exists()){
        ASSERT_TRUE(dir.removeRecursively()) << "Failed to remove test directory during tear down: " << this->testDirFullPath.toStdString();
    }
}

TEST_F(VERIFY_PATH_TEST, Verify_Path_Empty_Folder_Exists_Positive) {
    this->request.setPath(this->testDir);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    bool result = OPGUICore::api_verify_path(validatedRequest, this->response);

    EXPECT_TRUE(result) << "Verify path API call failed for empty folder";
    EXPECT_EQ(this->response.getRealPath(), TestHelpers::joinPaths(this->workSpacePath, this->request.getPath()))
        << "Real path "<<this->response.getRealPath().toStdString()
        << " does not match expected path " 
        << TestHelpers::joinPaths(this->workSpacePath, this->request.getPath()).toStdString();
    EXPECT_TRUE(this->response.isOk()) << "Response indicates failure for empty folder";
    EXPECT_TRUE(this->response.isEmpty()) << "Response is not empty for an empty folder";
}

TEST_F(VERIFY_PATH_TEST, Verify_Path_Folder_Containing_Folder_Exists_Positive) {
    this->request.setPath(this->testDir);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QString subfolder = QStringLiteral("test_sub");
    
    QDir dir(this->testDirFullPath);
    
    ASSERT_TRUE(dir.mkpath(TestHelpers::joinPaths(this->testDirFullPath, subfolder)))
        << "Failed to create subfolder for the test";

    bool result = OPGUICore::api_verify_path(validatedRequest, this->response);

    EXPECT_TRUE(result) << "Verify path API call failed for folder containing another folder";
    EXPECT_EQ(this->response.getRealPath(), TestHelpers::joinPaths(this->workSpacePath, this->request.getPath()))
        << "Real path does not match expected path for folder containing another folder";
    EXPECT_TRUE(this->response.isOk()) << "Response indicates failure for folder containing another folder";
    EXPECT_FALSE(this->response.isEmpty()) << "Response is unexpectedly empty for folder containing another folder";
}

TEST_F(VERIFY_PATH_TEST, Verify_Path_Folder_Containing_File_Exists_Positive) {
    this->request.setPath(this->testDir);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QString filename = QStringLiteral("test_file.xml");
    
    QFile file(TestHelpers::joinPaths(this->testDirFullPath, filename));
    ASSERT_TRUE(file.open(QIODevice::WriteOnly))
        << "Failed to create test file: " << filename.toStdString();
    file.close();

    bool result = OPGUICore::api_verify_path(validatedRequest, this->response);

    EXPECT_TRUE(result) << "Verify path API call failed for folder containing a file";
    EXPECT_EQ(this->response.getRealPath(), TestHelpers::joinPaths(this->workSpacePath, this->request.getPath()))
        << "Real path does not match expected path for folder containing a file";
    EXPECT_TRUE(this->response.isOk()) << "Response indicates failure for folder containing a file";
    EXPECT_FALSE(this->response.isEmpty()) << "Response is unexpectedly empty for folder containing a file";
}


TEST_F(VERIFY_PATH_TEST, Verify_Path_Folder_Containing_File_Searched_File_Positive) {
    this->request.setPath("test_file.xml");

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QString filename = QStringLiteral("test_file.xml");

    QFile file(TestHelpers::joinPaths(this->testDirFullPath, filename));
    ASSERT_TRUE(file.open(QIODevice::WriteOnly)) << "Failed to create test file: " << filename.toStdString();
    file.close();

    bool result = OPGUICore::api_verify_path(validatedRequest, this->response);

    EXPECT_TRUE(result) << "Verify path API call failed";
    EXPECT_EQ(this->response.getRealPath(), TestHelpers::joinPaths(this->testDirFullPath, this->request.getPath()))
        << "Real path mismatch";
    EXPECT_TRUE(this->response.isOk()) << "Response indicates failure";
    EXPECT_FALSE(this->response.isEmpty()) << "Response is unexpectedly empty";
}

TEST_F(VERIFY_PATH_TEST, Verify_Path_SubFolder_Containing_File_Searched_File_Positive) {
    this->request.setPath("test_file.xml");

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QString subfolder = QStringLiteral("test_sub");
    QDir dir(this->testDirFullPath);
    ASSERT_TRUE(dir.mkpath(TestHelpers::joinPaths(this->testDirFullPath,subfolder)));

    QString filename = QStringLiteral("test_file.xml");
    QFile file(TestHelpers::joinPaths(TestHelpers::joinPaths(this->testDirFullPath,"test_sub"),filename));
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.close();

    bool result = OPGUICore::api_verify_path(validatedRequest, this->response);

    EXPECT_TRUE(result) << "Verify path API call failed";
    EXPECT_EQ(this->response.getRealPath(), TestHelpers::joinPaths(TestHelpers::joinPaths(this->testDirFullPath,"test_sub"), this->request.getPath())) << "Real path doesn't match the expected path";
    EXPECT_TRUE(this->response.isOk()) << "Expected response to be OK but it was not";
    EXPECT_FALSE(this->response.isEmpty()) << "Expected response not to be empty, but it was";
}

TEST_F(VERIFY_PATH_TEST, Verify_path_two_folders_with_same_name_in_different_levels_Negative) {
    this->request.setPath(this->testDir);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    QString subfolder = QStringLiteral("test");
    
    QDir dir(this->testDirFullPath);
    
    ASSERT_TRUE(dir.mkpath(TestHelpers::joinPaths(this->testDirFullPath,subfolder)));

    bool result = OPGUICore::api_verify_path(validatedRequest, this->response);

    EXPECT_TRUE(result) << "Verify path API call failed";
    EXPECT_EQ(this->response.getRealPath(), "Folder/File found more than once inside the workspace directory or subdirectories") << "Real path message does not indicate multiple folder/file findings";
    EXPECT_FALSE(this->response.isOk()) << "Expected response not to be OK but it was";
    EXPECT_TRUE(this->response.isEmpty()) << "Expected response to be empty, but it was not";
}

TEST_F(VERIFY_PATH_TEST, Verify_Path_Folder_Exists_Negative) {
    QString subfolder = QStringLiteral("test_not_exist");
    request.setPath(subfolder);

    QJsonObject jsonRepresentation = this->request.asJsonObject();
    OpenAPI::OAIPathRequest validatedRequest;
    validatedRequest.fromJsonObject(jsonRepresentation);

    bool result = OPGUICore::api_verify_path(validatedRequest, this->response);

    EXPECT_TRUE(result) << "Verify path API call unexpectedly failed for non-existing subfolder";
    EXPECT_FALSE(this->response.isOk()) << "Response unexpectedly indicates success";
    EXPECT_TRUE(this->response.isEmpty()) << "Response is not empty for non-existing subfolder";
}


