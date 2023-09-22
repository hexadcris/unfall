/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2018 ITK Engineering GmbH
 *               2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  SimulationConfigImporter.cpp */
//-----------------------------------------------------------------------------


#include "common/opMath.h"

#include "simulationConfigImporter.h"
#include "importerLoggingHelper.h"
#include "importer/importerCommon.h"

namespace TAG = openpass::importer::xml::simulationConfigImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::simulationConfigImporter::attribute;

using namespace Importer;
using namespace SimulationCommon;

std::string SimulationConfigImporter::GetLibrary(const QDomElement& root, const std::string& tag, const std::string& defaultValue)
{
    std::string parsedName;
    if (!Parse(root, tag, parsedName))
    {
        LOG_INTERN(LogLevel::Info) << "Library " + tag << " undefined, falling back to default value " + defaultValue;
        return defaultValue;
    }
    return parsedName;
}

ExperimentConfig::Libraries SimulationConfigImporter::ImportLibraries(QDomElement rootElement)
{
    QDomElement libsRoot;
    if (!SimulationCommon::GetFirstChildElement(rootElement, TAG::libraries, libsRoot))
    {
        LOG_INTERN(LogLevel::Warning) << "No libraries found. Falling back to default values";
        return defaultLibraryMapping;
    }

    ExperimentConfig::Libraries libs;
    for (const auto& [tagName, defaultValue] : defaultLibraryMapping)
    {
        libs.try_emplace(tagName, SimulationConfigImporter::GetLibrary(libsRoot, tagName, defaultValue));
    }
    return libs;
}

void SimulationConfigImporter::ImportExperiment(QDomElement experimentElement,
        Configuration::SimulationConfig& simulationConfig)
{
    ExperimentConfig experimentConfig;
    
    ThrowIfFalse(ParseInt(experimentElement, "ExperimentID", experimentConfig.experimentId),
                 experimentElement, "ExperimentID not valid.");

    ThrowIfFalse(ParseInt(experimentElement, "NumberOfInvocations", experimentConfig.numberOfInvocations),
                 experimentElement, "NumberOfInvocations not valid.");

    unsigned long randomSeed;
    ThrowIfFalse(ParseULong(experimentElement, "RandomSeed", randomSeed),
                 experimentElement, "RandomSeed not valid.");

    experimentConfig.randomSeed = static_cast<std::uint32_t>(randomSeed);

    experimentConfig.libraries = ImportLibraries(experimentElement);

    simulationConfig.SetExperimentConfig(experimentConfig);
}

void SimulationConfigImporter::ImportScenario(QDomElement scenarioElement,
        const std::string configurationDir,
        Configuration::SimulationConfig& simulationConfig)
{
    ScenarioConfig scenarioConfig;
    std::string scenarioFilename;
    ThrowIfFalse(ParseString(scenarioElement, "OpenScenarioFile", scenarioFilename),
                 scenarioElement, "OpenScenarioFile not valid.");

    scenarioConfig.scenarioPath = openpass::core::Directories::Concat(configurationDir, scenarioFilename);
    simulationConfig.SetScenarioConfig(scenarioConfig);
}

void SimulationConfigImporter::ImportEnvironment(QDomElement environmentElement,
        Configuration::SimulationConfig& simulationConfig)
{
    EnvironmentConfig environmentConfig;
    //Parse all time of days
    QDomElement timeOfDaysElement;
    ThrowIfFalse(GetFirstChildElement(environmentElement, TAG::timeOfDays, timeOfDaysElement),
                 environmentElement, "Tag " + std::string(TAG::timeOfDays) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(timeOfDaysElement, "Value", TAG::timeOfDay, environmentConfig.timeOfDays, LogErrorAndThrow),
                 timeOfDaysElement, "Could not import Probabilities.");

    //Parse all visibility distances
    QDomElement visibilityDistancesElement;
    ThrowIfFalse(GetFirstChildElement(environmentElement, TAG::visibilityDistances, visibilityDistancesElement),
                 environmentElement, "Tag " + std::string(TAG::visibilityDistances) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(visibilityDistancesElement, "Value", TAG::visibilityDistance, environmentConfig.visibilityDistances, LogErrorAndThrow),
                 visibilityDistancesElement, "Could not import Probabilities.");

    //Parse all frictions
    QDomElement frictionsElement;
    ThrowIfFalse(GetFirstChildElement(environmentElement, TAG::frictions, frictionsElement),
                 environmentElement, "Tag " + std::string(TAG::frictions) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(frictionsElement, "Value", TAG::friction, environmentConfig.frictions, LogErrorAndThrow),
                 frictionsElement, "Could not import Probabilities.");

    //Parse all weathers
    QDomElement weathersElement;
    ThrowIfFalse(GetFirstChildElement(environmentElement, TAG::weathers, weathersElement),
                 environmentElement, "Tag " + std::string(TAG::weathers) + " is missing.");
    ThrowIfFalse(ImportProbabilityMap(weathersElement, "Value", TAG::weather, environmentConfig.weathers, LogErrorAndThrow),
                 weathersElement, "Could not import Probabilities.");

    //Parse traffic rules
    ThrowIfFalse(ParseString(environmentElement, TAG::trafficRules, environmentConfig.trafficRules),
                 environmentElement, "Tag " + std::string(TAG::trafficRules) + " is missing.");

    QDomElement turningRatesElement;
    if(GetFirstChildElement(environmentElement, TAG::turningRates, turningRatesElement))
    {
        ImportTurningRates(turningRatesElement, environmentConfig.turningRates);
    }
    simulationConfig.SetEnvironmentConfig(environmentConfig);
}

void SimulationConfigImporter::ImportSpawners(const QDomElement &spawnersElement,
                                                  Configuration::SimulationConfig& simulationConfig)
{
    QDomElement spawnPointElement;
    ThrowIfFalse(GetFirstChildElement(spawnersElement, TAG::spawner, spawnPointElement),
                 spawnersElement, "Tag " + std::string(TAG::spawner) + " is missing.");

    while (!spawnPointElement.isNull())
    {
        SpawnPointLibraryInfo spawnPointInfo;

        ThrowIfFalse(ParseString(spawnPointElement, TAG::library, spawnPointInfo.libraryName),
                     spawnPointElement, "Tag " + std::string(TAG::library) + " is missing.");

        std::string type;
        ThrowIfFalse(ParseString(spawnPointElement, TAG::type, type),
                     spawnPointElement, "Tag " + std::string(TAG::type) + " is missing.");
        const auto& spawnPointTypeIter = std::find_if(spawnPointTypeMapping.cbegin(),
                                                      spawnPointTypeMapping.cend(),
                                                      [&type](const auto& spawnPointTypePair) -> bool
                                                      {
                                                          return spawnPointTypePair.second == type;
                                                      });
        ThrowIfFalse(spawnPointTypeIter != spawnPointTypeMapping.cend(),
                     spawnPointElement, "SpawnPoint Type invalid");
        spawnPointInfo.type = spawnPointTypeIter->first;

        ThrowIfFalse(ParseInt(spawnPointElement, TAG::priority, spawnPointInfo.priority),
                     spawnPointElement, "Tag " + std::string(TAG::priority) + " is missing.");

        std::string spawnPointProfile;
        if(ParseString(spawnPointElement, "Profile", spawnPointProfile))
        {
            spawnPointInfo.profileName.emplace(spawnPointProfile);
        }

        simulationConfig.AddSpawnPointInfo(spawnPointInfo);

        spawnPointElement = spawnPointElement.nextSiblingElement(TAG::spawner);
    }
}

void SimulationConfigImporter::ImportObservations(const QDomElement& observationsElement, Configuration::SimulationConfig& simulationConfig)
{
    QDomElement observationElement;
    ThrowIfFalse(GetFirstChildElement(observationsElement, TAG::observation, observationElement),
                 observationsElement, "Tag " + std::string(TAG::observation) + " is missing.");

    int id = 0;

    while (!observationElement.isNull())
    {
        ObservationInstance observationInstance;
        observationInstance.id = id;
        ++id;

        ThrowIfFalse(ParseString(observationElement, TAG::library, observationInstance.libraryName),
                     observationElement, "Tag " + std::string(TAG::library) + " is missing.");

        QDomElement parametersElement;
        ThrowIfFalse(GetFirstChildElement(observationElement, TAG::parameters, parametersElement),
                     observationsElement, "Tag " + std::string(TAG::parameters) + " is missing.");
        observationInstance.parameters = openpass::parameter::Import(parametersElement, observationsElement);

        simulationConfig.AddObservationInstance(observationInstance);

        observationElement = observationElement.nextSiblingElement(TAG::observation);
    }
}

void SimulationConfigImporter::ImportTurningRates(const QDomElement& turningRatesElement, TurningRates& turningRates)
{
    QDomElement turningRateElement;
    ThrowIfFalse(GetFirstChildElement(turningRatesElement, TAG::turningRate, turningRateElement),
                 turningRatesElement, "Tag " + std::string(TAG::turningRate) + " is missing.");

    while (!turningRateElement.isNull())
    {
        TurningRate turnigRate;

        ThrowIfFalse(ParseAttribute(turningRateElement, ATTRIBUTE::incoming, turnigRate.incoming),
                     turningRateElement, "Attribute " + std::string(ATTRIBUTE::incoming) + " is missing.");
        ThrowIfFalse(ParseAttribute(turningRateElement, ATTRIBUTE::outgoing, turnigRate.outgoing),
                     turningRateElement, "Attribute " + std::string(ATTRIBUTE::outgoing) + " is missing.");
        ThrowIfFalse(ParseAttribute(turningRateElement, ATTRIBUTE::weight, turnigRate.weight),
                     turningRateElement, "Attribute " + std::string(ATTRIBUTE::weight) + " is missing.");

        turningRates.emplace_back(turnigRate);

        turningRateElement = turningRateElement.nextSiblingElement(TAG::turningRate);
    }
}

bool SimulationConfigImporter::Import(const std::string& configurationDir,
                                 const std::string& simulationConfigFile,
                                 Configuration::SimulationConfig& simulationConfig)
{
    try
    {
        std::locale::global(std::locale("C"));

        QFile xmlFile(simulationConfigFile.c_str()); // automatic object will be closed on destruction
        ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly), "an error occurred during simulation configuration import");

        QByteArray xmlData(xmlFile.readAll());
        QDomDocument document;
        ThrowIfFalse(document.setContent(xmlData), "invalid xml file format of file " + simulationConfigFile);

        QDomElement documentRoot = document.documentElement();
        if (documentRoot.isNull())
        {
            return false;
        }

        std::string configVersion;
        ParseAttributeString(documentRoot, ATTRIBUTE::schemaVersion, configVersion);
        ThrowIfFalse(configVersion.compare(supportedConfigVersion) == 0, "SimulationConfig version not suppored. Supported version is " + std::string(supportedConfigVersion));

        std::string profilesCatalog;
        ThrowIfFalse(ParseString(documentRoot, "ProfilesCatalog", profilesCatalog), "Could not import Proifles Catalog.");

        simulationConfig.SetProfilesCatalog(openpass::core::Directories::Concat(configurationDir, profilesCatalog));

        QDomElement experimentConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::experiment, experimentConfigElement),
                     "Tag " + std::string(TAG::experiment) + " is missing.");
        ImportExperiment(experimentConfigElement, simulationConfig);

        QDomElement scenarioConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::scenario, scenarioConfigElement),
                     "Tag " + std::string(TAG::scenario) + " is missing.");
        ImportScenario(scenarioConfigElement, configurationDir, simulationConfig);

        QDomElement environmentConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::environment, environmentConfigElement),
                     "Tag " + std::string(TAG::environment) + " is missing.");
        ImportEnvironment(environmentConfigElement, simulationConfig);

        QDomElement observationConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::observations, observationConfigElement),
                     "Tag " + std::string(TAG::observations) + " is missing.");
        ImportObservations(observationConfigElement, simulationConfig);

        QDomElement spawnPointsConfigElement;
        ThrowIfFalse(GetFirstChildElement(documentRoot, TAG::spawners, spawnPointsConfigElement),
                     "Tag " + std::string(TAG::spawners) + " is missing.");
        ImportSpawners(spawnPointsConfigElement, simulationConfig);

        return true;
    }
    catch (const std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "SimulationConfig import failed: " + std::string(e.what());
        return false;
    }
}
