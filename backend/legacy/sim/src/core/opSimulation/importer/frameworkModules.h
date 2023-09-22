/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "directories.h"
#include "common/opExport.h"
#include "common/spawnPointLibraryDefinitions.h"
#include "common/observationLibraryDefinitions.h"

namespace
{
    SpawnPointLibraryInfoCollection ConcatenateSpawnPointLibraries(const std::string& libraryDir, const SpawnPointLibraryInfoCollection& spawnPointLibraries)
    {
        SpawnPointLibraryInfoCollection libraries = spawnPointLibraries;
        for (auto& libraryInfo : libraries)
        {
            libraryInfo.libraryName = openpass::core::Directories::Concat(libraryDir, libraryInfo.libraryName);
        }

        return libraries;
    }

    ObservationInstanceCollection ConcatenateObservationLibraries(const std::string& libraryDir, const ObservationInstanceCollection& observationLibraries)
    {
        ObservationInstanceCollection libraries = observationLibraries;
        for (auto& libraryInfo : libraries)
        {
            libraryInfo.libraryName = openpass::core::Directories::Concat(libraryDir, libraryInfo.libraryName);
        }

        return libraries;
    }
}

struct SIMULATIONCOREEXPORT FrameworkModules
{
public:
    FrameworkModules(const int logLevel,
                     const std::string& libraryDir,
                     const std::string& dataBufferLibrary,
                     const std::string& eventDetectorLibrary,
                     const std::string& manipulatorLibrary,
                     const ObservationInstanceCollection& observationLibraries,
                     const std::string& stochasticsLibrary,
                     const std::string& worldLibrary,
                     const SpawnPointLibraryInfoCollection& spawnPointLibraries) :
        logLevel{logLevel},
        libraryDir{libraryDir},
        dataBufferLibrary{openpass::core::Directories::Concat(libraryDir, dataBufferLibrary)},
        eventDetectorLibrary{openpass::core::Directories::Concat(libraryDir, eventDetectorLibrary)},
        manipulatorLibrary{openpass::core::Directories::Concat(libraryDir, manipulatorLibrary)},
        observationLibraries{ConcatenateObservationLibraries(libraryDir, observationLibraries)},
        stochasticsLibrary{openpass::core::Directories::Concat(libraryDir, stochasticsLibrary)},
        worldLibrary{openpass::core::Directories::Concat(libraryDir, worldLibrary)},
        spawnPointLibraries{ConcatenateSpawnPointLibraries(libraryDir, spawnPointLibraries)}
    {}

    const int logLevel;
    const std::string libraryDir;
    const std::string dataBufferLibrary;
    const std::string eventDetectorLibrary;
    const std::string manipulatorLibrary;
    const ObservationInstanceCollection observationLibraries;
    const std::string stochasticsLibrary;
    const std::string worldLibrary;
    SpawnPointLibraryInfoCollection spawnPointLibraries;
};
