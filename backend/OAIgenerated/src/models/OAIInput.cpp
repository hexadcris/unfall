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

#include "OAIInput.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIInput::OAIInput(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIInput::OAIInput() {
    this->initializeModel();
}

OAIInput::~OAIInput() {}

void OAIInput::initializeModel() {

    m_id_isSet = false;
    m_id_isValid = false;

    m_type_isSet = false;
    m_type_isValid = false;

    m_title_isSet = false;
    m_title_isValid = false;

    m_unit_isSet = false;
    m_unit_isValid = false;

    m_cardinality_isSet = false;
    m_cardinality_isValid = false;
}

void OAIInput::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIInput::fromJsonObject(QJsonObject json) {

    m_id_isValid = ::OpenAPI::fromJsonValue(id, json[QString("id")]);
    m_id_isSet = !json[QString("id")].isNull() && m_id_isValid;

    m_type_isValid = ::OpenAPI::fromJsonValue(type, json[QString("type")]);
    m_type_isSet = !json[QString("type")].isNull() && m_type_isValid;

    m_title_isValid = ::OpenAPI::fromJsonValue(title, json[QString("title")]);
    m_title_isSet = !json[QString("title")].isNull() && m_title_isValid;

    m_unit_isValid = ::OpenAPI::fromJsonValue(unit, json[QString("unit")]);
    m_unit_isSet = !json[QString("unit")].isNull() && m_unit_isValid;

    m_cardinality_isValid = ::OpenAPI::fromJsonValue(cardinality, json[QString("cardinality")]);
    m_cardinality_isSet = !json[QString("cardinality")].isNull() && m_cardinality_isValid;
}

QString OAIInput::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIInput::asJsonObject() const {
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
    if (m_cardinality_isSet) {
        obj.insert(QString("cardinality"), ::OpenAPI::toJsonValue(cardinality));
    }
    return obj;
}

qint32 OAIInput::getId() const {
    return id;
}
void OAIInput::setId(const qint32 &id) {
    this->id = id;
    this->m_id_isSet = true;
}

bool OAIInput::is_id_Set() const{
    return m_id_isSet;
}

bool OAIInput::is_id_Valid() const{
    return m_id_isValid;
}

QString OAIInput::getType() const {
    return type;
}
void OAIInput::setType(const QString &type) {
    this->type = type;
    this->m_type_isSet = true;
}

bool OAIInput::is_type_Set() const{
    return m_type_isSet;
}

bool OAIInput::is_type_Valid() const{
    return m_type_isValid;
}

QString OAIInput::getTitle() const {
    return title;
}
void OAIInput::setTitle(const QString &title) {
    this->title = title;
    this->m_title_isSet = true;
}

bool OAIInput::is_title_Set() const{
    return m_title_isSet;
}

bool OAIInput::is_title_Valid() const{
    return m_title_isValid;
}

QString OAIInput::getUnit() const {
    return unit;
}
void OAIInput::setUnit(const QString &unit) {
    this->unit = unit;
    this->m_unit_isSet = true;
}

bool OAIInput::is_unit_Set() const{
    return m_unit_isSet;
}

bool OAIInput::is_unit_Valid() const{
    return m_unit_isValid;
}

qint32 OAIInput::getCardinality() const {
    return cardinality;
}
void OAIInput::setCardinality(const qint32 &cardinality) {
    this->cardinality = cardinality;
    this->m_cardinality_isSet = true;
}

bool OAIInput::is_cardinality_Set() const{
    return m_cardinality_isSet;
}

bool OAIInput::is_cardinality_Valid() const{
    return m_cardinality_isValid;
}

bool OAIInput::isSet() const {
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

        if (m_cardinality_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIInput::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_id_isValid && m_type_isValid && m_title_isValid && m_unit_isValid && m_cardinality_isValid && true;
}

} // namespace OpenAPI
