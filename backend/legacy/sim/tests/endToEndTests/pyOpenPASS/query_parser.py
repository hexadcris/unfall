################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#               2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import re
from event_parser import normalize, normalize_column


class Query:
    class Pd:
        def __init__(self, parsed_query):
            match = re.match(
                r'([a-z]+)\s?\(\s?([A-Za-z0-9-]+)\s?(\|\s?(.*))?\)\s?([~><=!]=?)\s?([0-9\.]+)', parsed_query)

            self.group = match.group(1)
            self.column = match.group(2)
            self.filter, self.iloc = self._parse_filter(
                match.group(4)) if match.group(3) else (None, None)
            self.operator = match.group(5)
            self.value = match.group(6)

        @staticmethod
        def is_numeric(value):
            try:
                _ = float(value)
                return True
            except:
                return False

        def _expand_approximate(self, filter):
            match = re.search(r'([^ )(]+)\s*~=\s*([^ )(]+)', filter)
            if match:
                lhs, rhs = match[1], match[2]
                if self.is_numeric(lhs):
                    lhs, rhs = (rhs, lhs)
                if self.is_numeric(lhs):
                    raise Exception(
                        f"Unable to expand '{match[0]}': Both sides are numeric (maybe you forgot to quote a column name?)")
                rhs = float(rhs)
                delta = abs(rhs * 1e-3 if rhs != 0 else 1e-3)
                return filter.replace(match[0], f'({lhs} > {(rhs - delta):.6E} and {lhs} < {(rhs + delta):.6E})')
            return filter

        def _quote_columns_containing_dashes(self, filter):
            matches = re.findall(
                r'([a-zA-Z0-9_]*(?:-+[a-zA-Z0-9_]*)+[a-zA-Z0-9_]*)', filter)
            for match in matches:
                if not self.is_numeric(match):
                    filter = re.sub(match, f'`{match}`', filter)
            return filter

        def _parse_filter(self, filter):
            iloc = None
            match = re.search(
                r'(Timestep\s*==\s*{\s*first\s*})\s*([^\s]*)\s*', filter)
            if match:
                if not filter.startswith(match[1]):
                    raise Exception(
                        "'Timestep == {first}' must be the first parameter in the filter")
                if match[2].lower() != 'and':
                    raise Exception(
                        "'Timestep == {first}' must succeeded by 'and'")
                filter = filter[len(match[0]):]
                iloc = 0
            filter = self._expand_approximate(filter)
            filter = self._quote_columns_containing_dashes(filter)
            return filter, iloc

    def __init__(self, query):
        self.raw_query = query
        self.parsed_query = ''
        self.parsed_events = dict()
        self.shifted_columns = list()
        self.parse(query)
        self.pd = self.Pd(self.parsed_query)

    def parse(self, raw_query):

        query_for_events = r'#\(([A-Za-z0-9_ \/]+)\)(?:\:([^\s]+))?(?:.*?#\(\1\)\:([^\s]+))*?'
        matches = re.findall(query_for_events, raw_query)

        for key, value, _ in matches:
            if len(value):
                split = re.search(r'\s?([=<>!]|no|is|contains)', value)
                if split is not None:
                    value = value[:split.start(1)]

                try:
                    self.parsed_events[key].add(value)
                except KeyError:
                    self.parsed_events[key] = {value}
            else:
                self.parsed_events[key] = set()

        for event, parameters in self.parsed_events.items():
            for parameter in parameters:
                raw_query = raw_query.replace(
                    f'#({event}):{parameter}', f'Event_{normalize_column(parameter, event)}')
            raw_query = raw_query.replace(
                f'#({event})', f'Event_{normalize(event)}')

        matches = set(re.findall(
            r"\|.*?(((?=[^`])[A-Za-z\d_-]+)-(\d+)(?=[^`]))", raw_query))

        if matches:
            for match, column, shift in matches:
                new_column = f'{column}_prev{shift}'
                self.shifted_columns.append((new_column, column, int(shift)))
                raw_query = raw_query.replace(match, new_column)

        self.parsed_query = raw_query

    @property
    def parsed(self):
        return self.parsed_query

    @property
    def events(self):
        return self.parsed_events.keys()
