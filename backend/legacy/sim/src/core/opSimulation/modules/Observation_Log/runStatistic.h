/********************************************************************************
 * Copyright (c) 2016-2017 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <string>
#include <list>
#include <QXmlStreamWriter>

#include "include/observationInterface.h"

class RunStatistic
{
public:
    enum class StopReason
    {
        DueToTimeOut
    };

    RunStatistic(std::uint32_t randomSeed);

    void AddStopReason(int time, StopReason reason);
    void WriteStatistics(QXmlStreamWriter* fileStream);

    // general
    int StopTime = -1; //!<this stays on UNDEFINED_NUMBER, if due time out -> replace in c#
    bool EgoCollision = false;
    std::map<std::string, double> distanceTraveled{}; //!< travel distance per agent
    double VisibilityDistance = -999.0; //!< Visibility distance of world in current run (defined in simulationConfig.xml)

    static QString BoolToString(bool b);

    ~RunStatistic() = default;

private:
    std::uint32_t _randomSeed;
    std::vector<int> _followerIds;

    static const QString StopReasonsStrings[];
    int _stopReasonIdx = static_cast<int>(StopReason::DueToTimeOut);
}; // class RunStatistic


