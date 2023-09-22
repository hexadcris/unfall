################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import json
from pathlib import Path
from typing import Union, List

from plugin_management.plugin_manager import Pluginmanager

class TestCase:
    def __init__(self, name: str, children):
        self.name = name
        self.children = children

def _check_rate(rate):
    if not isinstance(rate, int) and not isinstance(rate, float):
        raise Exception(
            f"Values must be numeric")
    if rate < 0 or rate > 1:
        raise Exception(
            f"Values must be between 0 and 1")
    return float(rate)

def _check_boundaries(success_rate):
    try:
        min, max = success_rate
    except:
        raise Exception(f"Must contain a maximum of 2 elements")

    min, max = _check_rate(min), _check_rate(max)

    if min > max:
        raise Exception(f"Minimum exceeds maximum")

    return (min, max)

def _expand_success_rate(test_parameter):
    if not 'success_rate' in test_parameter:
        return None

    success_rate = test_parameter['success_rate']
    try:
        if isinstance(success_rate, list):
            return _check_boundaries(success_rate)
        return _check_rate(success_rate)
    except Exception as exc:
        raise Exception(f'Error parsing "success_rate = {success_rate}": {exc}')


class TestItem:
    def __init__(self, name: str, config: str, param: dict, plugin=None, index=0, skip_setattr: Union[List[str], str] = None):
        self.name = name
        self.config = config
        self.random_seed_offset = 0
        self.plugin = plugin
        self.index = index
        self.param = param
        self.success_rate = _expand_success_rate(param)
        self.__expand_parameter(param, skip_setattr)

    @property
    def id(self):
        return f'[{self.invocations}x{self.duration}s@r{self.random_seed_offset}' + (f':{self.name}' if self.plugin else '') + ']'

    def __expand_parameter(self, param, skip_setattr: Union[List[str], str] = None):
        if not skip_setattr:
            skip_setattr = list()
        if not isinstance(skip_setattr, list):
            skip_setattr = [skip_setattr]
        skip_setattr.extend(['config', 'config_sets', 'success_rate'])

        for key, value in param.items():
            if key not in skip_setattr:
                setattr(self, key, value)

class TestCollector:
    class NO_SCOPE():
        pass

    declared_config_sets = []

    def __init__(self, name, param) -> None:
        self.name = name
        self.param = param
        self.configs = self._get_configs(param)
        self.plugins = self._get_plugins(param, Pluginmanager.get())

    def _lookup(self, config_set):
        try:
            return [config for config in TestCollector.declared_config_sets[config_set]]
        except KeyError as key:
            raise Exception(
                f"Error parsing '{self.name}': {key} not declared in 'config_sets'")
        except TypeError:
            raise Exception(
                f"Error parsing '{self.name}': Missing global declaration of 'config_sets'")

    def _get_configs(self, param):
        if 'config' in param:
            return {TestCollector.NO_SCOPE: param['config']}
        elif 'config_sets' in param:
            return {config_set: self._lookup(config_set) for config_set in param['config_sets']}
        else:
            raise Exception(
                f"Error parsing '{self.name}': Neighter 'config' nor 'config_sets' declared")

    def _get_plugins(self, param, variant_plugins):
        plugins = list()
        for plugin in variant_plugins:
            if plugin.applies(param):
                plugins.append(plugin)
        return plugins

    @classmethod
    def read_config_sets(cls, test_config):
        cls.declared_config_sets = test_config.get('config_sets', [])

    def _build_from_config(self, config):
        if self.plugins:
            test_items = []
            for plugin in self.plugins:
                for i in range(plugin.nr_of_variations(self.param)):
                    test_items.append(TestItem(f'{plugin.variation.prefix}{i:02d}', config, self.param, plugin=plugin, index=i))
            return TestCase(config, test_items)
        return TestItem(config, config, self.param, plugin=False)

    def _build_from_scope(self, scope, configs):
        return TestCase(scope, [self._build_from_config(c) for c in configs])

    def build(self):
        children = []
        for scope, configs in self.configs.items():
            if scope == TestCollector.NO_SCOPE:
                children = self._build_from_config(configs)
            else:
                children.append(self._build_from_scope(scope, configs))
        return TestCase(self.name, children)


def get_test_cases(test_config: dict):
    TestCollector.read_config_sets(test_config)
    tests = [TestCollector(name, param)
             for name, param in test_config['tests'].items()]
    return [t.build() for t in tests]


def read_config(test_file):
    test_file = Path(test_file)
    if not test_file.exists():
        raise Exception(f'File does not exist')
    with open(test_file) as t:
        return json.load(t)
