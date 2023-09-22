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

#include "OAIPathRequest.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIPathRequest::OAIPathRequest(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIPathRequest::OAIPathRequest() {
    this->initializeModel();
}

OAIPathRequest::~OAIPathRequest() {}

void OAIPathRequest::initializeModel() {

    m_path_isSet = false;
    m_path_isValid = false;
}

void OAIPathRequest::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIPathRequest::fromJsonObject(QJsonObject json) {

    m_path_isValid = ::OpenAPI::fromJsonValue(path, json[QString("path")]);
    m_path_isSet = !json[QString("path")].isNull() && m_path_isValid;
}

QString OAIPathRequest::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIPathRequest::asJsonObject() const {
    QJsonObject obj;
    if (m_path_isSet) {
        obj.insert(QString("path"), ::OpenAPI::toJsonValue(path));
    }
    return obj;
}

QString OAIPathRequest::getPath() const {
    return path;
}
void OAIPathRequest::setPath(const QString &path) {
    this->path = path;
    this->m_path_isSet = true;
}

bool OAIPathRequest::is_path_Set() const{
    return m_path_isSet;
}

bool OAIPathRequest::is_path_Valid() const{
    return m_path_isValid;
}

bool OAIPathRequest::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_path_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIPathRequest::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_path_isValid && true;
}

} // namespace OpenAPI
