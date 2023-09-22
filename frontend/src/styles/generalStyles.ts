import { styled } from '@mui/material/styles';
import { InputBase } from '@mui/material';

export const DivGrouped = styled('div')(({
    display: 'flex',
    alignItems: 'center'
}));

export const InputFile = styled(InputBase)(({ theme }) => ({
    border: '1px solid',
    borderColor: theme.palette.grey[300],
    paddingLeft: theme.spacing(3),
    background: '#FFFFFF',
    opacity: 1,
    pointerEvents: 'none',
    '&.Mui-error': {
        borderColor: theme.palette.error.main,
    }
}));
