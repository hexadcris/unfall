/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef OPENPASS_FMUFILEHELPER_H
#define OPENPASS_FMUFILEHELPER_H

#pragma once
#include <QString>
#include <map>
#include <optional>

class FmuFileHelper {

public:

    static std::string createAgentIDString (int id);
    static QString createOrOpenOutputFolder(const QString& outputDir, const QString& componentName, std::optional<const std::string> appendedFolder);

    static void writeBinaryTrace(const std::string &message, const QString &fileName, const QString& componentName, int time, std::map<std::string, std::pair<std::string, int>>& targetOutputTracesMap);
    static void writeTracesToFile(const QString& outputDir, const std::map<std::string, std::pair<std::string, int>>& fileToOutputTracesMap);
};


#endif //OPENPASS_FMUFILEHELPER_H
