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

import { Box, Button, LinearProgress, Modal, Typography, linearProgressClasses, useTheme } from '@mui/material';
import { useGlobalContext } from '../../context/ContextProvider';
import { IGenericRecord, Variant } from '../../index.d';
import { ModalStyled } from '../../styles/general.styles';
import FolderOpenOutlinedIcon from '@mui/icons-material/FolderOpenOutlined';

function ModalComponent() {
    const { modal, setModal, progress, setProgress } = useGlobalContext();
    const theme = useTheme();

    const handleClose = (_: object = {}) => {
        setModal({ ...modal, active: false });
        setProgress(0);
    }
    const handleAction = (action: (props?: IGenericRecord) => void) => {
        action();
        handleClose();
    }
    return (
        <Modal
            open={modal.active}
            onClose={handleClose}
            aria-labelledby="modal-modal-title"
            aria-describedby="modal-modal-description"
            data-testid={`test-id-modal-${modal.title}`}
        >
            <ModalStyled >
                <Box sx={{ background: theme.palette.primary.main, color: theme.palette.common.white, padding: '10px 0px 10px 20px', position: 'relative', borderTopLeftRadius: theme.spacing(1), borderTopRightRadius: theme.spacing(1) }}>
                    <Typography id="modal-modal-title" variant="h6" component="h2">
                        {modal.title}
                    </Typography>
                    <Typography onClick={handleClose} sx={{ position: 'absolute', right: '20px', fontSize: '1.6rem', bottom: '4px', cursor: 'pointer' }}>
                        &times;
                    </Typography>
                </Box>
                {modal?.descriptionTitle ?
                    <Box display='flex' flexDirection='row' alignItems='center' paddingLeft={5}>
                        <modal.icon color={modal?.iconVariant} sx={{ width: theme.spacing(16), height: theme.spacing(16) }} />
                        <Box sx={{ display: "flex", flexDirection: 'column', margin: 5, gap: 2 }}>
                            <Typography id="modal-modal-title-description" variant='h5' sx={{ flex: 1 }}>
                                {modal?.descriptionTitle}
                            </Typography>
                            <Typography id="modal-modal-description" sx={{ flex: 1 }}>
                                {modal.description}
                            </Typography>
                        </Box>
                    </Box>
                    :
                    <Typography id="modal-modal-description" sx={{ margin: 5, flex: 1 }}>
                        {modal.description}
                    </Typography>
                }
                {modal.loadingBar &&
                    <Box width='100%' marginTop={5} >
                        <Box display='flex' justifyContent='space-between' paddingX={3} paddingBottom={2}>
                            <i style={{ marginLeft: 5, width: theme.spacing(8), height: theme.spacing(8) }} className="icon-mask primary file-cog"></i>
                            <FolderOpenOutlinedIcon sx={{ marginRight: 2, width: theme.spacing(8), height: theme.spacing(8) }} color='primary' />
                        </Box>
                        <LinearProgress sx={{
                            width: '94%', borderRadius: 5, height: 10, marginLeft: 4, [`& .${linearProgressClasses.bar}`]: {
                                borderRadius: 5,
                            }
                        }} variant="determinate" value={progress} />
                    </Box>
                }
                <Box display='flex' gap={3} alignSelf='flex-end' padding={4}>
                    {modal.buttons.map((button) => (
                        <Button disabled={button.disabled} data-testid={button?.testId} key={button.title} color='primary' variant={button.variant ? button.variant : Variant.contained} onClick={(button.action ? () => button.action && handleAction(button.action) : handleClose)}>{button.title}</Button>
                    ))
                    }
                </Box>
            </ModalStyled>
        </Modal>
    )
}

export default ModalComponent;