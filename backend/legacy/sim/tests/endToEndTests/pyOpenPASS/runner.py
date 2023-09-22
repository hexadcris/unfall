################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from os import chdir, system
from pathlib import Path

class Runner:
    def __init__(self, executable):
        self.base_path = Path(executable).parent
        self.executable = executable

    def execute(self, logfile, configs_path, results_path):
        chdir(self.base_path)
        command = f'{self.executable} --logFile "{logfile}" --configs "{configs_path}" --results "{results_path}"'
        return system(command)
