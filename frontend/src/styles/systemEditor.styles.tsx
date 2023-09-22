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
import { Box, Button, TextField } from '@mui/material';

export const StepperButton = styled(Button)(({ theme }) => ({
    borderRadius: '50%',
    width: theme.spacing(7),
    height: theme.spacing(7),
    minWidth: theme.spacing(7)
}));

export const HideButton = styled(Box)(({ theme }) => ({
    position: 'absolute',
    zIndex: 2,
    right: '20%',
    background: theme.palette.primary.main,
    width: theme.spacing(7),
    height: theme.spacing(7),
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    top: theme.spacing(8),
    cursor: 'pointer'
}));

export const DraggableSideItem = styled(Box)(({ }) => ({
    '&:hover': {
        'h6': {
            textDecoration: 'underline',
            cursor: 'pointer'
        }
    }
}));

export const AddIconBox = styled(Box)(({ theme }) => ({
    zIndex: 1,
    cursor: 'pointer',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    marginLeft: theme.spacing(4),
    background: theme.palette.primary.main,
    borderRadius: '50%',
    width: theme.spacing(7),
    height: theme.spacing(7),
    minWidth: theme.spacing(7),
    'svg': {
        color: theme.palette.common.white
    },
    '&:hover': {
        background: 'transparent',
        'svg': {
            color: theme.palette.primary.mainContrast
        }
    }
}));

export const ColorBox = styled(Box)(({ theme }) => ({
    zIndex: 1,
    cursor: 'pointer',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    borderRadius: '50%',
    marginRight: theme.spacing(1),
    width: theme.spacing(4),
    height: theme.spacing(4),
    minWidth: theme.spacing(4)
}));

export const NumberInput = styled(TextField)(({ theme }) => ({
    background: '#fff',
    outline: 'none !important',
    marginTop: '5px',
    padding: 0,
    // paddingLeft: '5px',
    width: '40px',
    height: '30px',
    '&::before': {
        all: 'unset'
    },
    '&.Mui-focused': {
        outline: 'none !important'
    },
    'input': {
        paddingTop: theme.spacing(2),
        paddingBottom: theme.spacing(2),
        paddingLeft: 10,
        paddingRight: 5
    },
    borderRadius: theme.spacing(1)
}));