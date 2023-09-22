################################################################################
# Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import pytest

from config_parser import get_test_cases


def test_using_unknown_config_set__raises_error():
    ERR_NONEXISTING_CONFIG_SET_DEFINITION = {
        "config_sets": {
            "first_config_set": {"config_0"},
        },
        "tests": {
            "first_test_case_name": {
                "config_sets": [
                    "first_config_set",
                    "non_existing_config_set"
                ],
                "duration": 12,
                "invocations": 34
            }
        }
    }
    with pytest.raises(Exception) as exc:
        get_test_cases(ERR_NONEXISTING_CONFIG_SET_DEFINITION)
    assert 'non_existing_config_set' in str(exc.value)


def test_missing_declaration_of_config_sets__raises_error():
    ERR_UNDECLARED_CONFIG_SET_DEFINITION = {
        "tests": {
            "first_test_case_name": {
                "config_sets": [
                    "using_config_sets_but_forgot_to_declare_at_global_level"
                ],
                "duration": 12,
                "invocations": 34
            }
        }
    }
    with pytest.raises(Exception) as exc:
        get_test_cases(ERR_UNDECLARED_CONFIG_SET_DEFINITION)
    assert 'config_sets' in str(exc.value)


def test_no_config_at_all__raises_error():
    ERR_NO_CONFIG_DEFINITION = {
        "tests": {
            "first_test_case_name": {
                "duration": 12,
                "invocations": 34
            }
        }
    }
    with pytest.raises(Exception) as exc:
        get_test_cases(ERR_NO_CONFIG_DEFINITION)
    assert "'config'" in str(exc.value)
    assert "'config_sets'" in str(exc.value)


def test_given_single_config__creates_single_test():
    SINGLE_CONFIG_DEFINITION = {
        "tests": {
            "first_test_case_name": {
                "config": "the_config",
                "duration": 12,
                "invocations": 34
            }
        }
    }
    test_cases = get_test_cases(SINGLE_CONFIG_DEFINITION)
    assert len(test_cases) == 1
    assert test_cases[0].children.name == "the_config"


def test_given_config_sets__expands_config_sets_as_intermediate_node():
    CONFIG_SET_DEFINITION = {
        "config_sets": {
            "first_config_set": {"config_0", "config_1"},
            "second_config_set": {"config_2"},
            "third_config_set": {"config_3", "config_4", "config_5"},
        },
        "tests": {
            "first_test_case_name": {
                "config_sets": [
                    "first_config_set",
                    "second_config_set"
                ],
                "duration": 12,
                "invocations": 34
            },
            "second_test_case_name": {
                "config_sets": [
                    "third_config_set"
                ],
                "duration": 56,
                "invocations": 78
            }
        }
    }
    test_cases = get_test_cases(CONFIG_SET_DEFINITION)
    assert len(test_cases) == 2
    assert len(test_cases[0].children) == 2            # 2 config sets
    assert len(test_cases[0].children[0].children) == 2  # 2 configs
    assert len(test_cases[0].children[1].children) == 1  # 1 configs
    assert len(test_cases[1].children) == 1            # 1 config set
    assert len(test_cases[1].children[0].children) == 3  # 3 configs


def test_given_parameterized_config__creates_one_test_per_variation():
    VARIATION_DEFINITION = {
        "config_sets": {"some_config_set": {"a_config"}},
        "tests": {
            "parameterized_test_case_name": {
                "config_sets": [
                    "some_config_set"
                ],
                "duration": 12,
                "invocations": 34,
                "parameterization": {
                    "file": "some_file.xml",
                    "xpath": "some_xpath",
                    "values": [
                        "first_variation",
                        "second_variation"
                    ]
                }
            }
        }
    }
    test_cases = get_test_cases(VARIATION_DEFINITION)
    assert len(test_cases) == 1
    assert len(test_cases[0].children) == 1                      # 1 config set
    assert len(test_cases[0].children[0].children) == 1            # 1 config
    assert len(test_cases[0].children[0].children[0].children) == 2  # 2 variations
