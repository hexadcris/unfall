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

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
#include <QDir>
#include <QJsonValue>


namespace TestHelpers {

QString readFile(const QString &filePath);
QString removeSpacesBetweenTags(const QString& input);
bool createAndCheckFile(const QString& filenameWithPath, const QString& content);
QDomElement parseXMLString(const QString& xmlString);
QString joinPaths(const QString& path1, const QString& path2);
QString removeXmlElement(const QString& xmlStr, const QString& tagNameToRemove, const QString& parentTagName = QString());
QString replaceXmlElementContent(const QString& xmlStr, const QString& tagNameToReplace, const QString& newContent, const QString& parentTagName = QString());
QString removeXmlComments(const QString &xmlString);
QString removeJsonElement(const QString& jsonStr, const QString& keyToRemove);
QJsonValue recursiveRemove(const QJsonValue &value, const QString &keyToRemove);
QJsonValue recursiveEmptyArray(const QJsonValue &value, const QString &keyToEmptyArray);
QString emptyJsonArray(const QString& jsonStr, const QString& keyToEmptyArray);
} 

#endif // TEST_HELPERS_H