################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#               2020-2021 in-tech GmbH
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# Based on https://github.com/pytest-dev/pytest-html/blob/5e4950b6f47479eb3ce90999474b93c473e5aa9c/src/pytest_html/html_report.py


from dataclasses import dataclass
from distutils import dir_util
import time
import datetime
from enum import Enum
from pathlib import Path
from config_parser import TestItem
from typing import Dict, List


class Outcome(Enum):
    passed = "🟢"
    failed = "❌"
    xfailed = "💀"
    disabled = "⏸️"

    @staticmethod
    def decorate(outcome_string):
        return f'<div>{Outcome[outcome_string].value} {outcome_string}</div>'

def to_table_row(items, header=False):
    tag = 'th' if header else 'td'
    return r'<tr>' + ''.join(f'<{tag}>{item}</{tag}>' for item in items) + r'</tr>'

@dataclass(init=True, frozen=True)
class TestResult():
    fixture: str
    name: str
    duration: float
    outcome: str
    expected_rate: float
    actual_rate: float

    @staticmethod
    def paint_success_bar(actual_rate, min, max=None):
        if max:
            expected = f'<div>🏹 Exp: {min*100:.1f}&#8201;&#37;&hellip;{max*100:.1f}&#8201;&#37;</div>'
        else:
            expected = f'<div>🏹 Exp: {min*100:.1f}&#8201;&#37;</div>'
            max = 1.0

        success = actual_rate >= min and actual_rate <= max
        background_class = "bar_success" if success else "bar_failure"
        cursor_class = "cursor_success" if success else "cursor_failure"

        return (
            f'{expected}'
            f'<div>🎯 Act: {actual_rate*100:.1f}&#8201;&#37;</div>'
            f'<div class="{background_class}"></div>'
            f'<div class="acceptance" style="margin-left:{min*10:.1f}em; width:{(max-min)*10:.1f}em"></div>'
            f'<div class="{cursor_class}" style="margin-left:{actual_rate*10:.1f}em"></div>'
            )

    def _parse_success_rate(self):
        if self.actual_rate is None:
            return 'N/A'
        if isinstance(self.expected_rate, tuple):
            min, max = self.expected_rate
            return self.paint_success_bar(self.actual_rate, min, max)
        return self.paint_success_bar(self.actual_rate, self.expected_rate)

    def get_row(self):
        return to_table_row((
            self.fixture,
            self.name,
            Outcome.decorate(self.outcome),
            f'{self.duration:.2f}&#8201;s',
            self._parse_success_rate())
        )


TEMPLATE = r'''
<meta charset="UTF-8">
<html>
    <head>
        <title>🧪 {file_name}</title>
        <link rel="stylesheet" type="text/css" href="report.css"/ >
    </head>
    <body>
        <h1>pyOpenPASS EndToEnd Test Framework</h1>
        <h2>{file_name}</h2>

        <div class="generated">Generated: {generated}</div>

        <table class="results">
            <thead>
                {header}
            </thead>
            <tbody>
                {body}
            </tbody>
        </table>
    </body>
</html>
'''

class File:
    def __init__(self):
        self.reports = list()
        self.body_rows = list()
        self.collection_failed = False

class Report:
    def __init__(self, output_path):
        self.output_path = Path(output_path)
        self.header = to_table_row(["Test", "Config", "Result", "Duration", "Success Rate"], header=True)
        self.files = dict()

    @staticmethod
    def _table_row(items, header=False):
        tag = 'th' if header else 'td'
        return r'<tr>' + ''.join(f'<{tag}>{item}</{tag}>' for item in items) + r'</tr>'

    def _generate_report(self, file_name, file: File):
        generated = datetime.datetime.now().strftime("%d.%m.%Y, %H:%M:%S")

        return TEMPLATE.format(generated=generated,
                               file_name=file_name,
                               header=self.header,
                               body='\n'.join(file.body_rows))

    def _save_report(self, file_name: Path, report_content: str):
        Path(self.output_path).mkdir(parents=True, exist_ok=True)
        dir_util.copy_tree(Path(__file__).parent.resolve() / 'resources/html', f'{self.output_path}')
        with open(f'{self.output_path}/{Path(file_name).stem}.html', 'w', encoding="utf-8") as report_file:
            report_file.write(report_content)

    @staticmethod
    def parse_report(test_report, test_item: TestItem, result):
        test_result = TestResult(
            fixture=test_item.nodeid.split('::')[1],
            name="::".join(test_item.nodeid.split('::')[2:]),
            duration=test_report.duration,
            outcome=test_report.outcome,
            expected_rate=test_item.success_rate,
            actual_rate = result.analysis.get('success_rate', None) if result.analysis else None
            )

        return test_result.get_row()

    def _post_process_reports(self):
        for file in self.files.values():
            for report in file.reports:
                for extra in getattr(report, 'extra', []):
                    if isinstance(getattr(extra, 'test_item', None), TestItem):
                        if getattr(report, 'when', '') == 'call':
                            file.body_rows.append(self.parse_report(report, extra.test_item, extra.results))

    def pytest_runtest_logreport(self, report):
        file_name = Path(report.fspath).name
        file = self.files.get(file_name, File())
        file.reports.append(report)
        self.files[file_name] = file

    def pytest_collectreport(self, report):
        if report.failed:
            if getattr(report, "when", '') == 'collect':
                file_name = Path(report.fspath).name
                file = self.files.get(file_name, File())
                file.collection_failed = True
                file.body_rows.append(f'<tr><td colspan="2">{file_name}</td><td>collect</td><td>{Outcome.decorate(report.outcome)}</td></tr>')
                self.files[file_name] = file

    def pytest_sessionstart(self):
        self.suite_start_time = time.time()

    def pytest_sessionfinish(self):
        self._post_process_reports()
        for file_name, file in self.files.items():
            report_content = self._generate_report(file_name, file)
            if len(file.body_rows):
                self._save_report(file_name, report_content)

    def pytest_deselected(self, items):
        if not items: return

        for item in items:
            file_name = item.fspath.basename
            fixture = item.test_item.nodeid.split('::')[1]
            if fixture.lower().startswith('disabled_'):
                fixture = fixture[len('disabled_'):]

            file = self.files.get(file_name, File())
            test_result = TestResult(
                fixture,
                "::".join(item.test_item.nodeid.split('::')[2:]),
                0,
                "disabled",
                None,
                None
            )

            file.body_rows.append(test_result.get_row())
            self.files[file_name] = file

    def pytest_terminal_summary(self, terminalreporter):
        for file_name, file in self.files.items():
            if len(file.body_rows):
                terminalreporter.line(f"generated report: file://{self.output_path}/{Path(file_name).stem}.html", yellow=True, bold=True)
