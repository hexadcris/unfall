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

import { createTheme } from '@mui/material/styles';

// Change Color palet typescript
declare module '@mui/material/styles' {
    interface PaletteColor {
        accent1?: string;
        accent1Medium?: string;
        accent1Soft?: string;
        accent2?: string;
        accent2Soft?: string;
        accent3?: string;
        accent3Soft?: string;
        medium?: string;
        mainContrast?: string;
    }

    interface Palette {
        greyScale: Palette['primary'];
    }

    interface PaletteOptions {
        greyScale: PaletteOptions['primary'];
    }

    interface SimplePaletteColorOptions {
        accent1?: string;
        accent1Medium?: string;
        accent1Soft?: string;
        accent2?: string;
        accent2Soft?: string;
        accent3?: string;
        accent3Soft?: string;
        medium?: string;
        mainContrast?: string;
    }
}

export const theme = createTheme({
    spacing: (factor: number) => `${0.25 * factor}rem`,
    palette: {
        mode: 'light',
        primary: {
            main: '#1D2D53',
            contrastText: '#FFFFFF',
            mainContrast: '#53431D',
            light: '#DEE5EF',
            dark: '#173057',
            accent1: '#ADB9CE',
            accent2: '#FFF5CC',
            accent3: '#878787',
            accent1Medium: '#E8EDF6',
            accent1Soft: '#ADB9CE',
            accent2Soft: '#FFF5CC20',
            accent3Soft: '#87878720'
        },
        secondary: {
            main: '#000',
            contrastText: '#FFFFFF'
        },
        warning: {
            main: '#ED6C02',
            dark: '#E65100',
            light: '#F5EBE3'
        },
        success: {
            main: '#2E7D32',
            dark: '#1B5E20',
            light: '#E3EFE4'
        },
        error: {
            main: '#D32F2F',
            dark: '#C62828',
            light: '#F3DBDB'
        },
        greyScale: {
            main: '#BEBEBE',
            dark: '#808285',
            light: '#F4F4F4',
            medium: '#D7D7D7'
        },
        grey: {
            "300": '#BEBEBE',
            "900": '#808285',
            "100": '#F4F4F4',
            "500": '#D7D7D7',
        }
    },
    typography: {
        fontFamily: 'Roboto',
        htmlFontSize: 20,
    },
    components: {
        MuiStepIcon: {
            styleOverrides: {
                text: {
                    fill: 'black',
                    fontSize: 15
                },
                root: {
                    fill: 'white',
                    border: '1px solid grey',
                    borderRadius: '50%'
                }
            }
        },
        MuiButton: {
            styleOverrides: {
                contained: {
                    '&:hover': {
                        backgroundColor: 'var(--main-contrast)'
                    }
                },
                root: {
                    fontWeight: 'bold'
                }
            }
        },
        MuiCssBaseline: {
            styleOverrides: `
                @font-face {
                  font-family: 'Roboto';
                  font-style: normal;
                  font-weight: 400;
                  src: url('../../fonts/Roboto-Regular.ttf') format("truetype");
                }
                
                @font-face {
                  font-family: 'RobotoLight';
                  font-style: normal;
                  font-weight: 400;
                  src: url('../../fonts/Roboto-Light.ttf') format("truetype");
                }
                
                @font-face {
                  font-family: 'RobotoBold';
                  font-style: normal;
                  font-weight: 700;
                  src: url('../../fonts/Roboto-Bold.ttf') format("truetype");
                }
                
                @font-face {
                  font-family: 'RobotoItalic';
                  font-style: italic;
                  font-weight: 400;
                  src: url('../../fonts/Roboto-Italic.ttf') format("truetype");
                }
            `
        }
    }
});