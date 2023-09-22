################################################################################
# Copyright (c) 2021-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from pathlib import Path
from typing import Union
import hashlib

def generate_hash(folder: Union[str, Path]):
    '''
    Generates a hash over the content of a folder, including
    the names of the files and folders (relative to the given
    folder) and the content of the files.

    Example call: generate_hash(Path('/openPASS/bin/configs'))
    '''
    folder = Path(folder)
    hasher = hashlib.sha1()
    for file in folder.glob('**/*'):
        hasher.update(str(file.relative_to(folder)).encode())
        if file.is_file():
            hasher.update(file.read_bytes())
    return hasher.hexdigest()