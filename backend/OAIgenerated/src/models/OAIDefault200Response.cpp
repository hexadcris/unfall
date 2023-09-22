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

#include "OAIDefault200Response.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIDefault200Response::OAIDefault200Response(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIDefault200Response::OAIDefault200Response() {
    this->initializeModel();
}

OAIDefault200Response::~OAIDefault200Response() {}

void OAIDefault200Response::initializeModel() {

    m_response_isSet = false;
    m_response_isValid = false;
}

void OAIDefault200Response::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIDefault200Response::fromJsonObject(QJsonObject json) {

    m_response_isValid = ::OpenAPI::fromJsonValue(response, json[QString("response")]);
    m_response_isSet = !json[QString("response")].isNull() && m_response_isValid;
}

QString OAIDefault200Response::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIDefault200Response::asJsonObject() const {
    QJsonObject obj;
    if (m_response_isSet) {
        obj.insert(QString("response"), ::OpenAPI::toJsonValue(response));
    }
    return obj;
}

QString OAIDefault200Response::getResponse() const {
    return response;
}
void OAIDefault200Response::setResponse(const QString &response) {
    this->response = response;
    this->m_response_isSet = true;
}

bool OAIDefault200Response::is_response_Set() const{
    return m_response_isSet;
}

bool OAIDefault200Response::is_response_Valid() const{
    return m_response_isValid;
}

bool OAIDefault200Response::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_response_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIDefault200Response::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_response_isValid && true;
}

} // namespace OpenAPI
