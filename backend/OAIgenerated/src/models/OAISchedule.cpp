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

#include "OAISchedule.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAISchedule::OAISchedule(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAISchedule::OAISchedule() {
    this->initializeModel();
}

OAISchedule::~OAISchedule() {}

void OAISchedule::initializeModel() {

    m_offset_isSet = false;
    m_offset_isValid = false;

    m_cycle_isSet = false;
    m_cycle_isValid = false;

    m_response_isSet = false;
    m_response_isValid = false;

    m_priority_isSet = false;
    m_priority_isValid = false;
}

void OAISchedule::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAISchedule::fromJsonObject(QJsonObject json) {

    m_offset_isValid = ::OpenAPI::fromJsonValue(offset, json[QString("offset")]);
    m_offset_isSet = !json[QString("offset")].isNull() && m_offset_isValid;

    m_cycle_isValid = ::OpenAPI::fromJsonValue(cycle, json[QString("cycle")]);
    m_cycle_isSet = !json[QString("cycle")].isNull() && m_cycle_isValid;

    m_response_isValid = ::OpenAPI::fromJsonValue(response, json[QString("response")]);
    m_response_isSet = !json[QString("response")].isNull() && m_response_isValid;

    m_priority_isValid = ::OpenAPI::fromJsonValue(priority, json[QString("priority")]);
    m_priority_isSet = !json[QString("priority")].isNull() && m_priority_isValid;
}

QString OAISchedule::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAISchedule::asJsonObject() const {
    QJsonObject obj;
    if (m_offset_isSet) {
        obj.insert(QString("offset"), ::OpenAPI::toJsonValue(offset));
    }
    if (m_cycle_isSet) {
        obj.insert(QString("cycle"), ::OpenAPI::toJsonValue(cycle));
    }
    if (m_response_isSet) {
        obj.insert(QString("response"), ::OpenAPI::toJsonValue(response));
    }
    if (m_priority_isSet) {
        obj.insert(QString("priority"), ::OpenAPI::toJsonValue(priority));
    }
    return obj;
}

qint32 OAISchedule::getOffset() const {
    return offset;
}
void OAISchedule::setOffset(const qint32 &offset) {
    this->offset = offset;
    this->m_offset_isSet = true;
}

bool OAISchedule::is_offset_Set() const{
    return m_offset_isSet;
}

bool OAISchedule::is_offset_Valid() const{
    return m_offset_isValid;
}

qint32 OAISchedule::getCycle() const {
    return cycle;
}
void OAISchedule::setCycle(const qint32 &cycle) {
    this->cycle = cycle;
    this->m_cycle_isSet = true;
}

bool OAISchedule::is_cycle_Set() const{
    return m_cycle_isSet;
}

bool OAISchedule::is_cycle_Valid() const{
    return m_cycle_isValid;
}

qint32 OAISchedule::getResponse() const {
    return response;
}
void OAISchedule::setResponse(const qint32 &response) {
    this->response = response;
    this->m_response_isSet = true;
}

bool OAISchedule::is_response_Set() const{
    return m_response_isSet;
}

bool OAISchedule::is_response_Valid() const{
    return m_response_isValid;
}

qint32 OAISchedule::getPriority() const {
    return priority;
}
void OAISchedule::setPriority(const qint32 &priority) {
    this->priority = priority;
    this->m_priority_isSet = true;
}

bool OAISchedule::is_priority_Set() const{
    return m_priority_isSet;
}

bool OAISchedule::is_priority_Valid() const{
    return m_priority_isValid;
}

bool OAISchedule::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_offset_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_cycle_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_response_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_priority_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAISchedule::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_offset_isValid && m_cycle_isValid && m_response_isValid && m_priority_isValid && true;
}

} // namespace OpenAPI
