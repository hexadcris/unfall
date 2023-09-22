################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import os
from shutil import copyfile, rmtree
import re
from pathlib import Path


class ConfigSource():
    def __init__(self, common_path, config_base_path, config_dir):
        self.common = common_path
        self.config_full_path = os.path.join(config_base_path, config_dir)


class SimulatorBasePaths():
    LOG_FILE_NAME = 'test.log'

    def __init__(self, executable, input, output, artifacts):
        self.executable = executable
        self.base_path = Path(executable).parent
        self.configs = Path(input) if Path(
            input).is_absolute() else Path(self.base_path) / input
        self.results = Path(output) if Path(
            output).is_absolute() else Path(self.base_path) / output
        self.artifacts = Path(artifacts) if Path(
            artifacts).is_absolute() else Path(self.base_path) / artifacts


class SimulatorPaths():
    def __init__(self, base_paths: SimulatorBasePaths, config_subdirectory, artifacts_subdirectory):
        self.configs = base_paths.configs / config_subdirectory
        self.results = base_paths.results
        self.artifacts = base_paths.artifacts / artifacts_subdirectory


class PathManager:
    def __init__(self, simulation_base_paths: SimulatorBasePaths, config_source: ConfigSource, nodeid: str, id: str):
        simulator_paths = SimulatorPaths(
            simulation_base_paths,
            config_subdirectory=self.as_path(f'{nodeid}::{id}'),
            artifacts_subdirectory=self.as_path(f'{nodeid}::{id}'))
        self.config_source = config_source
        self.results_subfolder = None
        self.results_base_folder = simulator_paths.results
        self.configs = simulator_paths.configs
        self.artifacts = simulator_paths.artifacts

    def set_results_subfolder(self, folder):
        if not self.results_subfolder:
            self.results_subfolder = folder
        else:
            raise Exception("results_subfolder already set")

    @property
    def results(self):
        if not self.results_subfolder:
            raise Exception("results_subfolder not set")
        return self.results_base_folder / self.results_subfolder

    @property
    def logfile(self):
        return self.results / SimulatorBasePaths.LOG_FILE_NAME

    @staticmethod
    def as_path(string) -> Path:
        return Path(re.sub(r':+|@r|@+|<+|>+|"+|\\+|\|+|\?+|\*+', '_', string.replace('::', '/')))

    @staticmethod
    def _copydir(source, dest):
        """Copy a directory structure overwriting existing files"""
        # TODO: reactivate again
        # if not os.path.exists(source):
        #    raise Exception(f"Path '{source}' does not exist")
        for root, _, files in os.walk(source):
            if not os.path.isdir(root):
                os.makedirs(root)

            for file in files:
                rel_path = root.replace(str(source), '').lstrip(os.sep)
                dest_path = os.path.join(dest, rel_path)

                if not os.path.isdir(dest_path):
                    os.makedirs(dest_path)

                copyfile(os.path.join(root, file),
                         os.path.join(dest_path, file))

    @staticmethod
    def _clean_and_create(folder):
        if os.path.isfile(folder) or os.path.islink(folder):
            os.unlink(folder)

        if os.path.exists(folder):
            rmtree(folder, ignore_errors=True)

        os.makedirs(folder)

    def collect_config(self):
        destination = self.configs

        rmtree(destination, ignore_errors=True)
        os.makedirs(destination)
        if self.config_source.common:
            self._copydir(self.config_source.common, destination)
        self._copydir(self.config_source.config_full_path, destination)

    def collect_artifacts(self):
        self._copydir(self.configs,
                      self.artifacts)
        self._copydir(self.results,
                      self.artifacts)

        if self.logfile.exists():
            copyfile(
                str(self.logfile),
                str(self.artifacts / self.logfile.name))

    def clear_results(self):
        self._clean_and_create(self.results)
