################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from pathlib import Path
import os

def get_logs(artifact_path):
    logs = dict()
    for log_file in Path(artifact_path).glob('**/*.log'):
        if os.path.getsize(log_file) == 0:
            logs[log_file.name] = '<EMPTY>'
        else:
            with open(log_file, 'r') as l:
                logs[log_file.name] = l.read().strip()
    return logs