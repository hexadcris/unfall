/********************************************************************************
 * Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "DetectedObject.h"


const WorldObjectInterface* DetectedObject::GetWorldObject() const
{
    return worldObject;
}

double DetectedObject::GetSensorInformation(SensorInformationType informationType) const
{
    return sensorInformation.at(informationType);
}

void DetectedObject::SetSensorInformation(SensorInformationType informationType, double value)
{
    sensorInformation.insert({informationType, value});
}

void DetectedObject::SetSensorMetadata(std::string typeOfSensor, int Id)
{
    sensorType = typeOfSensor;
    sensorId = Id;
}

std::string DetectedObject::GetSensorType()  const
{
    return sensorType;
}

int DetectedObject::GetSensorId() const
{
    return sensorId;
}

bool operator==(const DetectedObject &dObj1, const DetectedObject &dObj2)
{
    //compare members
    if(!(dObj1.worldObject == dObj2.worldObject))
        return false;

    if(!(dObj1.sensorType == dObj2.sensorType  &&
          dObj1.sensorId == dObj2.sensorId))
        return false;

    //compare map "sensorInformation"
    if(dObj1.sensorInformation.size() != dObj2.sensorInformation.size())
      return false;

    typename std::map<SensorInformationType,double>::const_iterator i, j;
    for(i = dObj1.sensorInformation.begin(), j = dObj2.sensorInformation.begin(); i != dObj1.sensorInformation.end(); ++i, ++j)
    {
        if(i->first != j->first)  return false;
        if(i->second != j->second)  return false;
    }

    return true;
}
