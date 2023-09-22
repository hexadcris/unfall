################################################################################
# Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

def pytest_pyopenpass_simulate_finished(simulation_result):
    pass

def pytest_pyopenpass_analyse(analysis_result):
    """
    Called before analyse is called

    Idea: can capture an analysis of a certain type
    """
    pass