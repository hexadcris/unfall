/********************************************************************************
 * Copyright (c) 2016-2017 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  RunStatistic.cpp */
//-----------------------------------------------------------------------------

#include "runStatistic.h"

const QString RunStatistic::StopReasonsStrings[] =
{
    "Due to time out"
};

RunStatistic::RunStatistic(std::uint32_t randomSeed):
    _randomSeed(randomSeed)
{}

void RunStatistic::AddStopReason(int time, StopReason reason)
{
    _stopReasonIdx = static_cast<int>(reason);
    StopTime = time;
}

// ----------------------------- writing out --------------------- //
void RunStatistic::WriteStatistics(QXmlStreamWriter* fileStream)
{
    fileStream->writeStartElement("RandomSeed");
    fileStream->writeCharacters(QString::number(_randomSeed));
    fileStream->writeEndElement();

    fileStream->writeStartElement("VisibilityDistance");
    fileStream->writeCharacters(QString::number(VisibilityDistance));
    fileStream->writeEndElement();

    fileStream->writeStartElement("StopReason");
    fileStream->writeCharacters(StopReasonsStrings[_stopReasonIdx]);
    fileStream->writeEndElement();

    fileStream->writeStartElement("StopTime");
    fileStream->writeCharacters(QString::number(StopTime));
    fileStream->writeEndElement();

    fileStream->writeStartElement("EgoAccident");
    fileStream->writeCharacters(BoolToString(EgoCollision));
    fileStream->writeEndElement();

    double totalDistanceTraveled = 0;
    for(const auto& [_, distance] : distanceTraveled)
    {
        totalDistanceTraveled += distance;
    }

    fileStream->writeStartElement("TotalDistanceTraveled");
    fileStream->writeCharacters(QString::number(totalDistanceTraveled));
    fileStream->writeEndElement();

    fileStream->writeStartElement("EgoDistanceTraveled");
    fileStream->writeCharacters(QString::number(distanceTraveled.empty() ? 0.0 : distanceTraveled.at("0")));
    fileStream->writeEndElement();
}

QString RunStatistic::BoolToString(bool b)
{
    return b? "True" : "False";
}
