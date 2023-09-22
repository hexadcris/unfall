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

#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QDir>

#include "OPGUICoreGlobalConfig.h"

OPGUICoreGlobalConfig::OPGUICoreGlobalConfig(): initializationSuccessful(true) {
    setDefaultValues();
    this->initializationSuccessful = loadFromConfigFile();
}

QString OPGUICoreGlobalConfig::componentsFolder() const { return m_componentsFolder; }
QString OPGUICoreGlobalConfig::systemsFolder() const { return m_systemsFolder; }
QString OPGUICoreGlobalConfig::modulesFolder() const { return m_modulesFolder; }
QString OPGUICoreGlobalConfig::pathDatabasePCM() const { return this->cleanPathNoTrailing(m_pathDatabasePCM); }
QString OPGUICoreGlobalConfig::agentCopyFile() const { return m_agentCopyFile; }
QString OPGUICoreGlobalConfig::agentFollowFile() const { return m_agentFollowFile; }
QString OPGUICoreGlobalConfig::pathConfigFile() const { return m_pathConfigFile; }
QString OPGUICoreGlobalConfig::pathHome() const { return this->cleanPathNoTrailing(m_pathHome); }
int OPGUICoreGlobalConfig::backendPort() const { return m_backendPort; }
QString OPGUICoreGlobalConfig::backendIP() const { return m_backendIP; }
QString OPGUICoreGlobalConfig::pathOpenpassCore() const { return this->cleanPathNoTrailing(m_pathOpenpassCore); }
QString OPGUICoreGlobalConfig::opSimulationExe() const { return m_opSimulationExe; }
QString OPGUICoreGlobalConfig::opSimulationManagerExe() const { return m_opSimulationManagerExe; }
QString OPGUICoreGlobalConfig::opSimulationManagerXml() const { return m_opSimulationManagerXml; }
QString OPGUICoreGlobalConfig::workspace() const { return this->cleanPathNoTrailing(m_workspace); }
QString OPGUICoreGlobalConfig::pathConvertedCases() const { 
    if(m_pathConvertedCases.isEmpty()){
        return this->workspace();
    }
    else{
        return this->cleanPathNoTrailing(m_pathConvertedCases); 
    }
}
QString OPGUICoreGlobalConfig::pathLogFile() const { 
    if(m_pathLogFile.isEmpty()){
        return this->workspace()+"/"+"opGuiCore.log";
    }
    else{
        return m_pathLogFile; 
    }
}
QString OPGUICoreGlobalConfig::fullPathOpSimulationExe() const { return this->pathOpenpassCore()+"/"+m_opSimulationExe; }
QString OPGUICoreGlobalConfig::fullPathOpSimulationManagerExe() const { return this->pathOpenpassCore()+"/"+m_opSimulationManagerExe; }
QString OPGUICoreGlobalConfig::fullPathOpSimulationManagerXml() const { return this->pathOpenpassCore()+"/"+m_opSimulationManagerXml;}
QString OPGUICoreGlobalConfig::fullPathModulesFolder() const { return this->pathOpenpassCore()+"/"+m_modulesFolder;}
QString OPGUICoreGlobalConfig::fullPathComponentsFolder() const { return this->pathOpenpassCore()+"/"+m_componentsFolder;}
QString OPGUICoreGlobalConfig::fullPathAgentCopyFile() const { return this->pathOpenpassCore()+"/"+m_agentCopyFile;}
QString OPGUICoreGlobalConfig::fullPathAgentFollowFile() const { return this->pathOpenpassCore()+"/"+m_agentFollowFile;}

QString OPGUICoreGlobalConfig::fullPathUserSystemsFolder() const { return this->workspace()+"/"+m_systemsFolder;}
QString OPGUICoreGlobalConfig::fullPathCoreSystemsFolder() const { return this->pathOpenpassCore()+"/"+m_systemsFolder;}

// Setters
void OPGUICoreGlobalConfig::setComponentsFolder(const QString &path) { m_componentsFolder = path; }
void OPGUICoreGlobalConfig::setModulesFolder(const QString &path) { m_modulesFolder = path; }
void OPGUICoreGlobalConfig::setPathDatabasePCM(const QString &path) { m_pathDatabasePCM = path; }
void OPGUICoreGlobalConfig::setAgentCopyFile(const QString &path) { m_agentCopyFile = path; }
void OPGUICoreGlobalConfig::setPathAgentFollowFile(const QString &path) { m_agentFollowFile = path; }
void OPGUICoreGlobalConfig::setPathConfigFile(const QString &path) { m_pathConfigFile = path; }
void OPGUICoreGlobalConfig::setPathLogFile(const QString &path) { m_pathLogFile = path; }
void OPGUICoreGlobalConfig::setPathHome(const QString &path) { m_pathHome = path; }
void OPGUICoreGlobalConfig::setBackendPort(int port) { m_backendPort = port; }
void OPGUICoreGlobalConfig::setBackendIP(const QString &ip) { m_backendIP = ip; }
void OPGUICoreGlobalConfig::setPathOpenpassCore(const QString &path) { m_pathOpenpassCore = path; }
void OPGUICoreGlobalConfig::setOpSimulationExe(const QString &exe) { m_opSimulationExe = exe; }
void OPGUICoreGlobalConfig::setOpSimulationManagerExe(const QString &exe) { m_opSimulationManagerExe = exe; }
void OPGUICoreGlobalConfig::setOpSimulationManagerXml(const QString &xml) { m_opSimulationManagerXml = xml; }
void OPGUICoreGlobalConfig::setWorkspace(const QString &workspace) { m_workspace = workspace; }
void OPGUICoreGlobalConfig::setPathConvertedCases(const QString &path) { m_pathConvertedCases = path; }
void OPGUICoreGlobalConfig::setSystemsFolder(const QString &path) { m_systemsFolder = path; }

bool OPGUICoreGlobalConfig::isInitializationSuccessful() const{
    return this->initializationSuccessful;
}

void OPGUICoreGlobalConfig::setDefaultValues() {
    m_componentsFolder = QStringLiteral("components");
    m_systemsFolder = QStringLiteral("systems");
    m_modulesFolder = QStringLiteral("modules");
    m_pathDatabasePCM = QStringLiteral("");
    m_agentCopyFile = QStringLiteral("Agent_Copy.xml");
    m_agentFollowFile = QStringLiteral("Agent_Follow.xml");
    m_pathConfigFile = defaultPathConfigFile();
    m_pathHome = defaultPathHome();
    m_backendPort = 8848;
    m_backendIP = QStringLiteral("127.0.0.1");
    m_pathOpenpassCore = QStringLiteral("");
    m_opSimulationExe = defaultOpSimulationExe();
    m_opSimulationManagerXml = QStringLiteral("opSimulationManager.xml");
    m_opSimulationManagerExe = defaultOpSimulationManagerExe();
    m_workspace = QStringLiteral("");
    m_pathConvertedCases = QStringLiteral("");
    m_pathLogFile = QStringLiteral("");
}

QString OPGUICoreGlobalConfig::defaultOpSimulationExe() const {
#ifdef _WIN32
    return QStringLiteral("opSimulation.exe");
#elif __APPLE__ || __linux__
    return QStringLiteral("opSimulation");
#else
    return QStringLiteral("");
#endif
}

QString OPGUICoreGlobalConfig::defaultOpSimulationManagerExe() const {
#ifdef _WIN32
    return QStringLiteral("opSimulationManager.exe");
#elif __APPLE__ || __linux__
    return QStringLiteral("opSimulationManager");
#else
    return QStringLiteral("");
#endif
}

    QString OPGUICoreGlobalConfig::defaultPathConfigFile() const {
#ifdef _WIN32
        return QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first() + "\\opgui\\backend\\backendConfig.json";
#elif __APPLE__ || __linux__
        return QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first() + "/opgui/backend/backendConfig.json";
#else
        return QStringLiteral("");
#endif
    }

QString OPGUICoreGlobalConfig::defaultPathHome() const {
    return QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
}

bool OPGUICoreGlobalConfig::loadFromConfigFile() {
    QString configFilePath = defaultPathConfigFile();
    QFile configFile(configFilePath);

    if (!configFile.exists()) {
        qDebug() << "Config file does not exist at path:" << configFilePath;
        return false; // or however you'd like to handle this
    }

    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open config file for reading at path:" << configFilePath;
        return false; // or however you'd like to handle this
    }

    QByteArray fileContent = configFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(fileContent);

    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Invalid or malformed JSON content in config file at path:" << configFilePath;
        configFile.close();
        return false;
    }

    QJsonObject jsonObj = doc.object();

    // Check and load values from JSON, only if they exist
    if (jsonObj.contains("BACKEND_PORT")) {
        m_backendPort = jsonObj["BACKEND_PORT"].toInt();
    }

    if (jsonObj.contains("BACKEND_IP")) {
        m_backendIP = jsonObj["BACKEND_IP"].toString();
    }

    if (jsonObj.contains("PATH_DATABASE_PCM")) {
        m_pathDatabasePCM = jsonObj["PATH_DATABASE_PCM"].toString();
    }

    if (jsonObj.contains("AGENT_COPY_FILE")) {
        m_agentCopyFile = jsonObj["AGENT_COPY_FILE"].toString();
    }

    if (jsonObj.contains("AGENT_FOLLOW_FILE")) {
        m_agentFollowFile = jsonObj["AGENT_FOLLOW_FILE"].toString();
    }

    if (jsonObj.contains("MODULES_FOLDER")) {
        m_modulesFolder = jsonObj["MODULES_FOLDER"].toString();
    }

    if (jsonObj.contains("COMPONENTS_FOLDER")) {
        m_componentsFolder = jsonObj["COMPONENTS_FOLDER"].toString();
    }

    if (jsonObj.contains("SYSTEMS_FOLDER")) {
        m_systemsFolder = jsonObj["SYSTEMS_FOLDER"].toString();
    }

    if (jsonObj.contains("PATH_OPENPASS_CORE")) {
        m_pathOpenpassCore = jsonObj["PATH_OPENPASS_CORE"].toString();
    }

    if (jsonObj.contains("OPSIMULATION_EXE")) {
        m_opSimulationExe = jsonObj["OPSIMULATION_EXE"].toString();
    }

    if (jsonObj.contains("OPSIMULATION_MANAGER_XML")) {
        m_opSimulationManagerXml = jsonObj["OPSIMULATION_MANAGER_XML"].toString();
    }

    if (jsonObj.contains("OPSIMULATION_MANAGER_EXE")) {
        m_opSimulationManagerExe = jsonObj["OPSIMULATION_MANAGER_EXE"].toString();
    }

    if (jsonObj.contains("WORKSPACE")) {
        m_workspace = jsonObj["WORKSPACE"].toString();
    }

    if (jsonObj.contains("PATH_CONVERTED_CASES")) {
        m_pathConvertedCases = jsonObj["PATH_CONVERTED_CASES"].toString();
    }

    if (jsonObj.contains("PATH_LOG_FILE")) {
        m_pathLogFile = jsonObj["PATH_LOG_FILE"].toString();
    }

    configFile.close();
    return true;
 
}

OPGUICoreGlobalConfig& OPGUICoreGlobalConfig::getInstance() {
    static OPGUICoreGlobalConfig instance;
    return instance;
}

QString OPGUICoreGlobalConfig::cleanPathNoTrailing(const QString& originalPath) const {
    QString cleanedPath = QDir::cleanPath(originalPath);
    if (cleanedPath.endsWith('/') || cleanedPath.endsWith('\\')) {
        cleanedPath.chop(1);
    }
    return cleanedPath;
}

bool OPGUICoreGlobalConfig::modifyOrAddValueToConfigFile(const QString& key, const QJsonValue& value) {
    QFile configFile(this->m_pathConfigFile);
    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Unable to open config file for reading:" << this->m_pathConfigFile;
        return false;
    }
    QByteArray jsonData = configFile.readAll();
    configFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        if (doc.isNull()) {
            qDebug() << "Error: JSON document parsed from config file is null.";
            qDebug() << "File path:" << this->m_pathConfigFile;
            qDebug() << "JSON Data:" << jsonData; // This will show the actual content of jsonData
            return false;
        }
    }

    QJsonObject obj = doc.object();
    obj[key] = value;
    doc.setObject(obj);
    if (!configFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Unable to open config file for writing:" << this->m_pathConfigFile;
        return false;
    }
    configFile.write(doc.toJson());
    configFile.close();

    return true;
}



