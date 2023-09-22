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

#include "OAIComponentUI.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

#include "OAIHelpers.h"

namespace OpenAPI {

OAIComponentUI::OAIComponentUI(QString json) {
    this->initializeModel();
    this->fromJson(json);
}

OAIComponentUI::OAIComponentUI() {
    this->initializeModel();
}

OAIComponentUI::~OAIComponentUI() {}

void OAIComponentUI::initializeModel() {

    m_id_isSet = false;
    m_id_isValid = false;

    m_type_isSet = false;
    m_type_isValid = false;

    m_library_isSet = false;
    m_library_isValid = false;

    m_title_isSet = false;
    m_title_isValid = false;

    m_schedule_isSet = false;
    m_schedule_isValid = false;

    m_parameters_isSet = false;
    m_parameters_isValid = false;

    m_inputs_isSet = false;
    m_inputs_isValid = false;

    m_outputs_isSet = false;
    m_outputs_isValid = false;

    m_position_isSet = false;
    m_position_isValid = false;
}

void OAIComponentUI::fromJson(QString jsonString) {
    QByteArray array(jsonString.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
}

void OAIComponentUI::fromJsonObject(QJsonObject json) {

    m_id_isValid = ::OpenAPI::fromJsonValue(id, json[QString("id")]);
    m_id_isSet = !json[QString("id")].isNull() && m_id_isValid;

    m_type_isValid = ::OpenAPI::fromJsonValue(type, json[QString("type")]);
    m_type_isSet = !json[QString("type")].isNull() && m_type_isValid;

    m_library_isValid = ::OpenAPI::fromJsonValue(library, json[QString("library")]);
    m_library_isSet = !json[QString("library")].isNull() && m_library_isValid;

    m_title_isValid = ::OpenAPI::fromJsonValue(title, json[QString("title")]);
    m_title_isSet = !json[QString("title")].isNull() && m_title_isValid;

    m_schedule_isValid = ::OpenAPI::fromJsonValue(schedule, json[QString("schedule")]);
    m_schedule_isSet = !json[QString("schedule")].isNull() && m_schedule_isValid;

    m_parameters_isValid = ::OpenAPI::fromJsonValue(parameters, json[QString("parameters")]);
    m_parameters_isSet = !json[QString("parameters")].isNull() && m_parameters_isValid;

    m_inputs_isValid = ::OpenAPI::fromJsonValue(inputs, json[QString("inputs")]);
    m_inputs_isSet = !json[QString("inputs")].isNull() && m_inputs_isValid;

    m_outputs_isValid = ::OpenAPI::fromJsonValue(outputs, json[QString("outputs")]);
    m_outputs_isSet = !json[QString("outputs")].isNull() && m_outputs_isValid;

    m_position_isValid = ::OpenAPI::fromJsonValue(position, json[QString("position")]);
    m_position_isSet = !json[QString("position")].isNull() && m_position_isValid;
}

QString OAIComponentUI::asJson() const {
    QJsonObject obj = this->asJsonObject();
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    return QString(bytes);
}

QJsonObject OAIComponentUI::asJsonObject() const {
    QJsonObject obj;
    if (m_id_isSet) {
        obj.insert(QString("id"), ::OpenAPI::toJsonValue(id));
    }
    if (m_type_isSet) {
        obj.insert(QString("type"), ::OpenAPI::toJsonValue(type));
    }
    if (m_library_isSet) {
        obj.insert(QString("library"), ::OpenAPI::toJsonValue(library));
    }
    if (m_title_isSet) {
        obj.insert(QString("title"), ::OpenAPI::toJsonValue(title));
    }
    if (schedule.isSet()) {
        obj.insert(QString("schedule"), ::OpenAPI::toJsonValue(schedule));
    }
    if (parameters.size() > 0) {
        obj.insert(QString("parameters"), ::OpenAPI::toJsonValue(parameters));
    }
    if (inputs.size() > 0) {
        obj.insert(QString("inputs"), ::OpenAPI::toJsonValue(inputs));
    }
    if (outputs.size() > 0) {
        obj.insert(QString("outputs"), ::OpenAPI::toJsonValue(outputs));
    }
    if (position.isSet()) {
        obj.insert(QString("position"), ::OpenAPI::toJsonValue(position));
    }
    return obj;
}

qint32 OAIComponentUI::getId() const {
    return id;
}
void OAIComponentUI::setId(const qint32 &id) {
    this->id = id;
    this->m_id_isSet = true;
}

bool OAIComponentUI::is_id_Set() const{
    return m_id_isSet;
}

bool OAIComponentUI::is_id_Valid() const{
    return m_id_isValid;
}

QString OAIComponentUI::getType() const {
    return type;
}
void OAIComponentUI::setType(const QString &type) {
    this->type = type;
    this->m_type_isSet = true;
}

bool OAIComponentUI::is_type_Set() const{
    return m_type_isSet;
}

bool OAIComponentUI::is_type_Valid() const{
    return m_type_isValid;
}

QString OAIComponentUI::getLibrary() const {
    return library;
}
void OAIComponentUI::setLibrary(const QString &library) {
    this->library = library;
    this->m_library_isSet = true;
}

bool OAIComponentUI::is_library_Set() const{
    return m_library_isSet;
}

bool OAIComponentUI::is_library_Valid() const{
    return m_library_isValid;
}

QString OAIComponentUI::getTitle() const {
    return title;
}
void OAIComponentUI::setTitle(const QString &title) {
    this->title = title;
    this->m_title_isSet = true;
}

bool OAIComponentUI::is_title_Set() const{
    return m_title_isSet;
}

bool OAIComponentUI::is_title_Valid() const{
    return m_title_isValid;
}

OAISchedule OAIComponentUI::getSchedule() const {
    return schedule;
}
void OAIComponentUI::setSchedule(const OAISchedule &schedule) {
    this->schedule = schedule;
    this->m_schedule_isSet = true;
}

bool OAIComponentUI::is_schedule_Set() const{
    return m_schedule_isSet;
}

bool OAIComponentUI::is_schedule_Valid() const{
    return m_schedule_isValid;
}

QList<OAIParameter> OAIComponentUI::getParameters() const {
    return parameters;
}
void OAIComponentUI::setParameters(const QList<OAIParameter> &parameters) {
    this->parameters = parameters;
    this->m_parameters_isSet = true;
}

bool OAIComponentUI::is_parameters_Set() const{
    return m_parameters_isSet;
}

bool OAIComponentUI::is_parameters_Valid() const{
    return m_parameters_isValid;
}

QList<OAIInput> OAIComponentUI::getInputs() const {
    return inputs;
}
void OAIComponentUI::setInputs(const QList<OAIInput> &inputs) {
    this->inputs = inputs;
    this->m_inputs_isSet = true;
}

bool OAIComponentUI::is_inputs_Set() const{
    return m_inputs_isSet;
}

bool OAIComponentUI::is_inputs_Valid() const{
    return m_inputs_isValid;
}

QList<OAIOutput> OAIComponentUI::getOutputs() const {
    return outputs;
}
void OAIComponentUI::setOutputs(const QList<OAIOutput> &outputs) {
    this->outputs = outputs;
    this->m_outputs_isSet = true;
}

bool OAIComponentUI::is_outputs_Set() const{
    return m_outputs_isSet;
}

bool OAIComponentUI::is_outputs_Valid() const{
    return m_outputs_isValid;
}

OAIPosition OAIComponentUI::getPosition() const {
    return position;
}
void OAIComponentUI::setPosition(const OAIPosition &position) {
    this->position = position;
    this->m_position_isSet = true;
}

bool OAIComponentUI::is_position_Set() const{
    return m_position_isSet;
}

bool OAIComponentUI::is_position_Valid() const{
    return m_position_isValid;
}

bool OAIComponentUI::isSet() const {
    bool isObjectUpdated = false;
    do {
        if (m_id_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_type_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_library_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (m_title_isSet) {
            isObjectUpdated = true;
            break;
        }

        if (schedule.isSet()) {
            isObjectUpdated = true;
            break;
        }

        if (parameters.size() > 0) {
            isObjectUpdated = true;
            break;
        }

        if (inputs.size() > 0) {
            isObjectUpdated = true;
            break;
        }

        if (outputs.size() > 0) {
            isObjectUpdated = true;
            break;
        }

        if (position.isSet()) {
            isObjectUpdated = true;
            break;
        }
    } while (false);
    return isObjectUpdated;
}

bool OAIComponentUI::isValid() const {
    // only required properties are required for the object to be considered valid
    return m_id_isValid && m_type_isValid && m_library_isValid && m_title_isValid && m_schedule_isValid && true;
}

} // namespace OpenAPI
