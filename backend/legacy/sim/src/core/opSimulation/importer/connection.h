/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef JUNCTIONCONNECTION_H
#define JUNCTIONCONNECTION_H


#include "include/roadInterface/connectionInterface.h"
#include <map>

class Connection : public ConnectionInterface
{
public:
    Connection(std::string id, std::string incomingRoad, std::string connectingRoad, ContactPointType contactPoint);
    ~Connection();

    void virtual AddLink(int from, int to);

    virtual const std::string& GetConnectingRoadId() const override;

    virtual const std::string& GetIncommingRoadId() const override;

    virtual const std::map<int, int>& GetLinks() const override;

    virtual ContactPointType GetContactPoint() const override;


private:
    std::string id;
    std::string incommingRoadId;
    std::string connectingRoadId;
    ContactPointType contactPoint;
    std::map<int,int> links;
};

#endif // JUNCTIONCONNECTION_H
