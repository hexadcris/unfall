################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from functools import lru_cache
import pytest
from analysis import Analyzer

from config_parser import read_config, get_test_cases, TestCase, TestItem
from path_manager import SimulatorBasePaths
from simulation import Simulator


@lru_cache(maxsize=128)
def _getargs(parent):
    if hasattr(parent, 'args'):
        return parent.args
    elif hasattr(parent, 'parent'):
        return _getargs(parent.parent)
    raise Exception('Invalid test case hierarchy')


class OpenPassFile(pytest.File):
    def __init__(self, *, args, **kwargs):
        super(OpenPassFile, self).__init__(**kwargs)
        self.args = args

    def collect(self):
        try:
            test_definition = read_config(self.fspath)
            yield from OpenPassTestCase.create(
                parent=self,
                test_cases=get_test_cases(test_definition))
        except Exception as exc:
            raise OpenPassException(
                f"Unable to collect tests from '{self.fspath}': {exc}")


class OpenPassTestCase(pytest.Collector):
    def __init__(self, parent, test_case: TestCase):
        super(OpenPassTestCase, self).__init__(test_case.name, parent=parent)
        self.test_case = test_case

    @staticmethod
    def create(parent, test_cases):
        for test_case in test_cases:
            yield OpenPassTestCase.from_parent(parent=parent, test_case=test_case)

    def collect(self):
        if isinstance(self.test_case.children, TestItem):
            yield OpenPassTestItem.from_parent(parent=self, test_item=self.test_case.children)
        else:
            for child in self.test_case.children if isinstance(self.test_case.children, list) else [self.test_case.children]:
                if isinstance(child, TestCase):
                    yield OpenPassTestCase.from_parent(parent=self, test_case=child)
                elif isinstance(child, TestItem):
                    yield OpenPassTestItem.from_parent(parent=self, test_item=child)

class OpenPassTestResults:
    def __init__(self) -> None:
        self.simulation = None
        self.analysis = dict()

class OpenPassTestItem(pytest.Item):
    def __init__(self, parent, test_item=TestItem):
        super(OpenPassTestItem, self).__init__(test_item.name, parent=parent)
        self.test_item = test_item
        setattr(self.test_item, "nodeid", self.nodeid)
        self.simulator = self._resolve_simulator()
        self.analyzer = Analyzer(test_item)
        self.results = OpenPassTestResults()

    def _resolve_simulator(self):
        args = _getargs(self.parent)
        return Simulator(SimulatorBasePaths(
            args.simulation_path,
            args.input_path,
            args.output_path,
            args.artifact_path),
            args.mutual,
            args.resources)

    def runtest(self):
        simulation_result = self.simulator.run(self.test_item)
        self.results.simulation = simulation_result

        for analysis_result in self.analyzer.run(simulation_result):
            self.results.analysis = {**self.results.analysis, **analysis_result}

    def repr_failure(self, excinfo):
        """Called when self.runtest() raises an exception."""
        message = self.analyzer.parse_error(excinfo)
        if message: return message
        return ('An exception occured when executing pyOpenPASS:\n'
                f'{excinfo.traceback[-1].path}:{excinfo.traceback[-1].lineno}\n'
                f'{excinfo.value.args[0]}')

class OpenPassException(Exception):
    pass
