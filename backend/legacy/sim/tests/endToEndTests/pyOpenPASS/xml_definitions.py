################################################################################
# Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from dataclasses import dataclass


@dataclass(frozen=True)
class XmlNode:
    xml_file: str
    xpath_expr: str
    datatype: object = None


@dataclass(frozen=True)
class XmlSetter:
    dest: XmlNode
    value: str = None