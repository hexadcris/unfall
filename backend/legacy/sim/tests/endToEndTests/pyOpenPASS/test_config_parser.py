################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from config_parser import *

TEST_JSON = {
    'config_sets':
    {
        'DeterminismSet':
            ['SomeConfig'],
        'FirstConfigSet':
            ['FirstConfigSet_C1',
             'FirstConfigSet_C2'
             ],
        'ParameterizeSet':
            ['Parameterize_C1']
    },
    'tests':
    {
        'Determinism':
        {
            'config_sets': ['DeterminismSet',
                            'FirstConfigSet',
                            'ParameterizeSet'],
            'duration': 30,
            'invocations': 10,
            'determinism': True
        },
        'FirstConfigSetTest':
        {
            'config_sets': ['FirstConfigSet'],
            'duration': 45,
            'invocations': 10,
            'description': 'Some Description',
            'queries': [
                'first query',
                'second query'],
            'success_rate': 1.0
        },
        'ParameterTest':
        {
            'config_sets': ['ParameterizeSet'],
            'parameterization': {
                'file': 'someFile.xml',
                'xpath': "//value[../id='ParameterizeMe']",
                'values': ['WithValue1',
                           'WithValue2']},
            'duration': 30,
            'invocations': 1
        }
    }
}


def test_read_config():
    config_sets = read_config(Path('tests.json'))
    assert len(config_sets) != 0


def test_get_unique_configs():
    '''
    tests if configs are combined based on config_name, duration, invocation and parameterization

    pseudo-output
    Determinism:
        'configs': ['SomeConfig', 'FirstConfigSet_C1','FirstConfigSet_C2', 'Parameterize_C1'],
        'duration': 30,
        'invocations': 10,
        'parameters': False

    FirstConfigSetTest:
        'configs': ['FirstConfigSet_C1', 'FirstConfigSet_C2']
        'duration': 45,
        'invocations': 10,
        'parameters': False

    ParameterTest:
        'configs': ['Parameterize_C1'],
        'duration': 30,
        'invocations': 1
        'parameters': True

    => Makes 7 unique configs

    Note: Determinism Single Tests are "subconfigs" of DeterminismTests (see addendum)
    '''
    assert len(get_unique_configs(TEST_JSON)) == 7


def test__get_regular_tests__lists_all_tests():
    tests = get_regular_tests(TEST_JSON)
    assert len(tests) == 3
    assert len(tests['Determinism']) == 4
    assert len(tests['FirstConfigSetTest']) == 2
    assert len(tests['ParameterTest']) == 1


def test__get_determinism_addendum__sets_have_same_size():
    '''
    Determinism Single Tests -> Each invocation (1xn) will trigger a single invocation (nx1)
        Determinism[0..9]
            'configs': ['SomeConfig', 'FirstConfigSet_C1','FirstConfigSet_C2', 'Parameterize_C1'],
            'duration': 30,
            'invocations': 1
            'random_seeds': [Invocation0..Invocation9]
            'parameters': False

    => Makes 10 x 4 additional configs
    '''
    tests = get_determinism_addendum(TEST_JSON)
    assert len(tests) == 10
    assert len(tests['Determinism_00']) == 4
    assert len(tests['Determinism_01']) == 4
    assert len(tests['Determinism_02']) == 4
    assert len(tests['Determinism_03']) == 4
    assert len(tests['Determinism_04']) == 4
    assert len(tests['Determinism_05']) == 4
    assert len(tests['Determinism_06']) == 4
    assert len(tests['Determinism_07']) == 4
    assert len(tests['Determinism_08']) == 4
    assert len(tests['Determinism_09']) == 4

def test__get_determinism_addendum__sets_have_incresing_random_seed():
    tests = get_determinism_addendum(TEST_JSON)
    assert tests['Determinism_00'][0].random_seed_offset == 0
    assert tests['Determinism_01'][0].random_seed_offset == 1
    assert tests['Determinism_02'][0].random_seed_offset == 2
    assert tests['Determinism_03'][0].random_seed_offset == 3
    assert tests['Determinism_04'][0].random_seed_offset == 4
    assert tests['Determinism_05'][0].random_seed_offset == 5
    assert tests['Determinism_06'][0].random_seed_offset == 6
    assert tests['Determinism_07'][0].random_seed_offset == 7
    assert tests['Determinism_08'][0].random_seed_offset == 8
    assert tests['Determinism_09'][0].random_seed_offset == 9
