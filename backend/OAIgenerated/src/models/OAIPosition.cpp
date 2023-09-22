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

#include "OAIPosition.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIPosition::OAIPosition(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIPosition::OAIPosition() {
    this->initializeModel();
}

OAIPosition::~OAIPosition() {}

void OAIPosition::initializeModel() {

    m_x_isSet = false;
    m_x_isValid = false;

    m_y_isSet = false;
    m_y_isValid = false;
}

void OAIPosition::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIPosition::fromJsonObject(QJsonObject json) {

    m_x_isValid = ::OpenAPI::fromJsonValue(x, json[QString("x")]);
    m_x_isSet = !json[QString("x")].isNull() && m_x_isValid;

    m_y_isValid = ::OpenAPI::fromJsonValue(y, json[QString("y")]);
    m_y_isSet = !json[QString("y")].isNull() && m_y_isValid;
}

QString OAIPosition::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIPosition::asJsonObject() const {
    QJsonObject obj;
    if (m_x_isSet) {
        obj.insert(QString("x"), ::OpenAPI::toJsonValue(x));
    }
    if (m_y_isSet) {
        obj.insert(QString("y"), ::OpenAPI::toJsonValue(y));
    }
    return obj;
}

qint32 OAIPosition::getX() const {
    return x;
}
void OAIPosition::setX(const qint32 &x) {
    this->x = x;
    this->m_x_isSet = true;
}

bool OAIPosition::is_x_Set() const{
    return m_x_isSet;
}

bool OAIPosition::is_x_Valid() const{
    return m_x_isValid;
}

qint32 OAIPosition::getY() const {
    return y;
}
void OAIPosition::setY(const qint32 &y) {
    this->y = y;
    this->m_y_isSet = true;
}

bool OAIPosition::is_y_Set() const{
    return m_y_isSet;
}

bool OAIPosition::is_y_Valid() const{
    return m_y_isValid;
}

bool OAIPosition::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_x_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_y_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIPosition::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_x_isValid && m_y_isValid && true;
}

} // namespace OpenAPI
