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

#include "OAISimulationConfig.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAISimulationConfig::OAISimulationConfig(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAISimulationConfig::OAISimulationConfig() {
    this->initializeModel();
}

OAISimulationConfig::~OAISimulationConfig() {}

void OAISimulationConfig::initializeModel() {

    m_number_of_executions_isSet = false;
    m_number_of_executions_isValid = false;

    m_log_file_simulation_isSet = false;
    m_log_file_simulation_isValid = false;

    m_configurations_isSet = false;
    m_configurations_isValid = false;

    m_results_isSet = false;
    m_results_isValid = false;
}

void OAISimulationConfig::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAISimulationConfig::fromJsonObject(QJsonObject json) {

    m_number_of_executions_isValid = ::OpenAPI::fromJsonValue(number_of_executions, json[QString("numberOfExecutions")]);
    m_number_of_executions_isSet = !json[QString("numberOfExecutions")].isNull() && m_number_of_executions_isValid;

    m_log_file_simulation_isValid = ::OpenAPI::fromJsonValue(log_file_simulation, json[QString("logFileSimulation")]);
    m_log_file_simulation_isSet = !json[QString("logFileSimulation")].isNull() && m_log_file_simulation_isValid;

    m_configurations_isValid = ::OpenAPI::fromJsonValue(configurations, json[QString("configurations")]);
    m_configurations_isSet = !json[QString("configurations")].isNull() && m_configurations_isValid;

    m_results_isValid = ::OpenAPI::fromJsonValue(results, json[QString("results")]);
    m_results_isSet = !json[QString("results")].isNull() && m_results_isValid;
}

QString OAISimulationConfig::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAISimulationConfig::asJsonObject() const {
    QJsonObject obj;
    if (m_number_of_executions_isSet) {
        obj.insert(QString("numberOfExecutions"), ::OpenAPI::toJsonValue(number_of_executions));
    }
    if (m_log_file_simulation_isSet) {
        obj.insert(QString("logFileSimulation"), ::OpenAPI::toJsonValue(log_file_simulation));
    }
    if (m_configurations_isSet) {
        obj.insert(QString("configurations"), ::OpenAPI::toJsonValue(configurations));
    }
    if (m_results_isSet) {
        obj.insert(QString("results"), ::OpenAPI::toJsonValue(results));
    }
    return obj;
}

qint32 OAISimulationConfig::getNumberOfExecutions() const {
    return number_of_executions;
}
void OAISimulationConfig::setNumberOfExecutions(const qint32 &number_of_executions) {
    this->number_of_executions = number_of_executions;
    this->m_number_of_executions_isSet = true;
}

bool OAISimulationConfig::is_number_of_executions_Set() const{
    return m_number_of_executions_isSet;
}

bool OAISimulationConfig::is_number_of_executions_Valid() const{
    return m_number_of_executions_isValid;
}

QString OAISimulationConfig::getLogFileSimulation() const {
    return log_file_simulation;
}
void OAISimulationConfig::setLogFileSimulation(const QString &log_file_simulation) {
    this->log_file_simulation = log_file_simulation;
    this->m_log_file_simulation_isSet = true;
}

bool OAISimulationConfig::is_log_file_simulation_Set() const{
    return m_log_file_simulation_isSet;
}

bool OAISimulationConfig::is_log_file_simulation_Valid() const{
    return m_log_file_simulation_isValid;
}

QString OAISimulationConfig::getConfigurations() const {
    return configurations;
}
void OAISimulationConfig::setConfigurations(const QString &configurations) {
    this->configurations = configurations;
    this->m_configurations_isSet = true;
}

bool OAISimulationConfig::is_configurations_Set() const{
    return m_configurations_isSet;
}

bool OAISimulationConfig::is_configurations_Valid() const{
    return m_configurations_isValid;
}

QString OAISimulationConfig::getResults() const {
    return results;
}
void OAISimulationConfig::setResults(const QString &results) {
    this->results = results;
    this->m_results_isSet = true;
}

bool OAISimulationConfig::is_results_Set() const{
    return m_results_isSet;
}

bool OAISimulationConfig::is_results_Valid() const{
    return m_results_isValid;
}

bool OAISimulationConfig::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_number_of_executions_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_log_file_simulation_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_configurations_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_results_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAISimulationConfig::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_number_of_executions_isValid && m_log_file_simulation_isValid && m_configurations_isValid && m_results_isValid && true;
}

} // namespace OpenAPI
