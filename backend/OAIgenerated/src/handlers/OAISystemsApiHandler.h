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

#ifndef OAI_OAISystemsApiHandler_H
#define OAI_OAISystemsApiHandler_H

#include <QObject>

#include "OAIError400.h"
#include "OAIError500.h"
#include "OAISystemUI.h"
#include <QString>

namespace OpenAPI {

class OAISystemsApiHandler : public QObject
{
    Q_OBJECT

public:
    OAISystemsApiHandler();
    virtual ~OAISystemsApiHandler();


public slots:
    virtual void apiSystemsGet();
    

};

}

#endif // OAI_OAISystemsApiHandler_H
