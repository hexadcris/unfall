/*
 * Copyright (c) 2023 Hexad GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 */

import { styled } from '@mui/material/styles';
import { Input } from '@mui/material';

export const InputStyled = styled(Input)({
    background: '#fff',
    marginTop: '5px',
    padding: 0,
    paddingLeft: '5px',
    width: '40px',
    height: '30px',
    '&::before': {
        all: 'unset'
    },
    border: '1px solid #000'
});