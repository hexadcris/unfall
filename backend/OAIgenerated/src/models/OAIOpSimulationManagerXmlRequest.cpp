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

#include "OAIOpSimulationManagerXmlRequest.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIOpSimulationManagerXmlRequest::OAIOpSimulationManagerXmlRequest(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIOpSimulationManagerXmlRequest::OAIOpSimulationManagerXmlRequest() {
    this->initializeModel();
}

OAIOpSimulationManagerXmlRequest::~OAIOpSimulationManagerXmlRequest() {}

void OAIOpSimulationManagerXmlRequest::initializeModel() {

    m_log_level_isSet = false;
    m_log_level_isValid = false;

    m_log_file_simulation_manager_isSet = false;
    m_log_file_simulation_manager_isValid = false;

    m_simulation_isSet = false;
    m_simulation_isValid = false;

    m_libraries_isSet = false;
    m_libraries_isValid = false;

    m_simulation_configs_isSet = false;
    m_simulation_configs_isValid = false;
}

void OAIOpSimulationManagerXmlRequest::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIOpSimulationManagerXmlRequest::fromJsonObject(QJsonObject json) {

    m_log_level_isValid = ::OpenAPI::fromJsonValue(log_level, json[QString("logLevel")]);
    m_log_level_isSet = !json[QString("logLevel")].isNull() && m_log_level_isValid;

    m_log_file_simulation_manager_isValid = ::OpenAPI::fromJsonValue(log_file_simulation_manager, json[QString("logFileSimulationManager")]);
    m_log_file_simulation_manager_isSet = !json[QString("logFileSimulationManager")].isNull() && m_log_file_simulation_manager_isValid;

    m_simulation_isValid = ::OpenAPI::fromJsonValue(simulation, json[QString("simulation")]);
    m_simulation_isSet = !json[QString("simulation")].isNull() && m_simulation_isValid;

    m_libraries_isValid = ::OpenAPI::fromJsonValue(libraries, json[QString("libraries")]);
    m_libraries_isSet = !json[QString("libraries")].isNull() && m_libraries_isValid;

    m_simulation_configs_isValid = ::OpenAPI::fromJsonValue(simulation_configs, json[QString("simulationConfigs")]);
    m_simulation_configs_isSet = !json[QString("simulationConfigs")].isNull() && m_simulation_configs_isValid;
}

QString OAIOpSimulationManagerXmlRequest::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIOpSimulationManagerXmlRequest::asJsonObject() const {
    QJsonObject obj;
    if (m_log_level_isSet) {
        obj.insert(QString("logLevel"), ::OpenAPI::toJsonValue(log_level));
    }
    if (m_log_file_simulation_manager_isSet) {
        obj.insert(QString("logFileSimulationManager"), ::OpenAPI::toJsonValue(log_file_simulation_manager));
    }
    if (m_simulation_isSet) {
        obj.insert(QString("simulation"), ::OpenAPI::toJsonValue(simulation));
    }
    if (m_libraries_isSet) {
        obj.insert(QString("libraries"), ::OpenAPI::toJsonValue(libraries));
    }
    if (simulation_configs.size() > 0) {
        obj.insert(QString("simulationConfigs"), ::OpenAPI::toJsonValue(simulation_configs));
    }
    return obj;
}

qint32 OAIOpSimulationManagerXmlRequest::getLogLevel() const {
    return log_level;
}
void OAIOpSimulationManagerXmlRequest::setLogLevel(const qint32 &log_level) {
    this->log_level = log_level;
    this->m_log_level_isSet = true;
}

bool OAIOpSimulationManagerXmlRequest::is_log_level_Set() const{
    return m_log_level_isSet;
}

bool OAIOpSimulationManagerXmlRequest::is_log_level_Valid() const{
    return m_log_level_isValid;
}

QString OAIOpSimulationManagerXmlRequest::getLogFileSimulationManager() const {
    return log_file_simulation_manager;
}
void OAIOpSimulationManagerXmlRequest::setLogFileSimulationManager(const QString &log_file_simulation_manager) {
    this->log_file_simulation_manager = log_file_simulation_manager;
    this->m_log_file_simulation_manager_isSet = true;
}

bool OAIOpSimulationManagerXmlRequest::is_log_file_simulation_manager_Set() const{
    return m_log_file_simulation_manager_isSet;
}

bool OAIOpSimulationManagerXmlRequest::is_log_file_simulation_manager_Valid() const{
    return m_log_file_simulation_manager_isValid;
}

QString OAIOpSimulationManagerXmlRequest::getSimulation() const {
    return simulation;
}
void OAIOpSimulationManagerXmlRequest::setSimulation(const QString &simulation) {
    this->simulation = simulation;
    this->m_simulation_isSet = true;
}

bool OAIOpSimulationManagerXmlRequest::is_simulation_Set() const{
    return m_simulation_isSet;
}

bool OAIOpSimulationManagerXmlRequest::is_simulation_Valid() const{
    return m_simulation_isValid;
}

QString OAIOpSimulationManagerXmlRequest::getLibraries() const {
    return libraries;
}
void OAIOpSimulationManagerXmlRequest::setLibraries(const QString &libraries) {
    this->libraries = libraries;
    this->m_libraries_isSet = true;
}

bool OAIOpSimulationManagerXmlRequest::is_libraries_Set() const{
    return m_libraries_isSet;
}

bool OAIOpSimulationManagerXmlRequest::is_libraries_Valid() const{
    return m_libraries_isValid;
}

QList<OAISimulationConfig> OAIOpSimulationManagerXmlRequest::getSimulationConfigs() const {
    return simulation_configs;
}
void OAIOpSimulationManagerXmlRequest::setSimulationConfigs(const QList<OAISimulationConfig> &simulation_configs) {
    this->simulation_configs = simulation_configs;
    this->m_simulation_configs_isSet = true;
}

bool OAIOpSimulationManagerXmlRequest::is_simulation_configs_Set() const{
    return m_simulation_configs_isSet;
}

bool OAIOpSimulationManagerXmlRequest::is_simulation_configs_Valid() const{
    return m_simulation_configs_isValid;
}

bool OAIOpSimulationManagerXmlRequest::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_log_level_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_log_file_simulation_manager_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_simulation_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_libraries_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (simulation_configs.size() > 0) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIOpSimulationManagerXmlRequest::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_log_level_isValid && m_log_file_simulation_manager_isValid && m_simulation_isValid && m_libraries_isValid && true;
}

} // namespace OpenAPI
