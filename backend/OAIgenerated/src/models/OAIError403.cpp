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

#include "OAIError403.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIError403::OAIError403(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIError403::OAIError403() {
    this->initializeModel();
}

OAIError403::~OAIError403() {}

void OAIError403::initializeModel() {

    m_code_isSet = false;
    m_code_isValid = false;

    m_message_isSet = false;
    m_message_isValid = false;
}

void OAIError403::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIError403::fromJsonObject(QJsonObject json) {

    m_code_isValid = ::OpenAPI::fromJsonValue(code, json[QString("code")]);
    m_code_isSet = !json[QString("code")].isNull() && m_code_isValid;

    m_message_isValid = ::OpenAPI::fromJsonValue(message, json[QString("message")]);
    m_message_isSet = !json[QString("message")].isNull() && m_message_isValid;
}

QString OAIError403::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIError403::asJsonObject() const {
    QJsonObject obj;
    if (m_code_isSet) {
        obj.insert(QString("code"), ::OpenAPI::toJsonValue(code));
    }
    if (m_message_isSet) {
        obj.insert(QString("message"), ::OpenAPI::toJsonValue(message));
    }
    return obj;
}

qint32 OAIError403::getCode() const {
    return code;
}
void OAIError403::setCode(const qint32 &code) {
    this->code = code;
    this->m_code_isSet = true;
}

bool OAIError403::is_code_Set() const{
    return m_code_isSet;
}

bool OAIError403::is_code_Valid() const{
    return m_code_isValid;
}

QString OAIError403::getMessage() const {
    return message;
}
void OAIError403::setMessage(const QString &message) {
    this->message = message;
    this->m_message_isSet = true;
}

bool OAIError403::is_message_Set() const{
    return m_message_isSet;
}

bool OAIError403::is_message_Valid() const{
    return m_message_isValid;
}

bool OAIError403::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_code_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_message_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIError403::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_code_isValid && m_message_isValid && true;
}

} // namespace OpenAPI
