/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef XMLGLOBALDATA_H
#define XMLGLOBALDATA_H

#include "pcm_globalData.h"
#include "pcm_helper.h"
#include "XmlBaseClass.h"

class XmlGlobalData: public XmlBaseClass
{
public:
    XmlGlobalData() = default;
    XmlGlobalData(const PCM_GlobalData *globalData);

    bool WriteToXml( QXmlStreamWriter *xmlWriter );

private:
    const PCM_GlobalData *globalData = nullptr; //!< PCM global data
};

#endif // XMLGLOBALDATA_H
