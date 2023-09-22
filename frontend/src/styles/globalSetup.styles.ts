import { Box, Typography } from "@mui/material";
import { styled } from '@mui/material/styles';

export const GlobalSetupBox = styled(Box)(({ theme }) => ({
    display: 'flex',
    borderRadius: theme.spacing(1),
    marginRight: theme.spacing(8),
    justifyContent: 'space-between',
    alignItems: 'center',
    backgroundColor: theme.palette.common.white,
    paddingTop: theme.spacing(4),
    paddingBottom: theme.spacing(4),
    paddingLeft: theme.spacing(8),
    paddingRight: theme.spacing(8)
}));

export const GlobalSetupBoxTypography = styled(Typography)(({ }) => ({
    flex: .4
}));

export const GlobalSetupConfigBox = styled(Box)(({ theme }) => ({
    paddingLeft: theme.spacing(8),
    marginTop: theme.spacing(8)
}));

export const GlobalSetupTypography = styled(Typography)(({ theme }) => ({
    paddingBottom: theme.spacing(2),
    fontWeight: '500'
}));