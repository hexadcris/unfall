import { styled } from '@mui/material/styles';
import { Box, Checkbox, InputBase, Tooltip, TooltipProps, tooltipClasses } from '@mui/material';

export const DivGrouped = styled('div')(({
    display: 'flex',
    alignItems: 'center'
}));

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

export const SeparatorLine = styled(Box)(({ theme }) => ({
    height: '60%',
    border: `1px solid`,
    borderColor: theme.palette.grey[300]
}));

export const FilterCheckbox = styled(Checkbox)(({ theme }) => ({
    display: 'flex',
    width: 10,
    borderRadius: 0,
    paddingRight: theme.spacing(7),
    '&:hover': {
        backgroundColor: 'transparent'
    }
}));

export const BaseInputStyled = styled(InputBase)(({ theme }) => ({
    border: '1px solid',
    borderColor: theme.palette.grey[300],
    paddingLeft: theme.spacing(3),
    background: '#FFFFFF',
    opacity: 1,
    '&.Mui-error': {
        borderColor: theme.palette.error.main,
    }
}));

export const SideWindowBase = styled(Box)({
    position: 'relative',
    zIndex: 1
});

export const ModalStyled = styled(Box)(({ theme }) => ({
    display: 'flex',
    flexDirection: 'column',
    position: 'absolute',
    top: '50%',
    left: '50%',
    transform: 'translate(-50%, -50%)',
    width: theme.spacing(150),
    background: theme.palette.common.white,
    bgcolor: 'background.paper',
    borderRadius: theme.spacing(1)
}));

export const LightTooltip = styled(({ className, ...props }: TooltipProps) => (
    <Tooltip {...props} classes={{ popper: className }} />
))(({ theme }) => ({
    [`& .${tooltipClasses.tooltip}`]: {
        backgroundColor: theme.palette.common.white,
        color: 'rgba(0, 0, 0, 0.87)',
        boxShadow: theme.shadows[1],
        fontSize: 11,
        fontFamily: 'Roboto',
        marginTop: '0px !important'
    },
}));

export const StyledTab = styled(Box)(({ theme }) => ({
    cursor: 'pointer',
    color: theme.palette.primary.main,
    background: theme.palette.common.white,
    height: '100%',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    paddingLeft: theme.spacing(4),
    paddingRight: theme.spacing(4),
    paddingTop: theme.spacing(2),
    paddingBottom: theme.spacing(2),
    border: `1px solid ${theme.palette.greyScale.main}`,
    borderBottom: 'unset',
    textTransform: 'uppercase',
    transition: 'all .2s ease-in',
    '&:hover': {
        background: '#FFFFFF10',
        color: theme.palette.primary.mainContrast,
        transition: 'all .2s ease-in'
    },
    '&.selected': {
        borderBottom: `2px solid ${theme.palette.primary.main}`
    }
}));

