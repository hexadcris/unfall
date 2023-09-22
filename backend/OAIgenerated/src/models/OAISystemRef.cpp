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

#include "OAISystemRef.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAISystemRef::OAISystemRef(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAISystemRef::OAISystemRef() {
    this->initializeModel();
}

OAISystemRef::~OAISystemRef() {}

void OAISystemRef::initializeModel() {

    m_name_isSet = false;
    m_name_isValid = false;

    m_file_isSet = false;
    m_file_isValid = false;
}

void OAISystemRef::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAISystemRef::fromJsonObject(QJsonObject json) {

    m_name_isValid = ::OpenAPI::fromJsonValue(name, json[QString("name")]);
    m_name_isSet = !json[QString("name")].isNull() && m_name_isValid;

    m_file_isValid = ::OpenAPI::fromJsonValue(file, json[QString("file")]);
    m_file_isSet = !json[QString("file")].isNull() && m_file_isValid;
}

QString OAISystemRef::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAISystemRef::asJsonObject() const {
    QJsonObject obj;
    if (m_name_isSet) {
        obj.insert(QString("name"), ::OpenAPI::toJsonValue(name));
    }
    if (m_file_isSet) {
        obj.insert(QString("file"), ::OpenAPI::toJsonValue(file));
    }
    return obj;
}

QString OAISystemRef::getName() const {
    return name;
}
void OAISystemRef::setName(const QString &name) {
    this->name = name;
    this->m_name_isSet = true;
}

bool OAISystemRef::is_name_Set() const{
    return m_name_isSet;
}

bool OAISystemRef::is_name_Valid() const{
    return m_name_isValid;
}

QString OAISystemRef::getFile() const {
    return file;
}
void OAISystemRef::setFile(const QString &file) {
    this->file = file;
    this->m_file_isSet = true;
}

bool OAISystemRef::is_file_Set() const{
    return m_file_isSet;
}

bool OAISystemRef::is_file_Valid() const{
    return m_file_isValid;
}

bool OAISystemRef::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_name_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_file_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAISystemRef::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_name_isValid && m_file_isValid && true;
}

} // namespace OpenAPI
