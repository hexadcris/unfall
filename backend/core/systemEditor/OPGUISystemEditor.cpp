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

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QXmlStreamWriter>

#include "OPGUISystemEditor.h"
#include "OAITarget.h"
#include "OAISource.h"
#include "OPGUIQtLogger.h"
#include "OPGUICoreGlobalConfig.h"

namespace OPGUISystemEditor {

SystemEditor::SystemEditor() = default;

bool SystemEditor::loadComponentsFromDirectory(const QString &dirPath, QList<OpenAPI::OAIComponentUI> &components, QString &errorMsg) {
    LOG_INFO("Loading components from directory "+dirPath);
    QDir dir(dirPath);
    if (!dir.exists()) {
        errorMsg = "Directory does not exist.";
        return false;
    }

    QStringList xmlFiles = dir.entryList(QStringList() << "*.xml", QDir::Files);
    LOG_DEBUG(QString("Found %1 component files in components directory").arg(xmlFiles.size()));
    if(xmlFiles.size()==0){
        errorMsg = "No components files found in directory.";
        return false;
    }
    foreach (const QString &filename, xmlFiles) {
        LOG_DEBUG("Parsing "+filename+" component XML file");
        QString xmlContent = readXmlFile(dir.absoluteFilePath(filename));
        if (xmlContent.isEmpty()) {
            errorMsg = "Component XML file is empty: " + filename;
            return false;
        } else {
            QDomDocument doc;
            if (!doc.setContent(xmlContent)) {
                errorMsg = "Invalid XML content in XML component file: " + filename;
                return false;
            }

            OpenAPI::OAIComponentUI component;
            QDomElement rootElement = doc.documentElement();
            QList<OpenAPI::OAIComponentUI> emptyList = QList<OpenAPI::OAIComponentUI>();
            bool success=parseXmlComponent(rootElement, component, false,emptyList);
            if (!success) {
                errorMsg = "Failed to parse XML component from file: " + filename;
                return false;
            }
            components.append(component);
        }
    }
    return true;
}

bool SystemEditor::loadSystemsFromDirectory(const QString &dirPath, QList<OpenAPI::OAISystemUI> &systems, QString &errorMsg) {
    LOG_INFO("Loading systems from directory "+dirPath);
    QDir dir(dirPath);
    if (!dir.exists()) {
        errorMsg = "Directory does not exist.";
        return false;
    }

    QStringList xmlFiles = dir.entryList(QStringList() << "*.xml", QDir::Files);
    LOG_DEBUG(QString("Found %1 system files in systems directory").arg(xmlFiles.size()));
    if(xmlFiles.size()==0){
        errorMsg = "No system files found in directory.";
        return false;
    }
    foreach (const QString &filename, xmlFiles) {
        LOG_DEBUG("Parsing "+filename+" system XML file");
        QString xmlContent = readXmlFile(dir.absoluteFilePath(filename));
        if (xmlContent.isEmpty()) {
            errorMsg = "System XML file is empty: " + filename;
            return false;
        } else {
            QDomDocument doc;
            if (!doc.setContent(xmlContent)) {
                errorMsg = "Invalid XML content in XML system file: " + filename;
                return false;
            }

            OpenAPI::OAISystemUI system;
            QDomElement rootElement = doc.documentElement();

            if (!parseXmlSystem(rootElement.firstChildElement("system"), system)) {
                errorMsg = "Failed to parse XML system from file: " + filename;
                return false;
            }
            //TOOD check correct filename
            system.setFile(filename);
            systems.append(system);
        }
    }
    return true;
}

QString SystemEditor::readXmlFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR("Failed to open file:" + filePath);
        return QString();  // Return an empty string in case of failure
    }
    QTextStream in(&file);
    QString xmlContent = in.readAll();
    file.close();
    return xmlContent;
}

bool SystemEditor::parseXmlSystem(const QDomElement &systemEl, OpenAPI::OAISystemUI &system) {
    bool success = false;
    // Load original components from the directory
    QList<OpenAPI::OAIComponentUI> originalComponents;
    QString errorMsg;

    if (!loadComponentsFromDirectory(OPGUICoreGlobalConfig::getInstance().fullPathComponentsFolder(), originalComponents, errorMsg)) {
        LOG_ERROR("Failed to load original components: " + errorMsg);
        return false;
    }

    // Validate XML by using the root element
    if (!isValidSystemXml(systemEl)) { // Pass QDomElement directly
        LOG_ERROR("XML from system is not valid according to the System schema.");
        return false;
    }

    QString systemId = systemEl.firstChildElement("id").text();
    QString systemTitle = systemEl.firstChildElement("title").text();
    QString systemPriority = systemEl.firstChildElement("priority").text(); 
    

    LOG_DEBUG(QString("Parsing system with id %1 and title: %2").arg(systemId).arg(systemTitle)); 

    system.setId(systemId.toInt());  
    system.setTitle(systemTitle);
    system.setPriority(systemPriority.toInt());

    // Parse 'components'
    QDomElement componentsNode = systemEl.firstChildElement("components");

    QList<OpenAPI::OAIComponentUI> componentList; // Declare componentList

    QDomNodeList componentsList = componentsNode.elementsByTagName("component");
    if(componentsList.count()==0){
        LOG_ERROR("System has no components");
        return false;
    }
    for (int i = 0; i < componentsList.count(); ++i) {
        QDomElement componentElem = componentsList.at(i).toElement();
        OpenAPI::OAIComponentUI component;
        bool isInsideSystem=true;
        success = parseXmlComponent(componentElem, component, isInsideSystem, originalComponents);
        if (!success) { 
            LOG_ERROR(QString("Failed to parse components at %1th position of the components array").arg(i));
            return false;
        }

        componentList.append(component);
    }

    // Parse 'connections' (Assuming the connections are stored in a similar manner as components)
    QList<OpenAPI::OAIConnection> connectionList; // Declare connectionList
    QDomElement connectionsNode = systemEl.firstChildElement("connections");
    QDomNodeList connectionElems = connectionsNode.elementsByTagName("connection");
    for (int i = 0; i < connectionElems.count(); ++i) {
        QDomElement connectionElem = connectionElems.at(i).toElement();
        OpenAPI::OAIConnection connection;
        success = parseXmlConnection(connectionElem, connection);
        if (!success) { 
            LOG_ERROR(QString("Failed to parse connection at %1th position of the connections array").arg(i));
            return false;
        }

        connectionList.append(connection);
    }
    

    system.setComponents(componentList); // Assume there's a method like this to set components to the system
    system.setConnections(connectionList); // Assume there's a method like this to set connections to the system

    return true;  // Indicate success
}

bool SystemEditor::parseXmlComponent(const QDomElement &componentEl, OpenAPI::OAIComponentUI &component, bool isInsideSystem, const QList<OpenAPI::OAIComponentUI> &originalComponents) {
    bool success=false;
    if (!isValidComponentXml(componentEl, isInsideSystem)) { 
        LOG_ERROR("XML is not valid according to the component schema.");
        return false; 
    }

    // Parse basic attributes
    if (isInsideSystem) {
        QDomElement idElement = componentEl.firstChildElement("id");
        component.setId(idElement.text().toInt());
    } else {
        component.setId(0); 
    }

    // Parsing Title and setting library
    QString title = componentEl.firstChildElement("title").text();
    component.setTitle(title);

    LOG_DEBUG(QString("Parsing component with id %1 and title %2").arg(component.getId()).arg(component.getTitle())); 
    
    if (isInsideSystem){
        QString library = componentEl.firstChildElement("library").text();
        component.setLibrary(library); 
    } else {
        component.setLibrary(title);  
    }

    // Parse Parameters
    QList<OpenAPI::OAIParameter> parameters;
    success = parseXmlParameters(componentEl.firstChildElement("parameters"), parameters,isInsideSystem);
    if(!success) {
        LOG_ERROR("Error parsing parameter of component with title "+component.getTitle());
        return false;
    }
    component.setParameters(parameters);

    // Parse Inputs/Outputs and type
    QList<OpenAPI::OAIInput> inputs;
    QList<OpenAPI::OAIOutput> outputs;
    QString type;
    if (isInsideSystem){
        // Find the original component with matching library title
        const OpenAPI::OAIComponentUI* originalComponentPtr = findComponentByTitle(component.getLibrary(), originalComponents);
        
        if (originalComponentPtr == nullptr) {
            LOG_ERROR("Original component with library name " + component.getLibrary() + " not found");
            return false; // Return false if no original component is found
        }

        inputs = originalComponentPtr->getInputs();
        outputs = originalComponentPtr->getOutputs();
        type = originalComponentPtr->getType();
    }
    else{
        success=parseXmlInputs(componentEl.firstChildElement("inputs"), inputs);
        if(!success) {
            LOG_ERROR("Error parsing inputs of component with title "+component.getTitle());
            return false;
        }
        success = parseXmlOutputs(componentEl.firstChildElement("outputs"), outputs);
        if(!success) {
            LOG_ERROR("Error parsing outputs of component with title "+component.getTitle());
            return false;
        }
        type = componentEl.firstChildElement("type").text();
    }

    component.setType(type);
    component.setInputs(inputs);
    component.setOutputs(outputs);

    // Parse Schedule
    OpenAPI::OAISchedule schedule;
    success = parseXmlSchedule(componentEl.firstChildElement("schedule"), schedule, isInsideSystem);
    if(!success) {
        LOG_ERROR("Error parsing Schedule of component with title "+component.getTitle());
        return false;
    }
    component.setSchedule(schedule);
    LOG_ERROR("DEBUG::priority set with value:"+QString::number(component.getSchedule().getPriority()));
    
    // Parse Position
    OpenAPI::OAIPosition position;
    if (isInsideSystem){
         success = parseXmlPosition(componentEl.firstChildElement("position"), position);
            if(!success){
                LOG_ERROR("Error parsing position of component with title "+component.getTitle());
                return false;
            }
    }
    else{
        position.setX(0);
        position.setY(0);
    }
    component.setPosition(position);

    return true;  
}

bool SystemEditor::parseXmlParameters(const QDomElement &parametersEl, QList<OpenAPI::OAIParameter> &parameters,bool isInsideSystem) {
    if (parametersEl.isNull()) {
        LOG_ERROR("Parameters element not found in XML");
        return false;
    }

    QDomNodeList parametersList = parametersEl.elementsByTagName("parameter");
    for (int i = 0; i < parametersList.count(); i++) {
        QDomElement paramElement = parametersList.at(i).toElement();

        if (!isValidParameterXml(paramElement, isInsideSystem)) {
            LOG_ERROR(QString("Invalid parameter Xml element in %1th position of parameters array").arg(i+1));
            return false; 
        }

        OpenAPI::OAIParameter param;

        param.setId(paramElement.firstChildElement("id").text().toInt());
        param.setType(paramElement.firstChildElement("type").text());
        param.setUnit(paramElement.firstChildElement("unit").text());
        param.setValue(paramElement.firstChildElement("value").text());

        if (isInsideSystem) {
            // Iterate through the child nodes to find the comment
            QDomNodeList childNodes = paramElement.childNodes();
            for (int i = 0; i < childNodes.size(); ++i) {
                QDomNode childNode = childNodes.at(i);
                if (childNode.isComment() && childNode.nodeValue().contains("parameter's title:")) {
                    // Extract the title from the comment
                    QString title = childNode.nodeValue().section("parameter's title:", 1).trimmed();
                    param.setTitle(title);
                    break;
                }
            }
        } else {
            param.setTitle(paramElement.firstChildElement("title").text());
        }

        parameters.append(param);
    }

    return true;  // Return true if successful
}

bool SystemEditor::parseXmlInputs(const QDomElement &inputsEl, QList<OpenAPI::OAIInput> &inputs) {
    QDomNodeList inputList = inputsEl.elementsByTagName("input");
    for (int i = 0; i < inputList.count(); i++) {
        QDomElement inputElem = inputList.at(i).toElement();

        if (!isValidInputXml(inputElem)) {
            LOG_ERROR(QString("Invalid input Xml element at %1th position of the inputs array").arg(i));
            return false;
        }

        OpenAPI::OAIInput input;
        input.setId(inputElem.firstChildElement("id").text().toInt());
        input.setType(inputElem.firstChildElement("type").text());
        input.setTitle(inputElem.firstChildElement("title").text());
        input.setUnit(inputElem.firstChildElement("unit").text());
        input.setCardinality(inputElem.firstChildElement("cardinality").text().toInt());

        inputs.append(input);
    }
    return true;  // Return true if successful
}

bool SystemEditor::parseXmlOutputs(const QDomElement &outputsEl, QList<OpenAPI::OAIOutput> &outputs) {
    QDomNodeList outputList = outputsEl.elementsByTagName("output");
    for (int i = 0; i < outputList.count(); i++) {
        QDomElement outputElem = outputList.at(i).toElement();

        if (!isValidOutputXml(outputElem)) {
            LOG_ERROR(QString("Invalid output Xml element at %1th position of the outputs array").arg(i));
            return false;
        }

        OpenAPI::OAIOutput output;
        output.setId(outputElem.firstChildElement("id").text().toInt());
        output.setType(outputElem.firstChildElement("type").text());
        output.setTitle(outputElem.firstChildElement("title").text());
        output.setUnit(outputElem.firstChildElement("unit").text());

        outputs.append(output);
    }
    return true;  // Return true if successful
}

bool SystemEditor::parseXmlSchedule(const QDomElement &scheduleEl, OpenAPI::OAISchedule &schedule, bool isInsideSystem) {
    if (!isValidScheduleXml(scheduleEl, isInsideSystem)) {
        LOG_ERROR("Invalid Schedule Xml element");
        return false;
    }

    if (isInsideSystem) {
        LOG_ERROR("DEBUG::setting schedule priority to value");
        schedule.setPriority(scheduleEl.firstChildElement("priority").text().toInt());
    }
    else{
        LOG_ERROR("DEBUG::setting schedule priority to 0!");
        schedule.setPriority(0);
    }
    
    schedule.setOffset(scheduleEl.firstChildElement("offset").text().toInt());
    schedule.setCycle(scheduleEl.firstChildElement("cycle").text().toInt());
    schedule.setResponse(scheduleEl.firstChildElement("response").text().toInt());

    return true;  // Return true if successful
}

bool SystemEditor::parseXmlPosition(const QDomElement &positionEl, OpenAPI::OAIPosition &position) {
    if (!isValidPositionXml(positionEl)) {
        LOG_ERROR("Invalid Position Xml element");
        return false;
    }

    position.setX(positionEl.firstChildElement("x").text().toInt());
    position.setY(positionEl.firstChildElement("y").text().toInt());
    return true;  
}

bool SystemEditor::parseXmlConnection(const QDomElement &connectionEl, OpenAPI::OAIConnection &connection) {
    // First, validate the connection XML element
    if (!isValidConnectionXml(connectionEl)) {
        LOG_ERROR(QString("Invalid connection Xml element"));
        return false;
    }

    // Parse the ID of the connection
    connection.setId(connectionEl.firstChildElement("id").text().toInt());

    // Parse the 'source' element and its details
    QDomElement sourceEl = connectionEl.firstChildElement("source");
    OpenAPI::OAISource source;
    source.setComponent(sourceEl.firstChildElement("component").text().toInt());
    source.setOutput(sourceEl.firstChildElement("output").text().toInt());
    connection.setSource(source);

    // Parse the 'target' element and its details
    QDomElement targetEl = connectionEl.firstChildElement("target");
    OpenAPI::OAITarget target;
    target.setComponent(targetEl.firstChildElement("component").text().toInt());
    target.setInput(targetEl.firstChildElement("input").text().toInt());
    connection.setTarget(target);

    return true;
}

bool SystemEditor::exportScheduleToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAISchedule &schedule, bool isInsideSystem) {
    if(!isValidSchedule(schedule,isInsideSystem)){
        LOG_ERROR("Invalid Schedule object");
        return false;
    }
    xmlWriter.writeStartElement("schedule");
    xmlWriter.writeTextElement("offset", QString::number(schedule.getOffset()));
    xmlWriter.writeTextElement("cycle", QString::number(schedule.getCycle()));
    xmlWriter.writeTextElement("response", QString::number(schedule.getResponse()));
    if(isInsideSystem){
        xmlWriter.writeTextElement("priority", QString::number(schedule.getPriority()));
    }
    xmlWriter.writeEndElement(); 
    return true;
}

bool SystemEditor::exportParameterToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIParameter &param, bool isInsideSystem) {
    if(!isValidParameter(param)){
        LOG_ERROR("Invalid Parameter object");
        return false;
    }
    
    xmlWriter.writeStartElement("parameter");

    if (isInsideSystem) {
        // Write title as a comment
        xmlWriter.writeComment(QString("parameter's title: %1").arg(param.getTitle()));
    } else {
        // Write title as a text element
        xmlWriter.writeTextElement("title", param.getTitle());
    }

    xmlWriter.writeTextElement("id", QString::number(param.getId()));
    xmlWriter.writeTextElement("type", param.getType());
    if(param.getUnit().isEmpty() && isInsideSystem)
        xmlWriter.writeEmptyElement("unit");
    else
        xmlWriter.writeTextElement("unit", param.getUnit());
    xmlWriter.writeTextElement("value", param.getValue());

    xmlWriter.writeEndElement(); // End of "parameter"

    return true;
}

bool SystemEditor::exportConnectionToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIConnection &connection) {
    if(!isValidConnection(connection)){
        LOG_ERROR("Invalid Connection object");
        return false;
    }
    xmlWriter.writeStartElement("connection");

    // Exporting OAIConnection fields
    xmlWriter.writeTextElement("id", QString::number(connection.getId()));

    // Export Source
    xmlWriter.writeStartElement("source");
    xmlWriter.writeTextElement("component", QString::number(connection.getSource().getComponent()));
    xmlWriter.writeTextElement("output", QString::number(connection.getSource().getOutput()));
    xmlWriter.writeEndElement(); // End of Source

    // Export Target
    xmlWriter.writeStartElement("target");
    xmlWriter.writeTextElement("component", QString::number(connection.getTarget().getComponent()));
    xmlWriter.writeTextElement("input", QString::number(connection.getTarget().getInput()));
    xmlWriter.writeEndElement(); // End of Target

    xmlWriter.writeEndElement(); // End of Connection
    return true;
}

bool SystemEditor::exportInputToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIInput &input) {
    if(!isValidInput(input)){
        LOG_ERROR("Invalid Input object");
        return false;
    }
    xmlWriter.writeStartElement("input");
    xmlWriter.writeTextElement("id", QString::number(input.getId()));
    xmlWriter.writeTextElement("type", input.getType());
    xmlWriter.writeTextElement("title", input.getTitle());
    xmlWriter.writeTextElement("unit", input.getUnit());
    xmlWriter.writeTextElement("cardinality", QString::number(input.getCardinality()));
    xmlWriter.writeEndElement(); 
    return true;
}

bool SystemEditor::exportOutputToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIOutput &output) {
    if(!isValidOutput(output)){
        LOG_ERROR("Invalid Output object");
        return false;
    }
    xmlWriter.writeStartElement("output");
    xmlWriter.writeTextElement("id", QString::number(output.getId()));
    xmlWriter.writeTextElement("type", output.getType());
    xmlWriter.writeTextElement("title", output.getTitle());
    xmlWriter.writeTextElement("unit", output.getUnit());
    xmlWriter.writeEndElement(); 
    return true;
}

bool SystemEditor::exportPositionToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIPosition &position) {
    if(!isValidPosition(position)){
        LOG_ERROR("Invalid Position object");
        return false;
    }
    xmlWriter.writeStartElement("position");
    xmlWriter.writeTextElement("x", QString::number(position.getX()));
    xmlWriter.writeTextElement("y", QString::number(position.getY()));
    xmlWriter.writeEndElement(); 
    return true;
}

bool SystemEditor::exportComponentToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAIComponentUI &component, bool isInsideSystem) {
    bool success = false;
    if (!isValidComponent(component,isInsideSystem)){
        LOG_ERROR("Invalid Component object");
        return false;
    }

    xmlWriter.writeStartElement("component");

    // Exporting OAIComponentUI fields
    if (isInsideSystem) {
        xmlWriter.writeTextElement("id", QString::number(component.getId()));
        xmlWriter.writeTextElement("library", component.getLibrary());
    }
    else{
        xmlWriter.writeTextElement("type", component.getType());
    }
    
    xmlWriter.writeTextElement("title", component.getTitle());

    LOG_DEBUG(QString("Exporting component with id %1 and title %2").arg(component.getId()).arg(component.getTitle())); 
    success = exportScheduleToXml(xmlWriter, component.getSchedule(),isInsideSystem);
    if (!success){
        LOG_ERROR("Failed to export schedule element");
        return false;
    }
    
    xmlWriter.writeStartElement("parameters");
    QList<OpenAPI::OAIParameter> parameters = component.getParameters();
    for (int i = 0; i < parameters.size(); ++i) {
        OpenAPI::OAIParameter param = parameters.at(i);
        success = exportParameterToXml(xmlWriter, param, isInsideSystem);
        if (!success) {
            LOG_ERROR(QString("Failed to export parameter at position %1th inside the component").arg(i + 1));
            return false;
        }
    }
    xmlWriter.writeEndElement(); 

    if (!isInsideSystem) {
        xmlWriter.writeStartElement("inputs");
        QList<OpenAPI::OAIInput> inputs = component.getInputs(); 
        for (int i = 0; i < inputs.size(); ++i) {
            OpenAPI::OAIInput input = inputs.at(i); 
            success =  exportInputToXml(xmlWriter, input);
            if (!success) {
                LOG_ERROR(QString("Failed to export input at position %1th inside the component").arg(i + 1));
                return false;
            }
        }
        xmlWriter.writeEndElement(); 

        xmlWriter.writeStartElement("outputs");
        QList<OpenAPI::OAIOutput> outputs = component.getOutputs(); 
        for (int i = 0; i < outputs.size(); ++i) {
            OpenAPI::OAIOutput output = outputs.at(i);  
            success = exportOutputToXml(xmlWriter, output);
            if (!success) {
                LOG_ERROR(QString("Failed to export output at position %1th inside the component").arg(i + 1));
                return false;
            }
        }
        xmlWriter.writeEndElement(); 
    }
    else{
        OpenAPI::OAIPosition position = component.getPosition(); 
        success = exportPositionToXml(xmlWriter, position);
        if (!success) {
                LOG_ERROR("Failed to export position element");
                return false;
        }
    }

    xmlWriter.writeEndElement(); 
    return true;
}

bool SystemEditor::exportSystemToXml(QXmlStreamWriter &xmlWriter, const OpenAPI::OAISystemUI &system) {
    bool success=false;
    if (!isValidSystem(system)){
        LOG_ERROR("Invalid System object");
        return false;
    }
    xmlWriter.writeStartElement("systems");
    xmlWriter.writeStartElement("system");

    // Exporting OAISystem fields
    xmlWriter.writeTextElement("id", QString::number(system.getId()));
    xmlWriter.writeTextElement("title", system.getTitle());
    xmlWriter.writeTextElement("priority", QString::number(system.getPriority()));

    LOG_DEBUG( QString("Exporting system with id %1 and title %2").arg(system.getId()).arg(system.getTitle()));

    // Export Components
    xmlWriter.writeStartElement("components");
    QList<OpenAPI::OAIComponentUI> components = system.getComponents();
    if(components.size()==0){
        LOG_ERROR("System has no components");
        return false;
    }
    for (int i = 0; i < components.size(); ++i) {
        OpenAPI::OAIComponentUI component = components.at(i);
        success = exportComponentToXml(xmlWriter, component, true);
        if (!success) {
            LOG_ERROR(QString("Failed to export component at position %1th of the system").arg(i + 1)); 
            return false;
        }
    }
    xmlWriter.writeEndElement(); // End of Components

    // Export Connections
    xmlWriter.writeStartElement("connections");
    QList<OpenAPI::OAIConnection> connections = system.getConnections();
    for (int i = 0; i < connections.size(); ++i) {
        OpenAPI::OAIConnection connection = connections.at(i);
        success = exportConnectionToXml(xmlWriter, connection);
        if (!success) {
            LOG_ERROR(QString("Failed to export connection at position %1th").arg(i + 1));
            return false;
        }
    }
    xmlWriter.writeEndElement(); // End of Connections

    xmlWriter.writeEndElement(); // End of System

    xmlWriter.writeEndElement(); // End of Systems
    return true;
}

bool SystemEditor::exportSystemsToXmlFiles(const QString &folderPath, const QList<OpenAPI::OAISystemUI> &systems, QString &errorMsg) {
    QString backupFolderPath = folderPath + "/bk";
    QDir().mkpath(backupFolderPath); // Create the backup folder if it doesn't exist

    for (const auto &system : systems) {
        QString filePath = folderPath + "/" + system.getFile();

        if (system.is_deleted_Set() && system.is_deleted_Valid() && system.isDeleted()) {
            if (QFile::exists(filePath) && !QFile::remove(filePath)) {
                LOG_ERROR("Failed to delete system file:" + filePath);
            }
            if (QFile::exists(backupFolderPath + "/" + system.getFile() + ".bk1") &&  !QFile::remove(backupFolderPath + "/" + system.getFile() + ".bk1")) {
                LOG_ERROR("Failed to delete system backup file:" + backupFolderPath + "/" + system.getFile() + ".bk1");
            }
        } else {
            backupFile(filePath, backupFolderPath);

            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly)) {
                errorMsg = "Failed to open/create file " + filePath + " for writing the System.";
                LOG_ERROR(errorMsg);
                return false; 
            }

            QXmlStreamWriter xmlWriter(&file);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();

            bool success=exportSystemToXml(xmlWriter, system);
            if (!success) {
                errorMsg = "Failed to export system with file " + system.getFile() + " to XML.";
                LOG_ERROR(errorMsg);
                file.close();
                return false;
            }

            xmlWriter.writeEndDocument();
            file.close();
        }
    }

    return true;
}

bool SystemEditor::isValidSystemXml(const QDomElement &system) {
    // Check for all required elements for a system
    if (system.firstChildElement("id").isNull() || system.firstChildElement("id").text().isEmpty() || 
        system.firstChildElement("title").isNull() || system.firstChildElement("title").text().isEmpty() || 
        system.firstChildElement("priority").isNull() || system.firstChildElement("priority").text().isEmpty() || 
        system.firstChildElement("components").isNull() ||
        system.firstChildElement("connections").isNull()) {
        LOG_ERROR("Missing/empty one or more required XML elements: id, title, priority, components, connections");
        return false;
    }
    return true;
}

bool SystemEditor::isValidComponentXml(const QDomElement &component, bool isInsideSystem) {
    // Check common required elements
    if (component.firstChildElement("title").isNull() ||
        component.firstChildElement("parameters").isNull() ||
        component.firstChildElement("schedule").isNull() ) {
        LOG_ERROR("Title, parameters or schedule for component are not present");
        return false;
    }

    if (isInsideSystem) {
        if (component.firstChildElement("id").isNull() || 
            component.firstChildElement("library").isNull() || 
            component.firstChildElement("position").isNull()) {
            LOG_ERROR("Id, library and position should be present for a component inside a system");
            return false;
        }
    } else {
        if (component.firstChildElement("type").isNull() ||
            component.firstChildElement("inputs").isNull() || 
            component.firstChildElement("outputs").isNull()) {
            LOG_ERROR("Inputs, outputs and type should be present for a standalone component");
            return false;
        }
    }

    return true;
}

bool SystemEditor::isValidParameterXml(const QDomElement &paramElement, bool isInsideSystem) {
    if (paramElement.isNull()) {
        LOG_ERROR("Parameter element is null");
        return false;
    }

    if (isInsideSystem) {
        bool foundComment = false;
        QDomNodeList childNodes = paramElement.childNodes();
        for (int i = 0; i < childNodes.size(); ++i) {
            QDomNode childNode = childNodes.at(i);
            if (childNode.isComment() && childNode.nodeValue().contains("parameter's title:")) {
                foundComment = true;
                break;
            }
        }
        
        if (!foundComment) {
            LOG_ERROR("Missing required commented field 'parameter's title' in parameter");
            return false;
        }
    } else {
        if(paramElement.firstChildElement("title").isNull()){
            LOG_ERROR("Missing required field title in parameter");
            return false;
        }
    }

    if (paramElement.firstChildElement("id").isNull() ||
        paramElement.firstChildElement("type").isNull() ||
        paramElement.firstChildElement("unit").isNull()) {
        LOG_ERROR("Missing required fields id, type or unit in parameter");
        return false;
    }

    bool conversionOk;
    paramElement.firstChildElement("id").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("ID is not a valid integer");
        return false;
    }

    QString typeStr = paramElement.firstChildElement("type").text();
    QString valueStr = paramElement.firstChildElement("value").text();
    
    // Check if the value is empty; if so, it's considered valid
    if (valueStr.isEmpty()) {
        return true;
    }

    if (typeStr == "int") {
        int intValue = valueStr.toInt(&conversionOk);
        if (!conversionOk) {
            LOG_ERROR("Value is not a valid integer");
            return false;
        }
    } else if (typeStr == "bool") {
        if (!(valueStr.toLower() == "true" || valueStr == "1" || valueStr.toLower() == "false" || valueStr == "0")) {
            LOG_ERROR("Value is not a valid boolean");
            return false;
        }
    } else if (typeStr == "float" || typeStr == "double") {
        double doubleValue = valueStr.toDouble(&conversionOk);
        if (!conversionOk) {
            LOG_ERROR("Value is not a valid float/double");
            return false;
        }
    } else if (typeStr == "string" || typeStr == "normalDistribution") {
        // If it's a string, no additional validation is needed
    } else {
        LOG_ERROR("Unknown type: " + typeStr);
        return false;
    }
    
    return true;
}

bool SystemEditor::isValidInputXml(const QDomElement &inputEl) {
    if (inputEl.isNull()) {
        LOG_ERROR("Input element is null");
        return false;
    }

    if (inputEl.firstChildElement("id").isNull() ||
        inputEl.firstChildElement("type").isNull() ||
        inputEl.firstChildElement("title").isNull() ||
        inputEl.firstChildElement("unit").isNull() ||
        inputEl.firstChildElement("cardinality").isNull()) {
        LOG_ERROR("Missing one or more required fields id, type, title, unit or cardinality in input");
        return false;
    }

    bool conversionOk;
    inputEl.firstChildElement("id").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("ID is not a valid integer");
        return false;
    }

    inputEl.firstChildElement("cardinality").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("Cardinality is not a valid integer");
        return false;
    }

    return true;
}

bool SystemEditor::isValidOutputXml(const QDomElement &outputEl) {
    if (outputEl.isNull()) {
        LOG_ERROR("Output element is null");
        return false;
    }

    if (outputEl.firstChildElement("id").isNull() ||
        outputEl.firstChildElement("type").isNull() ||
        outputEl.firstChildElement("title").isNull() ||
        outputEl.firstChildElement("unit").isNull()) {
        LOG_ERROR("Missing one or more required fields id, type, title, unit or cardinality in output");
        return false;
    }

    bool conversionOk;
    outputEl.firstChildElement("id").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("ID is not a valid integer");
        return false;
    }

    return true;
}

bool SystemEditor::isValidScheduleXml(const QDomElement &scheduleEl, bool isInsideSystem) {
    if (scheduleEl.isNull()) {
        LOG_ERROR("Schedule element is null");
        return false;
    }

    if (scheduleEl.firstChildElement("offset").isNull() ||
        scheduleEl.firstChildElement("cycle").isNull() ||
        scheduleEl.firstChildElement("response").isNull()) {
        LOG_ERROR("Missing one or more required fields offset, cycle or response in schedule");
        return false;
    }

    if (isInsideSystem && scheduleEl.firstChildElement("priority").isNull()) {
        LOG_ERROR("Priority field missing in schedule");
        return false;
    }

    bool conversionOk;
    scheduleEl.firstChildElement("offset").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("offset is not a valid integer:" + scheduleEl.firstChildElement("offset").text());
        return false;
    }

    scheduleEl.firstChildElement("cycle").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("cycle is not a valid integer:" + scheduleEl.firstChildElement("cycle").text());
        return false;
    }

    scheduleEl.firstChildElement("response").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("response is not a valid integer:" + scheduleEl.firstChildElement("response").text());
        return false;
    }

    return true;
}

bool SystemEditor::isValidConnectionXml(const QDomElement &connectionEl) {
    bool conversionOk;

    // Validate the ID of the connection
    // Validate the source component and output
    QDomElement  idEl= connectionEl.firstChildElement("id");
    if (idEl.isNull()) {
        LOG_ERROR("Connection Id element is missing");
        return false;
    }

    connectionEl.firstChildElement("id").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("Connection ID is not a valid integer: " + connectionEl.firstChildElement("id").text());
        return false;
    }

    // Validate the source component and output
    QDomElement sourceEl = connectionEl.firstChildElement("source");
    if (sourceEl.isNull()) {
        LOG_ERROR("Connection Source element is missing");
        return false;
    }

    sourceEl.firstChildElement("component").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("Connection Source component missing/not a valid intege: " + sourceEl.firstChildElement("component").text());
        return false;
    }

    sourceEl.firstChildElement("output").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("Connection Source output missing/not a valid intege: " + sourceEl.firstChildElement("output").text());
        return false;
    }

    // Validate the target component and input
    QDomElement targetEl = connectionEl.firstChildElement("target");
    if (targetEl.isNull()) {
        LOG_ERROR("Connection Target element is missing");
        return false;
    }

    targetEl.firstChildElement("component").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("Connection Target component missing/not a valid integer: " + targetEl.firstChildElement("component").text());
        return false;
    }

    targetEl.firstChildElement("input").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("Connection Target input missing/not a valid intege: " + targetEl.firstChildElement("input").text());
        return false;
    }

    return true;
}

bool SystemEditor::isValidPositionXml(const QDomElement &positionEl) {
    // Check if both x and y elements are present
    if (positionEl.firstChildElement("x").isNull() ||
        positionEl.firstChildElement("y").isNull()) {
        LOG_ERROR("Missing one or more required fields x or y in position");
        return false;
    }

    // Check if x and y are valid integers
    bool conversionOk;
    positionEl.firstChildElement("x").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("x coordinate is not a valid integer: " + positionEl.firstChildElement("x").text());
        return false;
    }

    positionEl.firstChildElement("y").text().toInt(&conversionOk);
    if (!conversionOk) {
        LOG_ERROR("y coordinate is not a valid integer: " + positionEl.firstChildElement("y").text());
        return false;
    }

    // If all checks pass, return true
    return true;
}

bool SystemEditor::isValidSystem(const OpenAPI::OAISystemUI &system){
        if( !system.is_id_Valid() || !system.is_title_Valid() || !system.is_priority_Valid()){
            LOG_ERROR("Missing one or more required fields id, title, priority in system");
            return false;
        }
        return true;
};

bool SystemEditor::isValidComponent(const OpenAPI::OAIComponentUI &component, bool isInsideSystem){
    if(isInsideSystem){
        if( !component.is_id_Valid() || !component.is_library_Valid() || !component.is_title_Valid() ){
            LOG_ERROR("Missing one or more required fields id, library or title in component");
            return false;
        }
        return true;
    }
    else{
        if( !component.is_type_Valid() || !component.is_title_Valid()){
            LOG_ERROR("Missing one or more required fields type,  title in component");
            return false;
        }  
        return true; 
    }
    //array and objects fields are checked later
};

bool SystemEditor::isValidSchedule(const OpenAPI::OAISchedule &schedule, bool isInsideSystem){
    if(isInsideSystem){
        return schedule.isValid();
    }
    else{
        if(!schedule.is_offset_Valid() || !schedule.is_cycle_Valid() || !schedule.is_response_Valid()){
            LOG_ERROR("Missing one or more required offset, cycle or response in schedule");
            return false;
        }
        return true;
    }
};

bool SystemEditor::isValidParameter(const OpenAPI::OAIParameter &param){
    return param.isValid();
};

bool SystemEditor::isValidInput(const OpenAPI::OAIInput &input){
    return input.isValid();
};

bool SystemEditor::isValidOutput(const OpenAPI::OAIOutput &output){
    return output.isValid();
};

bool SystemEditor::isValidPosition(const OpenAPI::OAIPosition &position){
    return position.isValid();
};

bool SystemEditor::isValidConnection(const OpenAPI::OAIConnection &connection){
    return connection.isValid();
}

const OpenAPI::OAIComponentUI* SystemEditor::findComponentByTitle(const QString &title, const QList<OpenAPI::OAIComponentUI> &components) {
    for (const auto &component : components) {
        if (component.getTitle() == title) {
            return &component;
        }
    }
    return nullptr; // return nullptr if not found
}

bool SystemEditor::backupFile(const QString &sourceFilePath, const QString &backupFolderPath) {
    bool success = true;
    QString fileName = QFileInfo(sourceFilePath).fileName();
    QString backupFilePath1 = backupFolderPath + "/" + fileName + ".bk1";
    QString backupFilePath2 = backupFolderPath + "/" + fileName + ".bk2";

    // Rename the previous backup to become the new backup
    if (QFile::exists(backupFilePath1) && !QFile::rename(backupFilePath1, backupFilePath2)) {
        LOG_ERROR("Failed to rename 1st backup file:" + backupFilePath1 + "to 2nd backup " + backupFilePath2);
        success = false;
    }

    // Backup the existing file if it exists
    if (QFile::exists(sourceFilePath)) {
        if (!QFile::copy(sourceFilePath, backupFilePath1)) {
            LOG_ERROR("Failed to copy the original file:" + sourceFilePath + " to 1st backup " + backupFilePath1);
            success = false;
        }
    }
    return success;
}

}  // namespace OPGUISystemEditor




