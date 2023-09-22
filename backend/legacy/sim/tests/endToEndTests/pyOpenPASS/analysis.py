################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from query_parser import Query
from pathlib import Path
from determinism import check as check_determinism, check_determinismus_failed
from simulation import SimulationResult
from config_parser import TestItem
import query_executor
import event_loader
from dataclasses import dataclass
from typing import List
from log_parser import get_logs


class SimulationRunFailed(Exception):
    pass


class SuccessRateFailed(Exception):
    pass


@dataclass(init=True, frozen=True)
class AnalysisResult:
    success_rate: float
    failed_runs: List[int]


class Analyzer():
    def __init__(self, test_item: TestItem) -> None:
        self.test_item = test_item

    @staticmethod
    def query_result(base_run, agents, events, run_id, query, datatypes):
        output = query_executor.prepare_output(base_run, agents, datatypes)
        return query_executor.query_output(output, events, run_id, query)

    @staticmethod
    def check_if_failed(simulation_result: List[SimulationResult]):
        if any(map(lambda r: r.exit_code != 0, simulation_result)):
            raise SimulationRunFailed(simulation_result)
        pass

    def run(self, simulation_result: List[SimulationResult]):
        runners = [
            self.check_if_failed,
            check_determinism,
            self.check_query
        ]

        for runner in runners:
            generator = runner(simulation_result)
            if generator:
                for result in generator:
                    yield result

    def check_query(self, simulation_result: List[SimulationResult]):
        if getattr(self.test_item, 'success_rate', None) and len(simulation_result) == 1:
            artifacts = Path(simulation_result[0].result_path)

            failed_runs = set()
            simulation_output = artifacts / 'simulationOutput.xml'
            for query_string in self.test_item.queries:
                query = Query(query_string)

                for i in range(self.test_item.invocations):
                    agents, events = event_loader.get_trace_metadata(
                        str(simulation_output), query.events, i)
                    base_run = artifacts / f'Cyclics_Run_{i:03}.csv'
                    if not self.query_result(str(base_run), agents, events, i, query, getattr(self.test_item, 'datatypes', None)):
                        failed_runs.add(i)

            success_rate = (self.test_item.invocations -
                            len(failed_runs)) / self.test_item.invocations
            yield {'success_rate': success_rate}

            if isinstance(self.test_item.success_rate, tuple):
                min, max = self.test_item.success_rate
                if success_rate < min or success_rate > max:
                    raise SuccessRateFailed(failed_runs,
                                            success_rate,
                                            self.test_item.success_rate)
            if success_rate < self.test_item.success_rate:
                raise SuccessRateFailed(failed_runs,
                                        success_rate,
                                        self.test_item.success_rate)

    @staticmethod
    def _check_simulation_run_failed_error(excinfo):
        if isinstance(excinfo.value, SimulationRunFailed):
            run_result = excinfo.value.args[0]
            if isinstance(run_result, list):
                run_result = run_result[0]
            error_info = ["Simulation Run Failed"]
            for logfile, content in get_logs(run_result.result_path).items():
                error_info.append(f'  Output of log {logfile!r}:')
                for i, line in enumerate(content.splitlines()):
                    error_info.append(f'  {i:02}: {line}')
            return "\n".join(error_info)

    @staticmethod
    def _check_success_rate_failed(excinfo):
        if isinstance(excinfo.value, SuccessRateFailed):
            failed_runs, actual_rate, expected_rate = excinfo.value.args

            if isinstance(expected_rate, tuple):
                min, max = expected_rate
                return "\n".join([
                    "Success Rate Not Reached",
                    f'  Success rate  [Actual {actual_rate*100:.2f}% | Expected [{min*100:.2f}, {max*100:.2f}]%]',
                    f'  Failed RunIds {failed_runs}'])

            return "\n".join([
                "Success Rate Not Reached",
                f'  Success rate  [Actual {actual_rate*100:.2f}% | Expected {expected_rate*100:.2f}%]',
                f'  Failed RunIds {failed_runs}'])

    def parse_error(self, excinfo):
        checker = [
            check_determinismus_failed,
            self._check_simulation_run_failed_error,
            self._check_success_rate_failed
        ]

        for check in checker:
            message = check(excinfo)
            if message:
                return message

        pass
