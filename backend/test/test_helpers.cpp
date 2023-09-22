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


#include <QString>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValueRef>

#include "test_helpers.h"

namespace TestHelpers {

QString readFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QTextStream in(&file);
    return in.readAll();
}

QString removeSpacesBetweenTags(const QString& input) {
    QRegularExpression pattern(">(\\s+)<");
    QString output = input;
    output.replace(pattern, "><");
    return output;
}

/*bool createAndCheckFile(const QString& filenameWithPath, const QString& content) {
    QFile file(filenameWithPath);

    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream stream(&file);
    stream << content;
    file.close();

    QFileInfo fileInfo(file);
    return fileInfo.exists() && fileInfo.isReadable();
}*/

bool createAndCheckFile(const QString& filePath, const QString& content) {
    QFile file(filePath);

    // Open the file with WriteOnly and Truncate flags to replace the content
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false; // Failed to open the file
    }

    QTextStream outStream(&file);
    outStream << content;

    file.close();
    return true; // Successfully wrote to the file
}

QDomElement parseXMLString(const QString& xmlString) {
    QDomDocument doc;
    doc.setContent(xmlString);
    return doc.documentElement();
}

QString joinPaths(const QString& path1, const QString& path2) {
    QDir dir(path1);
    return QDir::cleanPath(path1 + QStringLiteral("/") + path2);
}

QString removeXmlElement(const QString& xmlStr, const QString& tagNameToRemove, const QString& parentTagName) {
    QDomDocument doc;
    doc.setContent(xmlStr);

    QDomNodeList elements;
    if (parentTagName.isEmpty()) {
        elements = doc.elementsByTagName(tagNameToRemove);
    } else {
        QDomNodeList parentElements = doc.elementsByTagName(parentTagName);
        for (int i = 0; i < parentElements.count(); ++i) {
            QDomNode parentElement = parentElements.at(i);
            elements = parentElement.toElement().elementsByTagName(tagNameToRemove);
        }
    }

    for (int i = 0; i < elements.count(); ++i) {
        QDomNode parent = elements.at(i).parentNode();
        parent.removeChild(elements.at(i));
    }

    QString modifiedXml = doc.toString();
    return modifiedXml;
}

QString replaceXmlElementContent(const QString& xmlStr, const QString& tagNameToReplace, const QString& newContent, const QString& parentTagName) {
    QDomDocument doc;
    doc.setContent(xmlStr);

    QDomNodeList elements;
    if (parentTagName.isEmpty()) {
        elements = doc.elementsByTagName(tagNameToReplace);
    } else {
        QDomNodeList parentElements = doc.elementsByTagName(parentTagName);
        for (int i = 0; i < parentElements.count(); ++i) {
            QDomNode parentElement = parentElements.at(i);
            elements = parentElement.toElement().elementsByTagName(tagNameToReplace);
        }
    }

    for (int i = 0; i < elements.count(); ++i) {
        QDomNode element = elements.at(i);
        QDomNode newNode = doc.createTextNode(newContent);  
        element.replaceChild(newNode, element.firstChild()); 
    }

    QString modifiedXml = doc.toString();
    return modifiedXml;
}

QString removeXmlComments(const QString &xmlString) {
    QRegularExpression commentPattern("<!--.*?-->");
    QString cleanedXml = xmlString;
    cleanedXml.replace(commentPattern, "");
    return cleanedXml;
}

QJsonValue recursiveRemove(const QJsonValue &value, const QString &keyToRemove) {
    if (value.isObject()) {
        QJsonObject object = value.toObject();
        object.remove(keyToRemove);
        for (auto key : object.keys()) {
            object[key] = recursiveRemove(object[key], keyToRemove);
        }
        return object;
    } else if (value.isArray()) {
        QJsonArray array = value.toArray();
        for (int i = 0; i < array.size(); ++i) {
            array[i] = recursiveRemove(array[i], keyToRemove);
        }
        return array;
    } else {
        return value;
    }
}

QString removeJsonElement(const QString& jsonStr, const QString& keyToRemove) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());

    if (doc.isNull() || doc.isEmpty()) {
        qDebug() << "Failed to parse the JSON document.";
        return QString();
    }

    if (doc.isObject()) {
        doc.setObject(recursiveRemove(doc.object(), keyToRemove).toObject());
    } else if (doc.isArray()) {
        doc.setArray(recursiveRemove(doc.array(), keyToRemove).toArray());
    }

    return QString(doc.toJson(QJsonDocument::Compact));
}

QJsonValue recursiveEmptyArray(const QJsonValue &value, const QString &keyToEmptyArray) {
    if (value.isObject()) {
        QJsonObject object = value.toObject();
        
        // Check for the key whose array should be emptied
        if (object.contains(keyToEmptyArray) && object[keyToEmptyArray].isArray()) {
            object.insert(keyToEmptyArray, QJsonArray());  // Empty the array
        }

        // Recurse into each key-value pair
        for (auto key : object.keys()) {
            object[key] = recursiveEmptyArray(object[key], keyToEmptyArray);
        }
        return object;
    } else if (value.isArray()) {
        QJsonArray array = value.toArray();
        QJsonArray newArray;
        for (int i = 0; i < array.size(); ++i) {
            newArray.append(recursiveEmptyArray(array[i], keyToEmptyArray));
        }
        return newArray;
    } else {
        return value;
    }
}

QString emptyJsonArray(const QString& jsonStr, const QString& keyToEmptyArray) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());

    if (doc.isNull() || doc.isEmpty()) {
        qDebug() << "Failed to parse the JSON document.";
        return QString();
    }

    if (doc.isObject()) {
        doc.setObject(recursiveEmptyArray(doc.object(), keyToEmptyArray).toObject());
    } else if (doc.isArray()) {
        doc.setArray(recursiveEmptyArray(doc.array(), keyToEmptyArray).toArray());
    }

    return QString(doc.toJson(QJsonDocument::Compact));
}

} // namespace TestHelpers
