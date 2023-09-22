/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "XmlAgent.h"
#include "XmlObservation.h"
#include "XmlSpawnPoint.h"

class XmlModelsConfig: public XmlBaseClass
{
public:
    XmlModelsConfig( int startTime, QString endTime, int nInvocations, QString weekday, int timeOfDay,
                  int randomSeed );

    ~XmlModelsConfig();

    bool WriteToXml( QXmlStreamWriter *xmlWriter );

    void AddSpawnPoint( XmlSpawnPoint *sp );
    void AddAgent(int id, int agentTypeRef, PCM_ParticipantData *participant);
    void AddObservation( XmlObservation *observation );

private:
    int startTime = 0;             //!< simulation start time
    QString endTime;               //!< simulation end time
    int numberInvocations =
        0;     //!< number of repetitions of the same situation described with this file
    QString weekday;               //!< weekday of the situation
    int timeOfDay = -1;            //!< hour of the day, the situation happens
    int randomSeed =
        -1;           //!< random seed of the situation for initialisation of non-deterministic modules
    QString worldLibrary = "World_PCM";//!< World.dll
    QString stochastics = "Stochastics"; //!< Stochastics.dll
    QString collisionDetection = "CollisionDetection_Impact"; //!< CollisionDetection.dll

    std::vector<XmlSpawnPoint *> spawnPoints;  //!< vector of all spawn points of the situation
    std::vector<XmlAgent>       agents;        //!< vector of all agents of the situation
    std::vector<XmlObservation *> observations;  //!< vector of all observations of the situation
};

