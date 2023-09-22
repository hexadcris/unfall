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

#include "OAISystemUI.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAISystemUI::OAISystemUI(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAISystemUI::OAISystemUI() {
    this->initializeModel();
}

OAISystemUI::~OAISystemUI() {}

void OAISystemUI::initializeModel() {

    m_id_isSet = false;
    m_id_isValid = false;

    m_title_isSet = false;
    m_title_isValid = false;

    m_priority_isSet = false;
    m_priority_isValid = false;

    m_components_isSet = false;
    m_components_isValid = false;

    m_connections_isSet = false;
    m_connections_isValid = false;

    m_file_isSet = false;
    m_file_isValid = false;

    m_deleted_isSet = false;
    m_deleted_isValid = false;
}

void OAISystemUI::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAISystemUI::fromJsonObject(QJsonObject json) {

    m_id_isValid = ::OpenAPI::fromJsonValue(id, json[QString("id")]);
    m_id_isSet = !json[QString("id")].isNull() && m_id_isValid;

    m_title_isValid = ::OpenAPI::fromJsonValue(title, json[QString("title")]);
    m_title_isSet = !json[QString("title")].isNull() && m_title_isValid;

    m_priority_isValid = ::OpenAPI::fromJsonValue(priority, json[QString("priority")]);
    m_priority_isSet = !json[QString("priority")].isNull() && m_priority_isValid;

    m_components_isValid = ::OpenAPI::fromJsonValue(components, json[QString("components")]);
    m_components_isSet = !json[QString("components")].isNull() && m_components_isValid;

    m_connections_isValid = ::OpenAPI::fromJsonValue(connections, json[QString("connections")]);
    m_connections_isSet = !json[QString("connections")].isNull() && m_connections_isValid;

    m_file_isValid = ::OpenAPI::fromJsonValue(file, json[QString("file")]);
    m_file_isSet = !json[QString("file")].isNull() && m_file_isValid;

    m_deleted_isValid = ::OpenAPI::fromJsonValue(deleted, json[QString("deleted")]);
    m_deleted_isSet = !json[QString("deleted")].isNull() && m_deleted_isValid;
}

QString OAISystemUI::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAISystemUI::asJsonObject() const {
    QJsonObject obj;
    if (m_id_isSet) {
        obj.insert(QString("id"), ::OpenAPI::toJsonValue(id));
    }
    if (m_title_isSet) {
        obj.insert(QString("title"), ::OpenAPI::toJsonValue(title));
    }
    if (m_priority_isSet) {
        obj.insert(QString("priority"), ::OpenAPI::toJsonValue(priority));
    }
    if (components.size() > 0) {
        obj.insert(QString("components"), ::OpenAPI::toJsonValue(components));
    }
    if (connections.size() > 0) {
        obj.insert(QString("connections"), ::OpenAPI::toJsonValue(connections));
    }
    if (m_file_isSet) {
        obj.insert(QString("file"), ::OpenAPI::toJsonValue(file));
    }
    if (m_deleted_isSet) {
        obj.insert(QString("deleted"), ::OpenAPI::toJsonValue(deleted));
    }
    return obj;
}

qint32 OAISystemUI::getId() const {
    return id;
}
void OAISystemUI::setId(const qint32 &id) {
    this->id = id;
    this->m_id_isSet = true;
}

bool OAISystemUI::is_id_Set() const{
    return m_id_isSet;
}

bool OAISystemUI::is_id_Valid() const{
    return m_id_isValid;
}

QString OAISystemUI::getTitle() const {
    return title;
}
void OAISystemUI::setTitle(const QString &title) {
    this->title = title;
    this->m_title_isSet = true;
}

bool OAISystemUI::is_title_Set() const{
    return m_title_isSet;
}

bool OAISystemUI::is_title_Valid() const{
    return m_title_isValid;
}

qint32 OAISystemUI::getPriority() const {
    return priority;
}
void OAISystemUI::setPriority(const qint32 &priority) {
    this->priority = priority;
    this->m_priority_isSet = true;
}

bool OAISystemUI::is_priority_Set() const{
    return m_priority_isSet;
}

bool OAISystemUI::is_priority_Valid() const{
    return m_priority_isValid;
}

QList<OAIComponentUI> OAISystemUI::getComponents() const {
    return components;
}
void OAISystemUI::setComponents(const QList<OAIComponentUI> &components) {
    this->components = components;
    this->m_components_isSet = true;
}

bool OAISystemUI::is_components_Set() const{
    return m_components_isSet;
}

bool OAISystemUI::is_components_Valid() const{
    return m_components_isValid;
}

QList<OAIConnection> OAISystemUI::getConnections() const {
    return connections;
}
void OAISystemUI::setConnections(const QList<OAIConnection> &connections) {
    this->connections = connections;
    this->m_connections_isSet = true;
}

bool OAISystemUI::is_connections_Set() const{
    return m_connections_isSet;
}

bool OAISystemUI::is_connections_Valid() const{
    return m_connections_isValid;
}

QString OAISystemUI::getFile() const {
    return file;
}
void OAISystemUI::setFile(const QString &file) {
    this->file = file;
    this->m_file_isSet = true;
}

bool OAISystemUI::is_file_Set() const{
    return m_file_isSet;
}

bool OAISystemUI::is_file_Valid() const{
    return m_file_isValid;
}

bool OAISystemUI::isDeleted() const {
    return deleted;
}
void OAISystemUI::setDeleted(const bool &deleted) {
    this->deleted = deleted;
    this->m_deleted_isSet = true;
}

bool OAISystemUI::is_deleted_Set() const{
    return m_deleted_isSet;
}

bool OAISystemUI::is_deleted_Valid() const{
    return m_deleted_isValid;
}

bool OAISystemUI::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_id_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_title_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_priority_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (components.size() > 0) {
            isObjectUpdated = true;
            break;
        }

        if (connections.size() > 0) {
            isObjectUpdated = true;
            break;
        }

        if (m_file_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_deleted_isSet) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAISystemUI::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_id_isValid && m_title_isValid && m_priority_isValid && m_components_isValid && true;
}

} // namespace OpenAPI
