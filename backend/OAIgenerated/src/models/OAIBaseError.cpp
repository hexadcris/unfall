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

#include "OAIBaseError.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIBaseError::OAIBaseError(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIBaseError::OAIBaseError() {
    this->initializeModel();
}

OAIBaseError::~OAIBaseError() {}

void OAIBaseError::initializeModel() {

    m_code_isSet = false;
    m_code_isValid = false;
}

void OAIBaseError::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIBaseError::fromJsonObject(QJsonObject json) {

    m_code_isValid = ::OpenAPI::fromJsonValue(code, json[QString("code")]);
    m_code_isSet = !json[QString("code")].isNull() && m_code_isValid;
}

QString OAIBaseError::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIBaseError::asJsonObject() const {
    QJsonObject obj;
    if (m_code_isSet) {
        obj.insert(QString("code"), ::OpenAPI::toJsonValue(code));
    }
    return obj;
}

qint32 OAIBaseError::getCode() const {
    return code;
}
void OAIBaseError::setCode(const qint32 &code) {
    this->code = code;
    this->m_code_isSet = true;
}

bool OAIBaseError::is_code_Set() const{
    return m_code_isSet;
}

bool OAIBaseError::is_code_Valid() const{
    return m_code_isValid;
}

bool OAIBaseError::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_code_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIBaseError::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_code_isValid && true;
}

} // namespace OpenAPI
