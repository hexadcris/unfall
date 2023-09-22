import { createTheme } from '@mui/material/styles';
import { render } from '@testing-library/react';
import { ThemeProvider } from "@mui/material";
import { ReactElement } from 'react';
import { GlobalContextProvider } from '../context/ContextProvider';
import { BrowserRouter } from 'react-router-dom';
// Define a mock theme with the necessary properties
export const mockTheme = createTheme({
  palette: {
    primary: {
      main: '#1D2D53',
      contrastText: '#FFFFFF',
      light: '#DEE5EF',
      dark: '#173057',
    },
    secondary: {
      main: '#000',
      contrastText: '#FFFFFF',
    },
    warning: {
      main: '#ED6C02',
      dark: '#E65100',
      light: '#F5EBE3',
    },
    success: {
      main: '#2E7D32',
      dark: '#1B5E20',
      light: '#E3EFE4',
    },
    error: {
      main: '#D32F2F',
      dark: '#C62828',
      light: '#F3DBDB',
    },
    greyScale: {
      main: '#BEBEBE',
      dark: '#808285',
      light: '#F4F4F4',
      medium: '#D7D7D7',
    },
  },
  typography: {
    fontFamily: 'Roboto',
    htmlFontSize: 20,
  },
});

// Use the mock theme in your tests
export const renderWithProviders = (component: ReactElement) => {
  return render(
    <GlobalContextProvider>
      <ThemeProvider theme={mockTheme}>
        <BrowserRouter>
          {component}
        </BrowserRouter>
      </ThemeProvider>
    </GlobalContextProvider>
  );
};
