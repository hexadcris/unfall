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

#include "OAIConnection.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIConnection::OAIConnection(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIConnection::OAIConnection() {
    this->initializeModel();
}

OAIConnection::~OAIConnection() {}

void OAIConnection::initializeModel() {

    m_id_isSet = false;
    m_id_isValid = false;

    m_source_isSet = false;
    m_source_isValid = false;

    m_target_isSet = false;
    m_target_isValid = false;
}

void OAIConnection::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIConnection::fromJsonObject(QJsonObject json) {

    m_id_isValid = ::OpenAPI::fromJsonValue(id, json[QString("id")]);
    m_id_isSet = !json[QString("id")].isNull() && m_id_isValid;

    m_source_isValid = ::OpenAPI::fromJsonValue(source, json[QString("source")]);
    m_source_isSet = !json[QString("source")].isNull() && m_source_isValid;

    m_target_isValid = ::OpenAPI::fromJsonValue(target, json[QString("target")]);
    m_target_isSet = !json[QString("target")].isNull() && m_target_isValid;
}

QString OAIConnection::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIConnection::asJsonObject() const {
    QJsonObject obj;
    if (m_id_isSet) {
        obj.insert(QString("id"), ::OpenAPI::toJsonValue(id));
    }
    if (source.isSet()) {
        obj.insert(QString("source"), ::OpenAPI::toJsonValue(source));
    }
    if (target.isSet()) {
        obj.insert(QString("target"), ::OpenAPI::toJsonValue(target));
    }
    return obj;
}

qint32 OAIConnection::getId() const {
    return id;
}
void OAIConnection::setId(const qint32 &id) {
    this->id = id;
    this->m_id_isSet = true;
}

bool OAIConnection::is_id_Set() const{
    return m_id_isSet;
}

bool OAIConnection::is_id_Valid() const{
    return m_id_isValid;
}

OAISource OAIConnection::getSource() const {
    return source;
}
void OAIConnection::setSource(const OAISource &source) {
    this->source = source;
    this->m_source_isSet = true;
}

bool OAIConnection::is_source_Set() const{
    return m_source_isSet;
}

bool OAIConnection::is_source_Valid() const{
    return m_source_isValid;
}

OAITarget OAIConnection::getTarget() const {
    return target;
}
void OAIConnection::setTarget(const OAITarget &target) {
    this->target = target;
    this->m_target_isSet = true;
}

bool OAIConnection::is_target_Set() const{
    return m_target_isSet;
}

bool OAIConnection::is_target_Valid() const{
    return m_target_isValid;
}

bool OAIConnection::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_id_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (source.isSet()) {
            isObjectUpdated = true;
            break;
        }

        if (target.isSet()) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIConnection::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_id_isValid && m_source_isValid && m_target_isValid && true;
}

} // namespace OpenAPI
