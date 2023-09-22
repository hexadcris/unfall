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

import { Box, Step, Stepper, useTheme } from "@mui/material";
import { StepperLine, StyledStepLabel, TabStyled } from "./navTabs.styles";
import SettingsIcon from '@mui/icons-material/Settings';
import HomeIcon from '@mui/icons-material/Home';
import { ColorVariant, PATHS, Variant, tabNames } from "../../index.d";
import { useNavigate, useLocation } from "react-router-dom";
import { useEffect } from "react";
import { LightTooltip } from "../../styles/general.styles";
import { useGlobalContext } from "../../context/ContextProvider";
import WarningAmberIcon from '@mui/icons-material/WarningAmber';

const NavTabs = () => {
    const theme = useTheme();
    const { selectedTab, setSelectedTab, setModal, modal, showDiscardModal, setShowDiscardModal } = useGlobalContext();
    const navigation = useNavigate();
    const location = useLocation();
    const homeTab = tabNames.find((tab) => tab.pathTo === PATHS.HOME)
    const navigate = (path: PATHS) => {
        if (showDiscardModal === true) {
            setModal({
                ...modal,
                active: true,
                title: "Discard Changes And Navigate",
                descriptionTitle: "All Your Changes Will Be Discarted.",
                description: "If you proceed with this action, any unsaved changes will be discarted",
                icon: WarningAmberIcon,
                iconVariant: ColorVariant.warning,
                buttons: [
                    {
                        title: 'Cancel',
                        variant: Variant.outlined
                    },
                    {
                        title: 'Discard and Navigate',
                        action: () => {
                            setSelectedTab(path);
                            navigation(path);
                            setShowDiscardModal(false);
                        }
                    }
                ]
            });
            return;
        }
        setSelectedTab(path);
        navigation(path);
    }

    useEffect(() => {
        setSelectedTab(location.pathname as PATHS);
    }, [])

    return (
        <Box className='sidenav' gap={theme.spacing(25)}>
            <Box display='flex' flexDirection='column' paddingTop={5}>
                {
                    <TabStyled
                        key={homeTab?.name}
                        data-testid={`test-id-nav-${homeTab?.name}`}
                        className={selectedTab === homeTab?.pathTo ? 'Mui-selected' : ''}
                        label={homeTab?.name}
                        value={homeTab?.pathTo}
                        disabled={false}
                        onClick={() => navigate(homeTab!.pathTo)}
                        icon={<HomeIcon />}
                        iconPosition={'start'}
                        sx={{ marginBottom: 5, alignItems: 'center', height: theme.spacing(8), minHeight: theme.spacing(8), paddingLeft: theme.spacing(3) }}
                    />
                }
                <Stepper orientation="vertical" color="red" connector={<StepperLine />} sx={{ marginLeft: theme.spacing(4) }}>
                    {tabNames.filter((tab) => (tab.pathTo !== PATHS.GLOBAL_SETUP && tab.pathTo !== PATHS.HOME)).map((tab) => (
                        <Step active={true} key={tab.name}>
                            <StyledStepLabel>
                                <LightTooltip
                                    PopperProps={{
                                        modifiers: [
                                            {
                                                name: "offset",
                                                options: {
                                                    offset: [tab.name === 'Pre' ? 80 : 15, -5],
                                                },
                                            },
                                        ],
                                    }}
                                    title={tab.tooltipText}
                                >
                                    <TabStyled
                                        key={tab.name}
                                        data-testid={`test-id-nav-${tab.name}`}
                                        className={selectedTab === tab.pathTo ? 'Mui-selected' : ''}
                                        label={tab.name}
                                        value={tab.pathTo}
                                        disabled={false}
                                        onClick={() => navigate(tab.pathTo)}
                                        icon={tab.name === 'Homepage' ? <HomeIcon /> : ''}
                                        iconPosition={'start'}
                                        sx={{ height: theme.spacing(8), minHeight: theme.spacing(8), marginLeft: 0, marginBottom: 0, alignItems: 'center', width: '95%' }}
                                    />
                                </LightTooltip>
                            </StyledStepLabel>
                        </Step>

                    ))
                    }
                </Stepper>
            </Box>
            <Box className='sidenav__docs'>
                <TabStyled
                    data-testid={`test-id-nav-openpass`}
                    label={'OpenPASS Website'}
                    disabled={false}
                    onClick={() => window.open('https://openpass.eclipse.org/', '_blank')}
                    icon={<i className="icon-mask white location-exit"></i>}
                    iconPosition={'start'}
                    sx={{ alignItems: 'center', height: theme.spacing(8), minHeight: theme.spacing(8), marginRight: theme.spacing(6), width: '100%' }}
                />
                <TabStyled
                    data-testid={`test-id-nav-documentations`}
                    label={'Documentation'}
                    disabled={false}
                    onClick={() => window.open('https://www.eclipse.org/openpass/content/html/index.html', '_blank')}
                    icon={<i className="icon-mask white file-cog"></i>}
                    iconPosition={'start'}
                    sx={{ alignItems: 'center', height: theme.spacing(8), minHeight: theme.spacing(8), marginRight: theme.spacing(6), width: '100%' }}
                />
            </Box>
            <Box className='sidenav__config' >
                <TabStyled
                    label={'Global Setup'}
                    onClick={() => navigate(PATHS.GLOBAL_SETUP)}
                    className={selectedTab === PATHS.GLOBAL_SETUP ? 'Mui-selected' : ''}
                    icon={<SettingsIcon />}
                    iconPosition={'start'}
                    sx={{
                        alignItems: 'center',
                        color: theme.palette.primary.main,
                        height: theme.spacing(8),
                        minHeight: theme.spacing(8),
                        marginRight: theme.spacing(6),
                        width: '95%',
                        '&:hover': {
                            border: '1px solid',
                            borderColor: theme.palette.primary.main
                        },
                        '&.Mui-selected:hover': {
                            background: theme.palette.primary.accent1,
                            color: theme.palette.common.white,
                            borderRadius: 2,
                            border: '1px solid',
                            borderColor: theme.palette.primary.accent1
                        },
                    }}
                />
            </Box>
        </Box>
    );
}

export default NavTabs;
