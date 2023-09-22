################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import pytest

from pyopenpass import PyOpenPass, Reporter, addoption

def pytest_addoption(parser):
    addoption(parser)

def pytest_configure(config: pytest):
    config._pyopenpass = PyOpenPass(config)
    config.pluginmanager.register(config._pyopenpass, name="PyOpenPASS")
    config._pyopenpass_reporter = Reporter(config)
    config.pluginmanager.register(config._pyopenpass_reporter, name="PyOpenPASSReporter")

def pytest_unconfigure(config):
    pyopenpass = getattr(config, "_pyopenpass", None)
    if pyopenpass:
        del config._pyopenpass
        config.pluginmanager.unregister(pyopenpass)

    pyopenpass_reporter = getattr(config, "_pyopenpass_reporter", None)
    if pyopenpass_reporter:
        del config._pyopenpass_reporter
        config.pluginmanager.unregister(pyopenpass_reporter)

@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    rep = outcome.get_result()
    Reporter.make_report(item, rep)
