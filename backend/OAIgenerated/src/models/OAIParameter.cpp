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

#include "OAIParameter.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIParameter::OAIParameter(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIParameter::OAIParameter() {
    this->initializeModel();
}

OAIParameter::~OAIParameter() {}

void OAIParameter::initializeModel() {

    m_id_isSet = false;
    m_id_isValid = false;

    m_type_isSet = false;
    m_type_isValid = false;

    m_title_isSet = false;
    m_title_isValid = false;

    m_unit_isSet = false;
    m_unit_isValid = false;

    m_value_isSet = false;
    m_value_isValid = false;
}

void OAIParameter::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIParameter::fromJsonObject(QJsonObject json) {

    m_id_isValid = ::OpenAPI::fromJsonValue(id, json[QString("id")]);
    m_id_isSet = !json[QString("id")].isNull() && m_id_isValid;

    m_type_isValid = ::OpenAPI::fromJsonValue(type, json[QString("type")]);
    m_type_isSet = !json[QString("type")].isNull() && m_type_isValid;

    m_title_isValid = ::OpenAPI::fromJsonValue(title, json[QString("title")]);
    m_title_isSet = !json[QString("title")].isNull() && m_title_isValid;

    m_unit_isValid = ::OpenAPI::fromJsonValue(unit, json[QString("unit")]);
    m_unit_isSet = !json[QString("unit")].isNull() && m_unit_isValid;

    m_value_isValid = ::OpenAPI::fromJsonValue(value, json[QString("value")]);
    m_value_isSet = !json[QString("value")].isNull() && m_value_isValid;
}

QString OAIParameter::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIParameter::asJsonObject() const {
    QJsonObject obj;
    if (m_id_isSet) {
        obj.insert(QString("id"), ::OpenAPI::toJsonValue(id));
    }
    if (m_type_isSet) {
        obj.insert(QString("type"), ::OpenAPI::toJsonValue(type));
    }
    if (m_title_isSet) {
        obj.insert(QString("title"), ::OpenAPI::toJsonValue(title));
    }
    if (m_unit_isSet) {
        obj.insert(QString("unit"), ::OpenAPI::toJsonValue(unit));
    }
    if (m_value_isSet) {
        obj.insert(QString("value"), ::OpenAPI::toJsonValue(value));
    }
    return obj;
}

qint32 OAIParameter::getId() const {
    return id;
}
void OAIParameter::setId(const qint32 &id) {
    this->id = id;
    this->m_id_isSet = true;
}

bool OAIParameter::is_id_Set() const{
    return m_id_isSet;
}

bool OAIParameter::is_id_Valid() const{
    return m_id_isValid;
}

QString OAIParameter::getType() const {
    return type;
}
void OAIParameter::setType(const QString &type) {
    this->type = type;
    this->m_type_isSet = true;
}

bool OAIParameter::is_type_Set() const{
    return m_type_isSet;
}

bool OAIParameter::is_type_Valid() const{
    return m_type_isValid;
}

QString OAIParameter::getTitle() const {
    return title;
}
void OAIParameter::setTitle(const QString &title) {
    this->title = title;
    this->m_title_isSet = true;
}

bool OAIParameter::is_title_Set() const{
    return m_title_isSet;
}

bool OAIParameter::is_title_Valid() const{
    return m_title_isValid;
}

QString OAIParameter::getUnit() const {
    return unit;
}
void OAIParameter::setUnit(const QString &unit) {
    this->unit = unit;
    this->m_unit_isSet = true;
}

bool OAIParameter::is_unit_Set() const{
    return m_unit_isSet;
}

bool OAIParameter::is_unit_Valid() const{
    return m_unit_isValid;
}

QString OAIParameter::getValue() const {
    return value;
}
void OAIParameter::setValue(const QString &value) {
    this->value = value;
    this->m_value_isSet = true;
}

bool OAIParameter::is_value_Set() const{
    return m_value_isSet;
}

bool OAIParameter::is_value_Valid() const{
    return m_value_isValid;
}

bool OAIParameter::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_id_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_type_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_title_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_unit_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_value_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIParameter::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_id_isValid && m_type_isValid && m_title_isValid && m_unit_isValid && m_value_isValid && true;
}

} // namespace OpenAPI
