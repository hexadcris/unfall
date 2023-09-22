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

#include "OAIError400_allOf.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIError400_allOf::OAIError400_allOf(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIError400_allOf::OAIError400_allOf() {
    this->initializeModel();
}

OAIError400_allOf::~OAIError400_allOf() {}

void OAIError400_allOf::initializeModel() {

    m_message_isSet = false;
    m_message_isValid = false;
}

void OAIError400_allOf::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIError400_allOf::fromJsonObject(QJsonObject json) {

    m_message_isValid = ::OpenAPI::fromJsonValue(message, json[QString("message")]);
    m_message_isSet = !json[QString("message")].isNull() && m_message_isValid;
}

QString OAIError400_allOf::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIError400_allOf::asJsonObject() const {
    QJsonObject obj;
    if (m_message_isSet) {
        obj.insert(QString("message"), ::OpenAPI::toJsonValue(message));
    }
    return obj;
}

QString OAIError400_allOf::getMessage() const {
    return message;
}
void OAIError400_allOf::setMessage(const QString &message) {
    this->message = message;
    this->m_message_isSet = true;
}

bool OAIError400_allOf::is_message_Set() const{
    return m_message_isSet;
}

bool OAIError400_allOf::is_message_Valid() const{
    return m_message_isValid;
}

bool OAIError400_allOf::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_message_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIError400_allOf::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_message_isValid && true;
}

} // namespace OpenAPI
