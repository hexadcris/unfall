################################################################################
# Copyright (c) 2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from importlib import import_module
from pathlib import Path
__all__ = [
    import_module(f".{f.stem}", __package__)
    for f in Path(__file__).parent.glob("*.py")
    if "__" not in f.stem
]
del import_module, Path