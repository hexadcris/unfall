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

import { StepConnector, StepLabel, Tab, stepConnectorClasses, styled } from "@mui/material";
export const TabStyled = styled(Tab)(({ theme }) => ({
    justifyContent: 'flex-start',
    minHeight: theme.spacing(10),
    height: theme.spacing(10),
    marginBottom: theme.spacing(2),
    marginLeft: theme.spacing(2),
    marginRight: theme.spacing(2),
    color: theme.palette.common.white,
    cursor: 'pointer',
    fontWeight: 'bold',
    opacity: 1,
    '&:hover': {
        border: '1px solid',
        borderColor: theme.palette.primary.accent1
    },
    '&:hover::after': {
        position: 'absolute',
        content: "'ᐳ'",
        right: 20
    },
    '&.Mui-selected': {
        background: theme.palette.common.white,
        color: theme.palette.common.black,
        borderRadius: 2
    },
    '&.Mui-selected:hover': {
        background: theme.palette.primary.main,
        color: theme.palette.common.white,
        borderRadius: 2,
        border: '1px solid',
        borderColor: theme.palette.primary.accent1
    },
    '&.Mui-selected::after': {
        position: 'absolute',
        content: "'ᐳ'",
        right: 20
    }
}));

export const StepperLine = styled(StepConnector)(({ }) => ({
    [`&.${stepConnectorClasses.alternativeLabel}`]: {
    },
    [`& .${stepConnectorClasses.line}`]: {
        borderLeft: "1px dotted",
    }
}));

export const StyledStepLabel = styled(StepLabel)(({ theme }) => ({
    padding: 0,
    marginLeft: theme.spacing(.6)
}));