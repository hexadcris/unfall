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

#include "OAIConfigsRequest.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIConfigsRequest::OAIConfigsRequest(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIConfigsRequest::OAIConfigsRequest() {
    this->initializeModel();
}

OAIConfigsRequest::~OAIConfigsRequest() {}

void OAIConfigsRequest::initializeModel() {

    m_selected_systems_isSet = false;
    m_selected_systems_isValid = false;

    m_selected_experiments_isSet = false;
    m_selected_experiments_isValid = false;
}

void OAIConfigsRequest::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIConfigsRequest::fromJsonObject(QJsonObject json) {

    m_selected_systems_isValid = ::OpenAPI::fromJsonValue(selected_systems, json[QString("selectedSystems")]);
    m_selected_systems_isSet = !json[QString("selectedSystems")].isNull() && m_selected_systems_isValid;

    m_selected_experiments_isValid = ::OpenAPI::fromJsonValue(selected_experiments, json[QString("selectedExperiments")]);
    m_selected_experiments_isSet = !json[QString("selectedExperiments")].isNull() && m_selected_experiments_isValid;
}

QString OAIConfigsRequest::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIConfigsRequest::asJsonObject() const {
    QJsonObject obj;
    if (selected_systems.size() > 0) {
        obj.insert(QString("selectedSystems"), ::OpenAPI::toJsonValue(selected_systems));
    }
    if (selected_experiments.size() > 0) {
        obj.insert(QString("selectedExperiments"), ::OpenAPI::toJsonValue(selected_experiments));
    }
    return obj;
}

QList<OAISystemRef> OAIConfigsRequest::getSelectedSystems() const {
    return selected_systems;
}
void OAIConfigsRequest::setSelectedSystems(const QList<OAISystemRef> &selected_systems) {
    this->selected_systems = selected_systems;
    this->m_selected_systems_isSet = true;
}

bool OAIConfigsRequest::is_selected_systems_Set() const{
    return m_selected_systems_isSet;
}

bool OAIConfigsRequest::is_selected_systems_Valid() const{
    return m_selected_systems_isValid;
}

QList<QString> OAIConfigsRequest::getSelectedExperiments() const {
    return selected_experiments;
}
void OAIConfigsRequest::setSelectedExperiments(const QList<QString> &selected_experiments) {
    this->selected_experiments = selected_experiments;
    this->m_selected_experiments_isSet = true;
}

bool OAIConfigsRequest::is_selected_experiments_Set() const{
    return m_selected_experiments_isSet;
}

bool OAIConfigsRequest::is_selected_experiments_Valid() const{
    return m_selected_experiments_isValid;
}

bool OAIConfigsRequest::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (selected_systems.size() > 0) {
            isObjectUpdated = true;
            break;
        }

        if (selected_experiments.size() > 0) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIConfigsRequest::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_selected_systems_isValid && m_selected_experiments_isValid && true;
}

} // namespace OpenAPI
