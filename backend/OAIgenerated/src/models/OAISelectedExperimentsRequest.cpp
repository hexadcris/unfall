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

#include "OAISelectedExperimentsRequest.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAISelectedExperimentsRequest::OAISelectedExperimentsRequest(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAISelectedExperimentsRequest::OAISelectedExperimentsRequest() {
    this->initializeModel();
}

OAISelectedExperimentsRequest::~OAISelectedExperimentsRequest() {}

void OAISelectedExperimentsRequest::initializeModel() {

    m_selected_experiments_isSet = false;
    m_selected_experiments_isValid = false;
}

void OAISelectedExperimentsRequest::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAISelectedExperimentsRequest::fromJsonObject(QJsonObject json) {

    m_selected_experiments_isValid = ::OpenAPI::fromJsonValue(selected_experiments, json[QString("selectedExperiments")]);
    m_selected_experiments_isSet = !json[QString("selectedExperiments")].isNull() && m_selected_experiments_isValid;
}

QString OAISelectedExperimentsRequest::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAISelectedExperimentsRequest::asJsonObject() const {
    QJsonObject obj;
    if (selected_experiments.size() > 0) {
        obj.insert(QString("selectedExperiments"), ::OpenAPI::toJsonValue(selected_experiments));
    }
    return obj;
}

QList<QString> OAISelectedExperimentsRequest::getSelectedExperiments() const {
    return selected_experiments;
}
void OAISelectedExperimentsRequest::setSelectedExperiments(const QList<QString> &selected_experiments) {
    this->selected_experiments = selected_experiments;
    this->m_selected_experiments_isSet = true;
}

bool OAISelectedExperimentsRequest::is_selected_experiments_Set() const{
    return m_selected_experiments_isSet;
}

bool OAISelectedExperimentsRequest::is_selected_experiments_Valid() const{
    return m_selected_experiments_isValid;
}

bool OAISelectedExperimentsRequest::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (selected_experiments.size() > 0) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAISelectedExperimentsRequest::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_selected_experiments_isValid && true;
}

} // namespace OpenAPI
