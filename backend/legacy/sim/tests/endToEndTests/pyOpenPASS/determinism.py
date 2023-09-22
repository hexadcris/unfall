################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from filecmp import cmp as files_equal
from pathlib import Path

class DeterminismFailed(Exception):
    pass

def applies(simulation_result):
    return len(simulation_result) > 1

def evalulate(simulation_result):
    failed_runs = check_determinism(simulation_result[0].result_path,
        list(map(lambda r: r.result_path, simulation_result[1:])))
    if len(failed_runs):
        raise DeterminismFailed(failed_runs)
    return

def check(simulation_result):
    if applies(simulation_result):
        evalulate(simulation_result)
    pass

def check_determinism(base_run, consecutive_runs):
    result_file = 'Cyclics_Run_{id:03d}.csv'
    failed_runs = list()
    for run_id, run in enumerate(consecutive_runs):
        if not files_equal(
            Path(base_run) / result_file.format(id=run_id),
            Path(run) / result_file.format(id=0)):
            failed_runs.append(run_id)
    return failed_runs

def check_determinismus_failed(excinfo):
    if isinstance(excinfo.value, DeterminismFailed):
        failed_runs = excinfo.value.args[0]
        return "\n".join([
            "Determinism Failed",
            f'  Differing Invocations {failed_runs}'])
    pass