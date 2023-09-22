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

#include "OAITarget.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAITarget::OAITarget(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAITarget::OAITarget() {
    this->initializeModel();
}

OAITarget::~OAITarget() {}

void OAITarget::initializeModel() {

    m_component_isSet = false;
    m_component_isValid = false;

    m_input_isSet = false;
    m_input_isValid = false;
}

void OAITarget::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAITarget::fromJsonObject(QJsonObject json) {

    m_component_isValid = ::OpenAPI::fromJsonValue(component, json[QString("component")]);
    m_component_isSet = !json[QString("component")].isNull() && m_component_isValid;

    m_input_isValid = ::OpenAPI::fromJsonValue(input, json[QString("input")]);
    m_input_isSet = !json[QString("input")].isNull() && m_input_isValid;
}

QString OAITarget::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAITarget::asJsonObject() const {
    QJsonObject obj;
    if (m_component_isSet) {
        obj.insert(QString("component"), ::OpenAPI::toJsonValue(component));
    }
    if (m_input_isSet) {
        obj.insert(QString("input"), ::OpenAPI::toJsonValue(input));
    }
    return obj;
}

qint32 OAITarget::getComponent() const {
    return component;
}
void OAITarget::setComponent(const qint32 &component) {
    this->component = component;
    this->m_component_isSet = true;
}

bool OAITarget::is_component_Set() const{
    return m_component_isSet;
}

bool OAITarget::is_component_Valid() const{
    return m_component_isValid;
}

qint32 OAITarget::getInput() const {
    return input;
}
void OAITarget::setInput(const qint32 &input) {
    this->input = input;
    this->m_input_isSet = true;
}

bool OAITarget::is_input_Set() const{
    return m_input_isSet;
}

bool OAITarget::is_input_Valid() const{
    return m_input_isValid;
}

bool OAITarget::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_component_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_input_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAITarget::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_component_isValid && m_input_isValid && true;
}

} // namespace OpenAPI
