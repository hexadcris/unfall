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

#include "OAISource.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAISource::OAISource(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAISource::OAISource() {
    this->initializeModel();
}

OAISource::~OAISource() {}

void OAISource::initializeModel() {

    m_component_isSet = false;
    m_component_isValid = false;

    m_output_isSet = false;
    m_output_isValid = false;
}

void OAISource::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAISource::fromJsonObject(QJsonObject json) {

    m_component_isValid = ::OpenAPI::fromJsonValue(component, json[QString("component")]);
    m_component_isSet = !json[QString("component")].isNull() && m_component_isValid;

    m_output_isValid = ::OpenAPI::fromJsonValue(output, json[QString("output")]);
    m_output_isSet = !json[QString("output")].isNull() && m_output_isValid;
}

QString OAISource::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAISource::asJsonObject() const {
    QJsonObject obj;
    if (m_component_isSet) {
        obj.insert(QString("component"), ::OpenAPI::toJsonValue(component));
    }
    if (m_output_isSet) {
        obj.insert(QString("output"), ::OpenAPI::toJsonValue(output));
    }
    return obj;
}

qint32 OAISource::getComponent() const {
    return component;
}
void OAISource::setComponent(const qint32 &component) {
    this->component = component;
    this->m_component_isSet = true;
}

bool OAISource::is_component_Set() const{
    return m_component_isSet;
}

bool OAISource::is_component_Valid() const{
    return m_component_isValid;
}

qint32 OAISource::getOutput() const {
    return output;
}
void OAISource::setOutput(const qint32 &output) {
    this->output = output;
    this->m_output_isSet = true;
}

bool OAISource::is_output_Set() const{
    return m_output_isSet;
}

bool OAISource::is_output_Valid() const{
    return m_output_isValid;
}

bool OAISource::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_component_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_output_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAISource::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_component_isValid && m_output_isValid && true;
}

} // namespace OpenAPI
