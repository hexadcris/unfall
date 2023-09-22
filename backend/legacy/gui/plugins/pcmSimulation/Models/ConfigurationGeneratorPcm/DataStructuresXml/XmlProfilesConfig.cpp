/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "XmlProfilesConfig.h"

XmlProfilesConfig::XmlProfilesConfig(int startTime, QString endTime, int nInvocations, QString weekday,
                                     int timeOfDay, int randomSeed):
    startTime(startTime), endTime(endTime), numberInvocations(nInvocations), weekday(weekday),
    timeOfDay(timeOfDay), randomSeed(randomSeed)
{}

XmlProfilesConfig::~XmlProfilesConfig()
{
    for ( XmlSpawnPoint *sp : spawnPoints)
    {
        delete sp;
    }

    for ( XmlObservation *observation : observations)
    {
        delete observation;
    }
}

void XmlProfilesConfig::AddSpawnPoint(XmlSpawnPoint *sp)
{
    spawnPoints.push_back(sp);
}

void XmlProfilesConfig::AddModelId(int id)
{
    modelIds.push_back(id);
}

void XmlProfilesConfig::AddObservation(XmlObservation *observation)
{
    observations.push_back(observation);
}

bool XmlProfilesConfig::WriteToXml(QXmlStreamWriter *xmlWriter)
{
    if (xmlWriter == nullptr)
    {
        return false;
    }

    xmlWriter->writeStartElement("AgentProfiles");

    for (uint i = 0; i < modelIds.size(); ++i)
    {
        QString name = "Agent_" + QString::number(modelIds.at(i));

        xmlWriter->writeStartElement("AgentProfile");
        xmlWriter->writeAttribute("Name", name);
        xmlWriter->writeAttribute("Type","Static");
        xmlWriter->writeStartElement("System");
        xmlWriter->writeTextElement("File","SystemConfig.xml");
        xmlWriter->writeTextElement("Id", QString::number(modelIds.at(i)));
        xmlWriter->writeEndElement(); // System
        xmlWriter->writeTextElement("VehicleModel", name);
        xmlWriter->writeEndElement(); // AgentProfile
    }
    xmlWriter->writeEndElement(); // AgentProfiles

    xmlWriter->writeStartElement("VehicleProfiles");
    for (int id : modelIds)
    {
        QString name = "Agent_" + QString::number(id);

        xmlWriter->writeStartElement("VehicleProfile");
        xmlWriter->writeAttribute("Name", name);
        xmlWriter->writeStartElement("Model");
        xmlWriter->writeAttribute("Name", name);
        xmlWriter->writeEndElement(); // Model
        xmlWriter->writeStartElement("Components");
        xmlWriter->writeEndElement(); // Components
        xmlWriter->writeStartElement("Sensors");
        xmlWriter->writeEndElement(); // Sensors
        xmlWriter->writeEndElement(); // VehicleProfile
    }
    xmlWriter->writeEndElement(); // VehicleProfiles

    xmlWriter->writeStartElement("ProfileGroup");
    xmlWriter->writeAttribute("Type","Driver");
    xmlWriter->writeEndElement(); // ProfileGroup

    xmlWriter->writeStartElement("ProfileGroup");
    xmlWriter->writeAttribute("Type","TrafficRules");
    xmlWriter->writeStartElement("Profile");
    xmlWriter->writeAttribute("Name","Germany");
    xmlWriter->writeStartElement("Double");
    xmlWriter->writeAttribute("Key", "OpenSpeedLimit");
    xmlWriter->writeAttribute("Value", "INF");
    xmlWriter->writeEndElement(); // Double
    xmlWriter->writeStartElement("Double");
    xmlWriter->writeAttribute("Key", "OpenSpeedLimitTrucks");
    xmlWriter->writeAttribute("Value", "22.2222222");
    xmlWriter->writeEndElement(); // Double
    xmlWriter->writeStartElement("Double");
    xmlWriter->writeAttribute("Key", "OpenSpeedLimitBuses");
    xmlWriter->writeAttribute("Value", "27.7777777");
    xmlWriter->writeEndElement(); // Double
    xmlWriter->writeStartElement("Bool");
    xmlWriter->writeAttribute("Key", "KeepToOuterLanes");

    xmlWriter->writeAttribute("Value", "true");
    xmlWriter->writeEndElement(); // Bool
    xmlWriter->writeStartElement("Bool");
    xmlWriter->writeAttribute("Key", "DontOvertakeOnOuterLanes");
    xmlWriter->writeAttribute("Value", "true");
    xmlWriter->writeEndElement(); // Bool
    xmlWriter->writeStartElement("Bool");
    xmlWriter->writeAttribute("Key", "FormRescueLane");
    xmlWriter->writeAttribute("Value", "true");
    xmlWriter->writeEndElement(); // Bool
    xmlWriter->writeStartElement("Bool");
    xmlWriter->writeAttribute("Key", "ZipperMerge");
    xmlWriter->writeAttribute("Value", "true");
    xmlWriter->writeEndElement(); // Bool
    xmlWriter->writeEndElement(); // Profile
    xmlWriter->writeEndElement(); // ProfileGroup

    return true;
}
