/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ScenarioImporterHelper.h
//! @brief This file contains the helper class of the scenario importer.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <QDomDocument>
#include "include/scenarioInterface.h"
#include "modelElements/parameters.h"
#include "common/openScenarioDefinitions.h"
#include "oscImporterCommon.h"

namespace openScenario
{

class ScenarioImporterHelper
{
public:
    /*!
    * \brief Import an openscenario Position Element
    *
    * @param[in]     root                   Root of the xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    Position
    */
    static Position ImportPosition(QDomElement root, Parameters& parameters);

    /*!
    * \brief Import an openscenario GlobalAction Element
    *
    * @param[in]     globalActionElement                   GlobalAction xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    GlobalAction
    */
    static GlobalAction ImportGlobalAction(QDomElement globalActionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario PrivateAction Element
    *
    * @param[in]     privateActionElement                   PrivatAction xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    * @param[in]     catalogPath        Catalogpath used by some sub functions
    *
    * return    PrivateAction
    */
    static PrivateAction ImportPrivateAction(QDomElement privateActionElement, Parameters& parameters, const std::string catalogPath = "");

    /*!
    * \brief Import an openscenario UserDefinedAction Element
    *
    * @param[in]     userDefinedActionElement                   Root of the xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    UserDefinedAction
    */
    static UserDefinedAction ImportUserDefinedAction(QDomElement userDefinedActionElement);

private:
    /*!
    * \brief Import an openscenario Orientation Element
    *
    * @param[in]     orientationElement                   Orientation xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    Orientation
    */
    static Orientation ImportOrientation(QDomElement& orientationElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario LanePosition Element
    *
    * @param[in]     positionElement                   Position xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    LanePosition
    */
    static LanePosition ImportLanePosition(QDomElement positionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario RelativeLanePosition Element
    *
    * @param[in]     positionElement                   Position xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    RelativeLanePosition
    */
    static RelativeLanePosition ImportRelativeLanePosition(QDomElement positionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario RoadPosition Element
    *
    * @param[in]     positionElement                   Position xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    RoadPosition
    */
    static RoadPosition ImportRoadPosition(QDomElement positionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario WorldPosition Element
    *
    * @param[in]     positionElement                   Position xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    WorldPosition
    */
    static WorldPosition ImportWorldPosition(QDomElement positionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario RelativeWorldPosition Element
    *
    * @param[in]     positionElement                   Position xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    RelativeWorldPosition
    */
    static RelativeWorldPosition ImportRelativeWorldPosition(const QDomElement& positionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario RelativeObjectPosition Element
    *
    * @param[in]     positionElement                   Position xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    RelativeObjectPosition
    */
    static RelativeObjectPosition ImportRelativeObjectPosition(const QDomElement& positionElement, Parameters& parameters);

    /*!
    * \brief Import an openPASS custom Stochastic Element
    *
    * @param[in]     stochasticsElement                   Stochastics xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    attributeName, Stochastic
    */
    static std::pair<std::string, StochasticAttribute> ImportStochastics(QDomElement& stochasticsElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario EntityAction Element
    *
    * @param[in]     entityActionElement               EntityAction xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    EntityAction
    */
    static EntityAction ImportEntityAction(QDomElement entityActionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario EnvironmentAction Element
    *
    * @param[in]     environmentActionElement               EnvironmentAction xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    EnvironmentAction
    */
    static EnvironmentAction ImportEnvironmentAction(QDomElement environmentActionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario LateralAction Element
    *
    * @param[in]     lateralActionElement         LateralAction xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    LateralAction
    */
    static LateralAction ImportLateralAction(QDomElement lateralActionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario LongitudinalAction Element
    *
    * @param[in]     longitudinalActionElement     LongitudinalAction xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    *
    * return    LongitudinalAction
    */
    static LongitudinalAction ImportLongitudinalAction(QDomElement longitudinalActionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario TeleportAction Element
    *
    * @param[in]     teleportActionElement      TeleportAction xml Element
    * @param[in]     parameters                 Parametersset for openScenario parameter references
    *
    * return    TeleportAction
    */
    static TeleportAction ImportTeleportAction(QDomElement teleportActionElement, Parameters& parameters);

    /*!
    * \brief Import an openscenario RoutingAction Element
    *
    * @param[in]     routingActionElement     RoutingAction xml Element
    * @param[in]     parameters       Parametersset for openScenario parameter references
    * @param[in]     catalogPath        CatalogPath
    *
    * return    RoutingAction
    */
    static RoutingAction ImportRoutingAction(QDomElement routingActionElement, Parameters& parameters, const std::string catalogPath);

    /*!
    * \brief Import an openscenario VisibilityAction Element
    *
    * @param[in]     visbilityActionElement     VisibilityAction xml Element
    * @param[in]     parameters                 Parametersset for openScenario parameter references
    *
    * return    VisibilityAction
    */
    static VisibilityAction ImportVisibilityAction(QDomElement visibilityActionElement, Parameters& parameters);

    /*!
    * \brief Gets the Trajectory xml Element from a specified catalog
    *
    * @param[in]     catalogName            Name of the catalog
    * @param[in]     catalogPath              Path of the catalog
    * @param[in]     entryName               Name of the catalog entry
    * @param[in]     parameters              Parametersset for openScenario parameter references
    *
    * return    TrajectoryElement
    */
    static QDomElement GetTrajectoryElementFromCatalog(const std::string& catalogName,
                                                       const std::string& catalogPath,
                                                       const std::string& entryName,
                                                       openScenario::Parameters& parameters);
};
} // namespace openScenario
