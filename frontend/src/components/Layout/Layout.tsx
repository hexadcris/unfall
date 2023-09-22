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

import { Box, Typography } from '@mui/material';
import { Outlet, useLocation } from 'react-router-dom';
import NavTabs from '../NavTabs/NavTabs';
import { tabNames } from '../../index.d';
import { useGlobalContext } from '../../context/ContextProvider';
import logo from '../../assets/logo_openpass.png';

const Layout = () => {
    const location = useLocation();
    const { sideWindowRef } = useGlobalContext();
    const pageTitle = tabNames.find(tab => tab.pathTo === location.pathname);
    return (
        <div id="root-content">
            <nav className='nav'>
                <img id="gui-logo"
                    src={logo}
                    alt="OpenPASS logo"
                />
                <Typography component='h5' variant='h5' sx={{ textTransform: 'uppercase', flex: 1, textAlign: 'center', marginRight: 40 }}>{pageTitle?.tooltipText || pageTitle?.name}</Typography>
            </nav>
            <div id="gui-sidebar">
                <NavTabs />
            </div>
            <Box ref={sideWindowRef} className='side-window'>
                <Outlet />
            </Box>
        </div>
    )
}

export default Layout;
