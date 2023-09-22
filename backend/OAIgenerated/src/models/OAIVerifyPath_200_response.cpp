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

#include "OAIVerifyPath_200_response.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIVerifyPath_200_response::OAIVerifyPath_200_response(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIVerifyPath_200_response::OAIVerifyPath_200_response() {
    this->initializeModel();
}

OAIVerifyPath_200_response::~OAIVerifyPath_200_response() {}

void OAIVerifyPath_200_response::initializeModel() {

    m_ok_isSet = false;
    m_ok_isValid = false;

    m_real_path_isSet = false;
    m_real_path_isValid = false;

    m_empty_isSet = false;
    m_empty_isValid = false;
}

void OAIVerifyPath_200_response::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIVerifyPath_200_response::fromJsonObject(QJsonObject json) {

    m_ok_isValid = ::OpenAPI::fromJsonValue(ok, json[QString("ok")]);
    m_ok_isSet = !json[QString("ok")].isNull() && m_ok_isValid;

    m_real_path_isValid = ::OpenAPI::fromJsonValue(real_path, json[QString("realPath")]);
    m_real_path_isSet = !json[QString("realPath")].isNull() && m_real_path_isValid;

    m_empty_isValid = ::OpenAPI::fromJsonValue(empty, json[QString("empty")]);
    m_empty_isSet = !json[QString("empty")].isNull() && m_empty_isValid;
}

QString OAIVerifyPath_200_response::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIVerifyPath_200_response::asJsonObject() const {
    QJsonObject obj;
    if (m_ok_isSet) {
        obj.insert(QString("ok"), ::OpenAPI::toJsonValue(ok));
    }
    if (m_real_path_isSet) {
        obj.insert(QString("realPath"), ::OpenAPI::toJsonValue(real_path));
    }
    if (m_empty_isSet) {
        obj.insert(QString("empty"), ::OpenAPI::toJsonValue(empty));
    }
    return obj;
}

bool OAIVerifyPath_200_response::isOk() const {
    return ok;
}
void OAIVerifyPath_200_response::setOk(const bool &ok) {
    this->ok = ok;
    this->m_ok_isSet = true;
}

bool OAIVerifyPath_200_response::is_ok_Set() const{
    return m_ok_isSet;
}

bool OAIVerifyPath_200_response::is_ok_Valid() const{
    return m_ok_isValid;
}

QString OAIVerifyPath_200_response::getRealPath() const {
    return real_path;
}
void OAIVerifyPath_200_response::setRealPath(const QString &real_path) {
    this->real_path = real_path;
    this->m_real_path_isSet = true;
}

bool OAIVerifyPath_200_response::is_real_path_Set() const{
    return m_real_path_isSet;
}

bool OAIVerifyPath_200_response::is_real_path_Valid() const{
    return m_real_path_isValid;
}

bool OAIVerifyPath_200_response::isEmpty() const {
    return empty;
}
void OAIVerifyPath_200_response::setEmpty(const bool &empty) {
    this->empty = empty;
    this->m_empty_isSet = true;
}

bool OAIVerifyPath_200_response::is_empty_Set() const{
    return m_empty_isSet;
}

bool OAIVerifyPath_200_response::is_empty_Valid() const{
    return m_empty_isValid;
}

bool OAIVerifyPath_200_response::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_ok_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_real_path_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_empty_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIVerifyPath_200_response::isValid() const {
    // only required properties are required for the object to be considered valid
    return true;
}

} // namespace OpenAPI
