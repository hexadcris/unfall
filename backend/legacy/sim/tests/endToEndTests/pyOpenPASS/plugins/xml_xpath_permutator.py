################################################################################
# Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from plugin_management.plugin_manager import Pluginmanager
from plugin_management.hook import VariationHook
from xml_definitions import XmlNode, XmlSetter
from xml_util import XmlUtil


class XmlXpathPermutator():
    variation = VariationHook('Par', 'parameterization', 'values')

    @classmethod
    def applies(cls, param):
        if cls.variation.identifier in param:
            if cls.variation.permutator not in param[cls.variation.identifier]:
                raise Exception(
                    f'"{cls.variation.identifier}" not applicable without list "{cls.variation.permutator}"')
            return True
        return False

    @classmethod
    def nr_of_variations(cls, param):
        return len(param[cls.variation.identifier][cls.variation.permutator])

    @classmethod
    def apply(cls, config_path, param, index):
        if cls.applies(param) == False:
            raise Exception('Invalid plugin')
        if index > cls.nr_of_variations(param):
            raise Exception('Invalid index')
        plugin_data = param[cls.variation.identifier]
        XmlUtil.custom_update(config_path,
                              XmlSetter(XmlNode(plugin_data['file'], plugin_data['xpath']),
                                        plugin_data[cls.variation.permutator][index]))


Pluginmanager.register(XmlXpathPermutator)
