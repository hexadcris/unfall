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
import { useMemo } from 'react';
import { createBrowserRouter, createRoutesFromElements, Route, RouterProvider } from 'react-router-dom';
import { appRoutes } from '../../config/routes.config';
import { RouteType } from '../..';
import Layout from '../Layout/Layout';

const AppRouter = () => {
    const routes = useMemo(() => {
        const _routes: RouteType[] = [...appRoutes];
        return _routes;
    }, []);

    const renderRoutes = () =>
        routes.map((route, i) => (
            <Route key={`route-${i}`} path={route.path} element={(<route.element />)} />
        ));

    const router = createBrowserRouter(
        createRoutesFromElements(
            <Route element={<Layout />}>
                {renderRoutes()}
            </Route>
        )
    );

    return <RouterProvider router={router} />;
};

export default AppRouter;
