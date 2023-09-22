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

import logging
import sys
import os.path
from lxml import etree
from xml_definitions import XmlNode, XmlSetter

def terminate_program(message):
    logging.error(message)
    sys.exit(message)


def get_file_or_default(config_under_test, file):
    """If avilable, returns the file from the current config path, else from the default path"""
    in_config_folder = os.path.join(
        config_under_test.base_path, config_under_test.name, file)
    if os.path.isfile(in_config_folder):
        return in_config_folder

    in_default_folder = os.path.join(config_under_test.default_path, file)
    if os.path.isfile(in_default_folder):
        return in_default_folder

    terminate_program(
        f'No file "{file}" in "{config_under_test.base_path}" or "{config_under_test.default_path}"')


def set(base_path, xml_setter: XmlSetter):
    """ Sets the node given by the (xml_file, xpath_expr) to value """
    (expr, tree, node) = get_node(base_path, xml_setter.dest)
    value = str(xml_setter.value).replace('${configFolder}', str(base_path))

    if len(expr) == 1:
        node.text = value
    else:
        node.attrib[expr[1]] = value

    xml_file = os.path.join(base_path, xml_setter.dest.xml_file)
    with open(xml_file, 'w') as f:
        f.write(etree.tostring(tree, encoding="unicode", pretty_print=True))


def get_node(base_path, xml_node: XmlNode):
    """ Gets the node for further processing """
    xml_file = os.path.join(base_path, xml_node.xml_file)

    tree = etree.parse(xml_file)
    expr = xml_node.xpath_expr.split('/@')
    try:
        node = tree.xpath(expr[0])[0]
    except IndexError:
        terminate_program(
            f'Unable to execute xpath expression "{xml_node.xpath_expr}" on "{xml_file}"')

    return (expr, tree, node)


class XmlUtil:
    """
    Utilily for xml manipulation w.r.t. openPASS configurations
    Also defines some manipulation node presets starting with 'CONFIG_*'
    """
    CONFIG_RANDOM_SEED = XmlNode(
        'simulationConfig.xml', '//Experiment/RandomSeed', int)
    CONFIG_NUMBER_OF_INVOCATIONS = XmlNode(
        'simulationConfig.xml', '//Experiment/NumberOfInvocations', int)
    CONFIG_LOGGING_TO_CSV = XmlNode(
        'simulationConfig.xml', "//Bool[@Key='LoggingCyclicsToCsv']/@Value", int)
    CONFIG_DURATION = XmlNode(
        'Scenario.xosc', "//StopTrigger/ConditionGroup/Condition[@name='EndTime']/ByValueCondition/SimulationTimeCondition/@value", int)

    @staticmethod
    def get(config_under_test, xml_node: XmlNode):
        """"Retrieves the value of the node given by an XPATH expression"""
        expr, _, node = get_node(config_under_test, xml_node)
        datatype = str if xml_node.datatype is None else xml_node.datatype
        return datatype(node.text) if len(expr) == 1 else datatype(node.attrib[expr[1]])

    @staticmethod
    def update(configs_path, config: dict):
        """Updates configuration files to log into CSV and set the values defined by the given dictionary

        Keyword arguments:
        configs_path -- Path to config set, which shall be manipulated
        config       -- dictionary for updating the values
                        Mandatory:  invocations, duration
                        Optional:   random seed
        """
        # default values
        set(configs_path,
            XmlSetter(XmlUtil.CONFIG_LOGGING_TO_CSV, 'true'))

        # parameterized fields
        set(configs_path,
            XmlSetter(XmlUtil.CONFIG_NUMBER_OF_INVOCATIONS, config["invocations"]))
        set(configs_path,
            XmlSetter(XmlUtil.CONFIG_DURATION, config["duration"]))

        # optional fields
        if 'random_seed' in config:
            set(configs_path,
                XmlSetter(XmlUtil.CONFIG_RANDOM_SEED, config['random_seed']))

    @staticmethod
    def custom_update(configs_path, xml_setter: XmlSetter):
        """Updates a single configuration files based on the given xml_setter

        Keyword arguments:
        configs_path -- Path to config set, which shall be manipulated
        xml_setter   -- A valid xml setter
        """
        set(configs_path, xml_setter)
