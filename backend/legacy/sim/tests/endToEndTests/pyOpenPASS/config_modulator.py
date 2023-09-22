################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from config_parser import TestItem
from xml_util import XmlUtil

class ConfigModulator:
    @staticmethod
    def apply(test_item: TestItem, config_path):
        update_values = {'invocations': test_item.invocations,
                         'duration': test_item.duration}
        if test_item.random_seed_offset != 0:
            update_values['random_seed'] = \
                XmlUtil.get(config_path, XmlUtil.CONFIG_RANDOM_SEED) + test_item.random_seed_offset
        XmlUtil.update(config_path, update_values)

        if test_item.plugin:
            test_item.plugin.apply(config_path, test_item.param, test_item.index)
