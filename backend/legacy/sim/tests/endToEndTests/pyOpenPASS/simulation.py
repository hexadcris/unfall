################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from copy import copy
from dataclasses import dataclass
from typing import List
from path_manager import PathManager, SimulatorBasePaths, ConfigSource
from runner import Runner
from config_modulator import ConfigModulator
from config_parser import TestItem
from path_hasher import generate_hash


@dataclass(init=True, frozen=True)
class SimulationResult:
    exit_code: int
    result_path: str


class ResultCache:
    def __init__(self):
        self.results = dict()

    def get(self, hash) -> SimulationResult:
        return self.results.get(hash)

    def set(self, hash, result: SimulationResult) -> None:
        self.results[hash] = result


class Simulator():
    def __init__(self, simulator_base_paths: SimulatorBasePaths, mutual_config_path, resources_path):
        self.resultCache = ResultCache()
        self.simulator_base_paths = simulator_base_paths
        self.runner = Runner(simulator_base_paths.executable)
        self.mutual_config_path = mutual_config_path
        self.resources_path = resources_path

    def _apply_config_and_run(self, paths: PathManager, test_item) -> SimulationResult:
        # for the hashing we need the actual configuration, but already modified
        # if it is identical to a config we already know, the result is being reused
        paths.collect_config()
        ConfigModulator.apply(test_item, paths.configs)
        config_hash = generate_hash(paths.configs)

        # for keeping reusing results, we keep them in a folder named after the hash
        # telling the pathmanager is necessary for collecting the artifacts later on
        paths.set_results_subfolder(config_hash)
        sim_result = self.resultCache.get(config_hash)

        if not sim_result:
            paths.clear_results()
            sim_exit_code = self.runner.execute(
                paths.logfile, paths.configs, paths.results)
            sim_result = SimulationResult(sim_exit_code, paths.results)
            self.resultCache.set(config_hash, sim_result)
        return sim_result

    def _run_determinism(self, test_item: TestItem) -> List[SimulationResult]:
        run_results = [self._run(test_item)]
        for run in range(test_item.invocations):
            single_run_test_item = copy(test_item)
            single_run_test_item.name = f'Rep{run:02d}'
            single_run_test_item.invocations = 1
            single_run_test_item.random_seed_offset = run
            run_results.append(self._run(single_run_test_item))
        return run_results

    def _run(self, test_item: TestItem) -> SimulationResult:
        config_source = ConfigSource(
            self.mutual_config_path, self.resources_path, test_item.config)
        paths = PathManager(self.simulator_base_paths,
                            config_source, test_item.nodeid, test_item.id)
        run_result = self._apply_config_and_run(paths, test_item)
        paths.collect_artifacts()
        return SimulationResult(run_result.exit_code, paths.artifacts)

    def run(self, test_item: TestItem) -> List[SimulationResult]:
        if hasattr(test_item, 'determinism') and test_item.determinism:
            return self._run_determinism(test_item)
        else:
            return [self._run(test_item)]
