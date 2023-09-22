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

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "OAIHttpFileElement.h"

namespace OpenAPI {

void OAIHttpFileElement::setMimeType(const QString &mime) {
    mime_type = mime;
}

void OAIHttpFileElement::setFileName(const QString &name) {
    local_filename = name;
}

void OAIHttpFileElement::setVariableName(const QString &name) {
    variable_name = name;
}

void OAIHttpFileElement::setRequestFileName(const QString &name) {
    request_filename = name;
}

bool OAIHttpFileElement::isSet() const {
    return !local_filename.isEmpty() || !request_filename.isEmpty();
}

QString OAIHttpFileElement::asJson() const {
    QFile file(local_filename);
    QByteArray bArray;
    bool result = false;
    if (file.exists()) {
        result = file.open(QIODevice::ReadOnly);
        bArray = file.readAll();
        file.close();
    }
    if (!result) {
        qDebug() << "Error opening file " << local_filename;
    }
    return QString(bArray);
}

QJsonValue OAIHttpFileElement::asJsonValue() const {
    QFile file(local_filename);
    QByteArray bArray;
    bool result = false;
    if (file.exists()) {
        result = file.open(QIODevice::ReadOnly);
        bArray = file.readAll();
        file.close();
    }
    if (!result) {
        qDebug() << "Error opening file " << local_filename;
    }
    return QJsonDocument::fromBinaryData(bArray.data()).object();
}

bool OAIHttpFileElement::fromStringValue(const QString &instr) {
    QFile file(local_filename);
    bool result = false;
    if (file.exists()) {
        file.remove();
    }
    result = file.open(QIODevice::WriteOnly);
    file.write(instr.toUtf8());
    file.close();
    if (!result) {
        qDebug() << "Error creating file " << local_filename;
    }
    return result;
}

bool OAIHttpFileElement::fromJsonValue(const QJsonValue &jval) {
    QFile file(local_filename);
    bool result = false;
    if (file.exists()) {
        file.remove();
    }
    result = file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(jval.toObject()).toBinaryData());
    file.close();
    if (!result) {
        qDebug() << "Error creating file " << local_filename;
    }
    return result;
}

QByteArray OAIHttpFileElement::asByteArray() const {
    QFile file(local_filename);
    QByteArray bArray;
    bool result = false;
    if (file.exists()) {
        result = file.open(QIODevice::ReadOnly);
        bArray = file.readAll();
        file.close();
    }
    if (!result) {
        qDebug() << "Error opening file " << local_filename;
    }
    return bArray;
}

bool OAIHttpFileElement::fromByteArray(const QByteArray &bytes) {
    QFile file(local_filename);
    bool result = false;
    if (file.exists()) {
        file.remove();
    }
    result = file.open(QIODevice::WriteOnly);
    file.write(bytes);
    file.close();
    if (!result) {
        qDebug() << "Error creating file " << local_filename;
    }
    return result;
}

bool OAIHttpFileElement::saveToFile(const QString &varName, const QString &localFName, const QString &reqFname, const QString &mime, const QByteArray &bytes) {
    setMimeType(mime);
    setFileName(localFName);
    setVariableName(varName);
    setRequestFileName(reqFname);
    return fromByteArray(bytes);
}

QByteArray OAIHttpFileElement::loadFromFile(const QString &varName, const QString &localFName, const QString &reqFname, const QString &mime) {
    setMimeType(mime);
    setFileName(localFName);
    setVariableName(varName);
    setRequestFileName(reqFname);
    return asByteArray();
}

} // namespace OpenAPI
