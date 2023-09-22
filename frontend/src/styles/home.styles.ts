import { styled } from '@mui/material/styles';
import { Box, Button, ButtonProps } from '@mui/material';

interface HomeButtonProps extends ButtonProps {
  'data-type'?: string;
}

export const HomeBox = styled(Box)(({ theme }) => ({
    display: 'flex',
    borderRadius: theme.spacing(1),
    paddingLeft: theme.spacing(8),
    paddingRight: theme.spacing(8),
    justifyContent: 'space-between',
    alignItems: 'center',
    backgroundColor: theme.palette.common.white,
    width: theme.spacing(183),
    height: theme.spacing(21)
}));

export const HomeButton = styled(Button)<HomeButtonProps>(({ theme, 'data-type': type }) => ({
    width: theme.spacing(40),
    '&::after': {
        position: 'absolute',
        content: "'ᐳ'",
        right: type === 'monitor' ? 35 : 45
    }
}));

export const HomeContainer = styled(Box)(({ theme }) => ({
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    flexDirection: 'column',
    marginTop: theme.spacing(20),
    marginRight: theme.spacing(20),
    gap: theme.spacing(8)
}));