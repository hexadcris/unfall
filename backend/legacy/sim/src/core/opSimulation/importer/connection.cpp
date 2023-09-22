/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "connection.h"


Connection::Connection(std::string id, std::string incomingRoad, std::string connectingRoad, ContactPointType contactPoint)
{
    this->id = id;
    this->incommingRoadId = incomingRoad;
    this->connectingRoadId = connectingRoad;
    this->contactPoint = contactPoint;
}

Connection::~Connection()
{

}

void Connection::AddLink(int from, int to)
{
    links[from] = to;
}

const std::string& Connection::GetConnectingRoadId() const
{
    return connectingRoadId;
}

const std::string& Connection::GetIncommingRoadId() const
{
    return incommingRoadId;
}

const std::map<int, int>& Connection::GetLinks() const
{
    return links;
}

ContactPointType Connection::GetContactPoint() const
{
    return contactPoint;
}

