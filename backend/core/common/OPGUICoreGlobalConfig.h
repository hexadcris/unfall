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

#ifndef OPGUICORE_GLOBALCONFIG_H
#define OPGUICORE_GLOBALCONFIG_H

#include <QString>

class OPGUICoreGlobalConfig {
public:
    static OPGUICoreGlobalConfig& getInstance();

    bool isInitializationSuccessful() const;

    // Getters
    QString componentsFolder() const;
    QString modulesFolder() const;
    QString pathDatabasePCM() const;
    QString agentCopyFile() const;
    QString agentFollowFile() const;
    QString pathConfigFile() const;
    QString pathLogFile() const;
    QString pathHome() const;
    int backendPort() const;
    QString backendIP() const;
    QString pathOpenpassCore() const;
    QString opSimulationExe() const;
    QString opSimulationManagerExe() const;
    QString opSimulationManagerXml() const;
    QString workspace() const;
    QString pathConvertedCases() const;
    QString fullPathOpSimulationExe() const;
    QString fullPathOpSimulationManagerExe() const;
    QString fullPathOpSimulationManagerXml() const;
    QString fullPathModulesFolder() const;
    QString fullPathComponentsFolder() const;
    QString fullPathAgentCopyFile() const;
    QString fullPathAgentFollowFile() const;
    QString systemsFolder() const;
    QString fullPathUserSystemsFolder() const;
    QString fullPathCoreSystemsFolder() const;

    // Setters
    void setComponentsFolder(const QString &value);
    void setModulesFolder(const QString &value);
    void setPathDatabasePCM(const QString &value);
    void setAgentCopyFile(const QString &value);
    void setPathAgentFollowFile(const QString &value);
    void setPathConfigFile(const QString &value);
    void setPathLogFile(const QString &value);
    void setPathHome(const QString &value);
    void setBackendPort(int value);
    void setBackendIP(const QString &value);
    void setPathOpenpassCore(const QString &value);
    void setOpSimulationExe(const QString &value);
    void setOpSimulationManagerExe(const QString &value);
    void setOpSimulationManagerXml(const QString &value);
    void setWorkspace(const QString &value);
    void setPathConvertedCases(const QString &value);
    void setSystemsFolder(const QString &path);


    bool modifyOrAddValueToConfigFile(const QString& key, const QJsonValue& value);

private:
    OPGUICoreGlobalConfig();
    ~OPGUICoreGlobalConfig() = default;

    void setDefaultValues();

    bool loadFromConfigFile(); 
    QString defaultPathConfigFile() const; 
    QString defaultPathHome() const;
    QString defaultOpSimulationManagerExe() const;
    QString defaultOpSimulationExe() const;
    QString cleanPathNoTrailing(const QString& originalPath) const;
    bool initializationSuccessful;

    // Member variables
    QString m_componentsFolder;
    QString m_modulesFolder;
    QString m_pathDatabasePCM;
    QString m_agentCopyFile;
    QString m_agentFollowFile;
    QString m_pathConfigFile;
    QString m_pathLogFile;
    QString m_pathHome;
    int m_backendPort;
    QString m_backendIP;
    QString m_pathOpenpassCore;
    QString m_opSimulationExe;
    QString m_opSimulationManagerExe;
    QString m_opSimulationManagerXml;
    QString m_workspace;
    QString m_pathConvertedCases;
    QString m_systemsFolder;
};

#endif // OPGUICORE_GLOBALCONFIG_H
