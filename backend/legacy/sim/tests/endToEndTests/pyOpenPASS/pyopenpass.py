################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from pathlib import Path
from dataclasses import dataclass
from openpass_file import OpenPassFile
from os import name as os_name
from report import Report

def addoption(parser):
    group = parser.getgroup("pyopenpass")
    group.addoption(
        "--simulation",
        action="store",
        dest="simulator",
        default=r'C:\OpenPass\bin\opSimulation.exe' if os_name == 'nt' else '/openPass/bin/opSimulation',
        help=r'path to the openPASS simulation executable')
    group.addoption(
        "--configs-path",
        action="store",
        dest="input",
        default=r'configs',
        help=r'relative path for providing configs during testing')
    group.addoption(
        "--results-path",
        action="store",
        dest="output",
        default=r'results',
        help=r'relative path for collecting test results during testing')
    group.addoption(
        "--artifacts-path",
        action="store",
        dest="artifacts",
        default=r'artifacts',
        help=r'relative path for collecting test artifacts during testing')
    group.addoption(
        "--mutual",
        action="store",
        dest="mutual",
        default=None,
        help=r'path to mutual configuration files, if existing')
    group.addoption(
        "--resources",
        action="store",
        dest="resources",
        default=None,
        help=r'path to the resources (e.g. /openPASS/bin/examples/configs)')
    group.addoption(
        "--report-path",
        action="store",
        dest="report_path",
        default='.',
        help=r'path for html report (e.g. /openPASS/bin/examples/report)')

@dataclass(frozen=True)
class Args:
    simulation_path: str
    input_path: str
    output_path: str
    artifact_path: str
    mutual: str
    resources: str
    report_path: str


def parse_arguments(config) -> Args:
    try:
        return Args(
            config.getoption('simulator'),
            config.getoption('input'),
            config.getoption('output'),
            config.getoption('artifacts'),
            config.getoption('mutual'),
            config.getoption('resources'),
            config.getoption('report_path'))
    except ValueError:
        raise OSError(f"Missing one or more required options "
                      f"'simulation', 'mutual', 'resources', or 'report-path'")

def _is_pyopenpass_testfile(path):
    path = Path(path)
    return path.suffix == ".json" and path.name.startswith("test")

class Reporter(Report):
    def __init__(self, config) -> None:
        args = parse_arguments(config)
        super(Reporter, self).__init__(args.report_path)

    @staticmethod
    def make_report(item, report):
        '''
        see https://docs.pytest.org/en/latest/example/simple.html#post-process-test-reports-failures
        '''
        if not _is_pyopenpass_testfile(item.fspath): return
        extra = getattr(report, "extra", [])
        if report.when == "call":
            extra.append(item)
            report.extra = extra

class PyOpenPass:
    def __init__(self, config) -> None:
        self.args = parse_arguments(config)
        self.config = config


    def pytest_collect_file(self, parent, path):
        if _is_pyopenpass_testfile(path):
            return OpenPassFile.from_parent(parent, fspath=path, args=self.args)

    def pytest_collection_modifyitems(cls, items):
        selected_items = []
        deselected_items = []

        for item in items:
            if any(map(lambda s: s.lower().startswith('disabled_'), item.nodeid.split('::'))):
                deselected_items.append(item)
            else:
                selected_items.append(item)

        cls.config.hook.pytest_deselected(items=deselected_items)
        items[:] = selected_items