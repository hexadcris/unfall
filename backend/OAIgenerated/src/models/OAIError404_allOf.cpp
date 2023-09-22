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

#include "OAIError404_allOf.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIError404_allOf::OAIError404_allOf(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIError404_allOf::OAIError404_allOf() {
    this->initializeModel();
}

OAIError404_allOf::~OAIError404_allOf() {}

void OAIError404_allOf::initializeModel() {

    m_message_isSet = false;
    m_message_isValid = false;
}

void OAIError404_allOf::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIError404_allOf::fromJsonObject(QJsonObject json) {

    m_message_isValid = ::OpenAPI::fromJsonValue(message, json[QString("message")]);
    m_message_isSet = !json[QString("message")].isNull() && m_message_isValid;
}

QString OAIError404_allOf::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIError404_allOf::asJsonObject() const {
    QJsonObject obj;
    if (m_message_isSet) {
        obj.insert(QString("message"), ::OpenAPI::toJsonValue(message));
    }
    return obj;
}

QString OAIError404_allOf::getMessage() const {
    return message;
}
void OAIError404_allOf::setMessage(const QString &message) {
    this->message = message;
    this->m_message_isSet = true;
}

bool OAIError404_allOf::is_message_Set() const{
    return m_message_isSet;
}

bool OAIError404_allOf::is_message_Valid() const{
    return m_message_isValid;
}

bool OAIError404_allOf::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_message_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIError404_allOf::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_message_isValid && true;
}

} // namespace OpenAPI
