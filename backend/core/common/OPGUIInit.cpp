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

#include "OPGUIInit.h"
#include <QDir>
#include <QFile>
#include <QException>

#include <OPGUICore.h>
#include <OPGUICoreGlobalConfig.h>
#include <OPGUIQtLogger.h>

namespace OPGUIInit {

    bool Initialise_OPGUI() {
        LOG_INFO("Initialising opGUI");
        bool isInitialised = true;

        struct CheckItem {
            QString path;
            QString description;  // New field for description
            bool isDirectory;
            bool createIfNotExists;
        };

        QList<CheckItem> checkList = {
            {OPGUICoreGlobalConfig::getInstance().workspace(), "Workspace", true, true},
            {OPGUICoreGlobalConfig::getInstance().pathConvertedCases(), "Converted Cases Path", true, false},
            {OPGUICoreGlobalConfig::getInstance().pathOpenpassCore(), "Path to Openpass Core", true, false},
            {OPGUICoreGlobalConfig::getInstance().fullPathModulesFolder(), "Modules Folder Path", true, false},
            {OPGUICoreGlobalConfig::getInstance().fullPathComponentsFolder(), "Components Folder Path", true, false},
            {OPGUICoreGlobalConfig::getInstance().fullPathUserSystemsFolder(), "User Systems Folder Path", true, false},
            {OPGUICoreGlobalConfig::getInstance().fullPathOpSimulationExe(), "Op Simulation Executable Path", false, false},
            {OPGUICoreGlobalConfig::getInstance().fullPathOpSimulationManagerExe(), "Op Simulation Manager Executable Path", false, false},
            {OPGUICoreGlobalConfig::getInstance().pathLogFile(), "Log File Path", false, false}
        };

        for (const auto& item : checkList) {
            if (containsPlaceholder(item.path)) {
                LOG_ERROR("Placeholder detected in " + item.description + ": " + item.path + ". Please update the configuration with actual values.");
                return false;  // Halt initialization
            }

            if (item.isDirectory) {
                isInitialised &= checkAndCreateDir(item.path, item.description, item.createIfNotExists);
            } else {
                isInitialised &= checkFileExists(item.path, item.description);
            }
        }

        OPGUICore::setSimulationRunning(false);
        return isInitialised;
    }

    bool containsPlaceholder(const QString& str) {
        return str.contains("<") && str.contains(">");
    }

    bool checkAndCreateDir(const QString& path, const QString& description, bool createIfNotExists) {
        QDir dir(path);
        if (dir.exists()) {
            LOG_INFO(description + " DIR found at: " + path);
            return true;
        } else {
            LOG_ERROR(description + " DIR not found at: " + path);
            if (createIfNotExists && dir.mkpath(path)) {
                LOG_INFO("Created DIR: " + path);
                return true;
            } else if (createIfNotExists) {
                LOG_ERROR("Failed to create DIR: " + path);
            }
            return false;
        }
    }

    bool checkFileExists(const QString& filePath, const QString& description) {
        if (QFile::exists(filePath)) {
            LOG_INFO(description + " file found at: " + filePath);
            return true;
        } else {
            LOG_ERROR(description + " file not found at: " + filePath);
            return false;
        }
    }

} // namespace OPGUIInit
