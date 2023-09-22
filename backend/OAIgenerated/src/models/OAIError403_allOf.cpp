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

#include "OAIError403_allOf.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIError403_allOf::OAIError403_allOf(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIError403_allOf::OAIError403_allOf() {
    this->initializeModel();
}

OAIError403_allOf::~OAIError403_allOf() {}

void OAIError403_allOf::initializeModel() {

    m_message_isSet = false;
    m_message_isValid = false;
}

void OAIError403_allOf::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIError403_allOf::fromJsonObject(QJsonObject json) {

    m_message_isValid = ::OpenAPI::fromJsonValue(message, json[QString("message")]);
    m_message_isSet = !json[QString("message")].isNull() && m_message_isValid;
}

QString OAIError403_allOf::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIError403_allOf::asJsonObject() const {
    QJsonObject obj;
    if (m_message_isSet) {
        obj.insert(QString("message"), ::OpenAPI::toJsonValue(message));
    }
    return obj;
}

QString OAIError403_allOf::getMessage() const {
    return message;
}
void OAIError403_allOf::setMessage(const QString &message) {
    this->message = message;
    this->m_message_isSet = true;
}

bool OAIError403_allOf::is_message_Set() const{
    return m_message_isSet;
}

bool OAIError403_allOf::is_message_Valid() const{
    return m_message_isValid;
}

bool OAIError403_allOf::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_message_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIError403_allOf::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_message_isValid && true;
}

} // namespace OpenAPI
