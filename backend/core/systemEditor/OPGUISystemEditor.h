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

#ifndef SYSTEMEDITOR_H
#define SYSTEMEDITOR_H

#include <QString>
#include <QDomElement>
#include <QList>

#include "OAISystemUI.h"
#include "OAIComponentUI.h"
#include "OAISchedule.h"
#include "OAIPosition.h"
#include "OAIParameter.h"
#include "OAIInput.h"
#include "OAIOutput.h"
#include "OAIConnection.h"

namespace OPGUISystemEditor {

/**
 * @brief Manages the OpenAPI System User Interface components
 *
 * This class provides methods for reading, parsing, exporting, and validating
 * XML representations of OpenAPI System UI components and their associated elements.
 */
class SystemEditor {
public:
    /**
     * @brief Default constructor for SystemEditor
     */
    SystemEditor();

    /* Reading and Parsing Functions */

    /**
     * @brief Reads an XML file and returns its contents as a QString
     */
    QString readXmlFile(const QString &filePath);

    /**
     * @brief Loads components from a directory and adds them to a QList
     */
    bool loadComponentsFromDirectory(const QString &dirPath, QList<OpenAPI::OAIComponentUI> &components, QString &errorMsg);

    /**
     * @brief Loads components from a directory and adds them to a QList
     */
    bool loadSystemsFromDirectory(const QString &dirPath, QList<OpenAPI::OAISystemUI> &systems, QString &errorMsg);

    /**
     * @brief Loads systems from a file and adds them to a QList
     */
    //bool loadSystemsFromFile(const QString &filePath, QList<OpenAPI::OAISystemUI> &systems, QString &errorMsg);

    /**
     * @brief Parses an XML element to populate an OAISystemUI object
     */
    bool parseXmlSystem(const QDomElement &systemEl, OpenAPI::OAISystemUI &system);

    /**
     * @brief Parses an XML element to populate an OAIComponentUI object
     */
    bool parseXmlComponent(const QDomElement &componentEl, OpenAPI::OAIComponentUI &component, bool isInsideSystem, const QList<OpenAPI::OAIComponentUI> &originalComponents);

    /**
     * @brief Parses an XML element to populate a list of OAIParameter objects
     */
    bool parseXmlParameters(const QDomElement &parametersEl, QList<OpenAPI::OAIParameter> &parameters,bool isInsideSystem);

    /**
     * @brief Parses an XML element to populate a list of OAIInput objects
     */
    bool parseXmlInputs(const QDomElement &inputsEl, QList<OpenAPI::OAIInput> &inputs);

    /**
     * @brief Parses an XML element to populate a list of OAIOutput objects
     */
    bool parseXmlOutputs(const QDomElement &outputsEl, QList<OpenAPI::OAIOutput> &outputs);

    /**
     * @brief Parses an XML element to populate an OAISchedule object
     */
    bool parseXmlSchedule(const QDomElement &scheduleEl, OpenAPI::OAISchedule &schedule, bool isInsideSystem);

    /**
     * @brief Parses an XML element to populate an OAIPosition object
     */
    bool parseXmlPosition(const QDomElement &positionEl, OpenAPI::OAIPosition &position);

    /**
     * @brief Parses an XML element to populate an OAIConnection object
     */
    bool parseXmlConnection(const QDomElement &connectionEl, OpenAPI::OAIConnection &connection);

    /* Validation Functions */

    /**
     * @brief Validates the structure of a System XML element
     */
    bool isValidSystemXml(const QDomElement &systemEl);

    /**
     * @brief Validates the structure of a Component XML element
     */
    bool isValidComponentXml(const QDomElement &componentEl, bool isInsideSystem);

    /**
     * @brief Validates the structure of a Parameter XML element
     */
    bool isValidParameterXml(const QDomElement &paramElement, bool isInsideSystem);

    /**
     * @brief Validates the structure of a Input XML element
     */
    bool isValidInputXml(const QDomElement &inputEl);

    /**
     * @brief Validates the structure of a Ouput XML element
     */
    bool isValidOutputXml(const QDomElement &outputEl);

    /**
     * @brief Validates the structure of a Schedule XML element
     */
    bool isValidScheduleXml(const QDomElement &scheduleEl, bool isInsideSystem);

    /**
     * @brief Validates the structure of a Connection XML element
     */
    bool isValidConnectionXml(const QDomElement &connectionEl);

    /**
     * @brief Validates the structure of a Position XML element
     */
    bool isValidPositionXml(const QDomElement &positionEl);
    
    /* Exporting Functions */

    /**
     * @brief Exports a list of systems to an XML file
     */
    bool exportSystemsToXmlFiles(const QString &folderPath, const QList<OpenAPI::OAISystemUI> &systems, QString &errorMsg);

    /**
     * @brief Exports an OAISchedule object to XML
     */
    bool exportScheduleToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAISchedule &schedule, bool isInsideSystem);

    /**
     * @brief Exports an OAIParameter object to XML
     */
    bool exportParameterToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIParameter &param, bool isInsideSystem);

    /**
     * @brief Exports an OAIInput object to XML
     */
    bool exportInputToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIInput &input);

    /**
     * @brief Exports an OAIOutput object to XML
     */
    bool exportOutputToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIOutput &output);

    /**
     * @brief Exports an OAIPosition object to XML
     */
    bool exportPositionToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIPosition &position);

    /**
     * @brief Exports an OAIComponentUI object to XML
     */
    bool exportComponentToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIComponentUI &component, bool isInsideSystem);

    /**
     * @brief Exports an OAISystemUI object to XML
     */
    bool exportSystemToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAISystemUI &system);

    /**
     * @brief Exports an OAIConnection object to XML
     */
    bool exportConnectionToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIConnection &connection);

    /* Object Validation Functions */

    /**
     * @brief Validates an OAISystemUI object
     */
    bool isValidSystem(const OpenAPI::OAISystemUI &component);

    /**
     * @brief Validates an OAIComponentUI object
     */
    bool isValidComponent(const OpenAPI::OAIComponentUI &component, bool isInsideSystem);

    /**
     * @brief Validates an OAISchedule object
     */
    bool isValidSchedule(const OpenAPI::OAISchedule &schedule, bool isInsideSystem);

    /**
     * @brief Validates an OAIParameter object
     */
    bool isValidParameter(const OpenAPI::OAIParameter &param);

    /**
     * @brief Validates an OAIInput object
     */
    bool isValidInput(const OpenAPI::OAIInput &input);

    /**
     * @brief Validates an OAIOutput object
     */
    bool isValidOutput(const OpenAPI::OAIOutput &output);

    /**
     * @brief Validates an OAIPosition object
     */
    bool isValidPosition(const OpenAPI::OAIPosition &position);

    /**
     * @brief Validates an OAIConnection object
     */
    bool isValidConnection(const OpenAPI::OAIConnection &connection);

    /**
     * @brief Finds a component by its title in a list of components
     *
     * This function searches for a component with the given title in a list of OAIComponentUI objects
     * and returns a pointer to it if found, or nullptr otherwise.
     */
    const OpenAPI::OAIComponentUI* findComponentByTitle(const QString &title, const QList<OpenAPI::OAIComponentUI> &components);

    bool backupFile(const QString &sourceFilePath, const QString &backupFolderPath);
};

}  // namespace OPGUISystemEditor

#endif // SYSTEMEDITOR_H
