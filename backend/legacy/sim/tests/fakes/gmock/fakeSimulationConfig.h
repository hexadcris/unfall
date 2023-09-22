/********************************************************************************
 * Copyright (c) 2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/simulationConfigInterface.h"

class FakeSimulationConfig : public SimulationConfigInterface {
 public:
  MOCK_CONST_METHOD0(GetExperimentConfig,
      const ExperimentConfig &());
  MOCK_METHOD1(SetExperimentConfig,
      void (const ExperimentConfig& experimentConfig));
  MOCK_CONST_METHOD0(GetScenarioConfig,
      const ScenarioConfig &());
  MOCK_METHOD1(SetScenarioConfig,
      void(const ScenarioConfig& scenarioConfig));
  MOCK_CONST_METHOD0(GetSpawnPointsConfig,
      const SpawnPointLibraryInfoCollection &());
  MOCK_METHOD1(AddSpawnPointInfo,
      void (const SpawnPointLibraryInfo& spawnPointInfo));
  MOCK_CONST_METHOD0(GetObservationConfig,
      const ObservationInstanceCollection &());
  MOCK_METHOD1(AddObservationInstance,
      void (const ObservationInstance& observationInstance));
  MOCK_CONST_METHOD0(GetEnvironmentConfig,
      const EnvironmentConfig &());
  MOCK_METHOD0(SetEnvironmentConfig,
      void (const EnvironmentConfig& environmentConfig)());
  MOCK_CONST_METHOD0(GetProfilesCatalog,
               const std::string& ());
  MOCK_METHOD1(SetProfilesCatalog,
               void (const std::string& profilesCatalog));
};
