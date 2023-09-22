################################################################################
# Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

class Pluginmanager():
    registered_plugins = list()

    @classmethod
    def register(cls, plugin):
        cls.registered_plugins.append(plugin)
        print(f'regististered plugin {plugin.variation}')

    @classmethod
    def get(cls):
        return cls.registered_plugins
