# Unfallforschung Frontend Project Documentation

## Table of Contents

- [Unfallforschung Frontend Project Documentation](#unfallforschung-frontend-project-documentation)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installation](#installation)
      - [Running the Project Locally](#running-the-project-locally)
      - [Building the Project](#building-the-project)
      - [Running Tests](#running-tests)
    - [Project Structure](#project-structure)
  - [Environment Variables](#environment-variables)
  - [React and TypeScript](#react-and-typescript)
    - [Overview of React and TypeScript Usage](#overview-of-react-and-typescript-usage)
    - [Context and State Management](#context-and-state-management)
    - [Custom State Management Hook](#custom-state-management-hook)
    - [Global Context for State Management](#global-context-for-state-management)
  - [Material-UI and Styled Components](#material-ui-and-styled-components)
    - [Material-UI Theme](#material-ui-theme)
    - [Styled Components](#styled-components)
  - [Vite Configuration](#vite-configuration)
  - [Unit Testing with Jest and Integration with React Testing Library](#unit-testing-with-jest-and-integration-with-react-testing-library)
    - [Test Setup](#test-setup)
    - [Test Suite Example](#test-suite-example)
  - [Routing with React Router](#routing-with-react-router)
    - [Router Component](#router-component)
    - [Route Configuration](#route-configuration)
    - [Path Enum](#path-enum)
    - [Layout Component](#layout-component)

---

## Introduction

Unfallforschung is an open source project which is aiming to build a simulation platform where all interested parties can simulate different traffic simulations and use the information for the further car safety projects development.

---

## Getting Started

### Prerequisites

Ensure you have the following prerequisites installed on your system:

- **npm:** Version >=8.3.1 <=9.0.0
- **node:** Version >=16.13.2
- **yarn:** Version >=1.21.17

### Installation

To install project dependencies:

1. Navigate to the `openAPI/generateApiClientFrontend` folder.
2. Run the following commands:
   ```shell
   yarn install
   yarn build
   ```
3. Return to the root project directory.
4. Run the following command to install dependencies for the root project:
   ```shell
   yarn install
   ```
#### Running the Project Locally
To run the project locally, use the following command:
  ```shell
   yarn dev
   ```
#### Building the Project
To build the project, execute the following command:
   ```shell
   yarn build
   ``` 
#### Running Tests
To run tests, use the following command:
  ```shell
  yarn test
  ``` 
All of these commands should be executed after installing project dependencies with yarn install.

---
### Project Structure

- **`package.json`**: Contains project metadata and dependencies.
- **`tsconfig.json`**: TypeScript configuration file specifying compilation settings.
- **`.env`**: Environment variables file for project configuration.
- **`eslint.cjs`**: Configuration for ESLint, a linting tool for code quality.
- **`vite.config.js`**: Configuration for Vite, the build tool.
- **`src/`**: Main source code directory containing:

    - **`components/`**: React components for the user interface.
    - **`__tests__/`**: Test files for components and interfaces.
    - **`assets/`**: Static assets like images and icons.
    - **`config/`**: General application configuration settings.
    - **`context/`**: React context for global state management.
    - **`hooks/`**: Custom React hooks for component logic.
    - **`styles/`**: Styled Components and CSS files.
    - **`utils/`**: General utility functions and backend API calls.
    - **`setupTest.ts`**: Setup code for tests.
    - **`index.css`**: Main CSS file with global styles.
    - **`main.tsx`**: Entry point for the React application.
    - **`app.tsx`**: Main component of the application.

## Environment Variables

- **`VITE_ENVIRONMENT`**: Determines the application environment ('local' or 'dev').
- **`VITE_API_DEV_URL`**: API URL for backend endpoints in the 'dev' environment.
- **`VITE_API_LOCAL_URL`**: API URL for backend endpoints in the 'local' environment.
- **`VITE_TEST_REACT`**: Controls rendering of main or test components based on the environment.

---

## React and TypeScript

### Overview of React and TypeScript Usage

React and TypeScript are at the core of this project. We use TypeScript to provide static typing for our React components and ensure a more robust and maintainable codebase.

### Context and State Management

In this section, we'll explore how context and state management are handled in the project. The project uses React's context API for managing global state, and a custom hook is provided for handling local state with dynamic typing.

### Custom State Management Hook

A custom hook named `useForm` is used to manage local state with dynamic typing. This hook is designed to simplify state management for objects, especially in forms. It provides the following functions:

- `handleInputChange(e: React.ChangeEvent<HTMLInputElement>)`: Handles input changes and updates the state accordingly.
- `handleChange(newValues: Partial<Type>)`: Merges new values into the current state.
- `handleChangeByName(name: string, value: any, simulations: ISimulationId[] | null = null)`: Handles changes by name, allowing you to update specific properties in the state.

```javascript
import { useState } from "react";
import { ISimulationId } from "../components/interfaces/SetupConfigs/setupConfigs";

export const useForm = <Type>(initialState: Type) => {
  const [values, setValues] = useState(initialState);


  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setValues({ ...values, [name]: value });
  }

  const handleChange = (newValues: Partial<Type>) => {
    setValues(prev => ({ ...prev, ...newValues }));
  }

  const handleChangeByName = (name: string, value: any, simulations: ISimulationId[] | null = null) => {
    if (simulations) {
      setValues({
        ...values,
        simulations: simulations,
        [name]: value
      });
      return;
    }
    setValues({ ...values, [name]: value });
  }

  return { values, handleChangeByName, handleInputChange, handleChange };
}
```
### Global Context for State Management
For managing global state, the project uses React's context API. This global context provides state and functions that can be accessed throughout the application. It includes:
- **`modal`**: State for managing modals in the application.
- **`setModal`**: Function to set the modal state.
- **`selectedTab`**: State for managing the selected route.
- **`setSelectedTab`**: Function to set the selected route.
- **`progress`**: State to manage progress indicator bar.
- **`setProgress`**: Function to set the progress indicator bar.

```javascript
import { createContext, useContext, useState, ReactNode, Dispatch, SetStateAction, useRef } from "react";
import { IModal, PATHS } from "../index.d";

interface GlobalContent {
  // ... (state properties and functions)
}

export const GlobalContext = createContext<GlobalContent>({
  // ... (initial default values)
});

export const GlobalContextProvider: React.FC<GlobalContextProviderProps> = ({
  children
}) => {
  // ... (state and functions)

  return (
    <GlobalContext.Provider value={{ modal, setModal, sideWindowRef, scrollByPixels, selectedTab, setSelectedTab, progress, setProgress }}>
      {children}
    </GlobalContext.Provider>
  );
};

export const useGlobalContext = () => useContext(GlobalContext);

```

The global context allows components to access and update shared state without the need for prop drilling, making it easier to manage global concerns.

---

## Material-UI and Styled Components
### Material-UI Theme

The project adopts Material-UI as the component library and introduces a custom theme configuration. The theme is created using createTheme from @mui/material/styles, encompassing customizations for various aspects such as colors, typography, and component styles.

```javascript
import { createTheme } from '@mui/material/styles';

export const theme = createTheme({
  palette: {
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
  }
  typography: {
        fontFamily: 'Roboto',
        htmlFontSize: 20,
    },
});

```

The theme configuration includes specifications for primary and secondary colors, as well as personalized adjustments for diverse Material-UI components.

### Styled Components
For styling, the project employs Styled Components. Below, you'll find examples of two styled components, StickyDiv and FooterComponent, accompanied by their respective styling properties:

```javascript
import { styled } from '@mui/material/styles';
import { Box } from '@mui/material';

export const StickyDiv = styled(Box)(({ theme }) => ({
  display: 'flex',
  alignItems: 'center',
  justifyContent: 'space-between',
  borderBottom: `1px solid ${theme.palette.grey[300]}`,
  zIndex: 1,
  position: 'sticky',
  top: 0,
  background: "#FFFFFF",
  height: theme.spacing(16)
}));

export const FooterComponent = styled(Box)(({ theme }) => ({
  position: 'fixed',
  bottom: 0,
  background: theme.palette.common.white,
  width: '-webkit-fill-available',
  paddingLeft: theme.spacing(1),
  height: theme.spacing(14),
  display: 'flex',
  alignItems: 'center'
}));

```
These styled components offer a convenient and maintainable approach to styling components within the application. The styling properties are defined based on the provided theme, ensuring consistency in the visual design of the application.

---

## Vite Configuration

In this project, Vite serves as the build tool, and the configuration is minimal without any custom settings. The configuration is as follows:
```javascript
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
})
```

---

## Unit Testing with Jest and Integration with React Testing Library

In this project, unit testing is performed using Jest, a popular JavaScript testing framework, and React Testing Library, a library that simplifies testing React components. The testing setup includes the configuration of environment variables and the creation of test suites and cases.

### Test Setup

To configure the testing environment, a setupTest.ts file is used. It includes the following setup:

```javascript
import '@testing-library/jest-dom';
import '@testing-library/react';

// Mocking environment variables for testing purposes
jest.mock('./utils/constants', () => ({
    VITE_ENVIRONMENT: 'dev',
    VITE_API_DEV_URL: 'https://8cea34a7-8df9-463b-ab26-75fe56c1390e.mock.pstmn.io/',
    VITE_API_LOCAL_URL: 'http://localhost:8848',
    VITE_TEST_REACT: 'dev'
}));

// Additional global declarations or configurations can be added here
declare global {
    function showDirectoryPicker({ }: any): any;
}

```

This file sets up Jest DOM and React Testing Library, and it mocks certain environment variables for testing purposes.

### Test Suite Example

Here's an example of a test suite for a simple component named GlobalSetup. This suite utilizes renderWithProviders to render the component and then performs various assertions to test its behavior:

```javascript
import { screen } from "@testing-library/react";
import GlobalSetup from "../components/interfaces/GlobalSetup/GlobalSetup";
import { renderWithProviders } from "../utils/test.utils";

describe('Render Test for Global Setup Interface', () => {
  beforeEach(() => {
    renderWithProviders(<GlobalSetup />);
  });

  test('Should Render System Editor Section with its input', () => {
    // Perform assertions to test the rendering of System Editor section
  });

  test("renders PRE section", () => {
    // Perform assertions to test the rendering of PRE section
  });

  test("renders RUN section", () => {
    const runSection = screen.getByText("RUN");
    expect(runSection).toBeInTheDocument();
    const pathToLibraries = screen.getByText("Path to libraries/modules");
    expect(pathToLibraries).toBeInTheDocument();
    const pathToCore = screen.getByText("Path to openPASS core");
    expect(pathToCore).toBeInTheDocument();
    const pathToSimManager = screen.getByText("Path to openPASS simulation manager");
    expect(pathToSimManager).toBeInTheDocument();
  });

  test("renders FooterComponent with Save Changes button", () => {
    // Perform assertions to test the rendering of FooterComponent
  });
});

```
This test suite uses the renderWithProviders utility function to render the GlobalSetup component with the necessary providers and context. It then contains individual test cases to verify different aspects of the rendered component, such as the presence of specific elements and text content.

With this setup, you can comprehensively test your React components to ensure their correctness and behavior in different scenarios.

---

## Routing with React Router

In this project, React Router is used to manage the routing and navigation within the application. React Router provides a declarative way to handle client-side navigation and rendering of components based on the current URL.

### Router Component
The core of the routing setup is the AppRouter component, which is responsible for defining the application's routes and rendering the appropriate components based on the current URL. Below is the AppRouter component:

```javascript
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

```

### Route Configuration
The AppRouter component uses a list of routes defined in the appRoutes array. Each route consists of a path and an associated React component element. Here's the RouteType interface and the list of app routes:

```javascript
export interface RouteType {
    path: string
    element: React.ComponentType<any>;
}

export const appRoutes: RouteType[] = [
    {
        path: PATHS.HOME,
        element: Home
    },
    {
        path: PATHS.GLOBAL_SETUP,
        element: GlobalSetup
    },
    // ... (other routes)
];

```

### Path Enum
The PATHS enum defines the various URL paths used in the application:
```javascript
export enum PATHS {
    HOME = '/',
    GLOBAL_SETUP = '/global-setup',
    SYSTEM_EDITOR = '/system-editor',
    SETUP_CONFIGS = '/setup-config',
    // ... (other paths)
}

```

### Layout Component
The Layout component is a crucial part of the routing setup. It provides the overall structure of the application layout, including the navigation menu and content area. Here's the Layout component:
```javascript
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

```
The Layout component serves as the template for rendering the application's content and provides a consistent user interface for all routes.

---