/********************************************************************************
 * Copyright (c) 2019-2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <QObject>

#include "Models/ModelTimePlot.h"
#include "Presenters/PresenterTimePlot.h"
#include "Views/ViewTimePlot.h"
#include "openPASS/PluginInterface.h"

class ModelTimePlot;
class PresenterTimePlot;
class ViewTimePlot;

class PluginTimePlot : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "openPASS.TimePlot")
    Q_INTERFACES(PluginInterface)

public:
    explicit PluginTimePlot(QObject *const parent = nullptr);
    virtual ~PluginTimePlot() override = default;

public:
    virtual bool initialize() override;
    virtual bool deinitialize() override;

protected:
    ModelTimePlot *modelTimePlot;
    ViewTimePlot *viewTimePlot;
    PresenterTimePlot *presenterTimePlot;
};
