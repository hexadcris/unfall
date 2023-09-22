/********************************************************************************
 * Copyright (c) 2019-2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef PRESENTERTIMEPLOT_H
#define PRESENTERTIMEPLOT_H

#include <QObject>

#include "Models/ModelTimePlot.h"
#include "Views/ViewTimePlot.h"

class PresenterTimePlot : public QObject
{
    Q_OBJECT

public:
    explicit PresenterTimePlot(ModelTimePlot *modelTimePlot,
                               ViewTimePlot *viewTimePlot,
                               QObject *parent = nullptr);
    virtual ~PresenterTimePlot() = default;

private:
    ModelTimePlot *const modelTimePlot;
    ViewTimePlot *const viewTimePlot;
};

#endif // PRESENTERTIMEPLOT_H
