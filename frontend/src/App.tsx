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

import { CssBaseline, ThemeProvider } from "@mui/material";
import AppRouter from "./components/Routes/RoutesComponent";
import { theme } from './config/themeProvider.ts';
import { GlobalContextProvider } from "./context/ContextProvider.tsx";
function App() {
  return (
    <GlobalContextProvider>
      <ThemeProvider theme={theme}>
        <CssBaseline>
          <AppRouter />
        </CssBaseline>
      </ThemeProvider>
    </GlobalContextProvider>
  )
}

export default App;
