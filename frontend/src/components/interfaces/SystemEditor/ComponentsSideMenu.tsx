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

import { Accordion, AccordionDetails, AccordionSummary, Box, Typography, useTheme } from '@mui/material'
import { useCardTarget } from '../../../hooks/useCardTarget';
import { ISystemEditor } from './systemEditor.d';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import { DraggableSideItem } from '../../../styles/systemEditor.styles';

interface Props extends Partial<ISystemEditor> {
    handleChange: (values: Partial<ISystemEditor>) => void
};
const ComponentsSideMenu: React.FC<Props> = ({ showAccordion, components, availableElements, handleChange }) => {
    const theme = useTheme();
    const { dragStartItem } = useCardTarget(availableElements!, handleChange);
    return (
        <Box
            sx={{
                height: '100%',
                width: showAccordion ? '20%' : '.9%',
                position: 'relative',
                overflowY: 'scroll',
                backgroundColor: theme.palette.primary.accent1Medium,
                transition: 'all 0.5s ease-in-out',
                transform: showAccordion ? 'translateX(0%p)' : 'translateX(5%)',
                paddingBottom: theme.spacing(14)
            }}
            data-testid="test-id-box-accordion-zone"
        >
            {components &&
                Object.keys(components).map((key) => (
                    <Accordion
                        sx={{
                            background: 'unset',
                            paddingX: 2,
                            color: showAccordion ? theme.palette.primary.main : 'transparent'
                        }}
                        key={key}
                        defaultExpanded={true}
                        data-testid={`test-id-accordion-${key}`}
                    >
                        <AccordionSummary
                            expandIcon={<ExpandMoreIcon />}
                            aria-controls="panel1a-content"
                            id="panel1a-header"
                        >
                            <Typography sx={{ textTransform: 'capitalize' }}>{key}</Typography>
                        </AccordionSummary>
                        <AccordionDetails>
                            {components[key].length > 0 &&
                                components[key].map((value, subIndex) => (
                                    <DraggableSideItem
                                        key={subIndex}
                                        draggable
                                        data-testid={`test-id-draggable-${value.title}`}
                                        onDragStart={(ev: React.DragEvent<HTMLDivElement>) => { dragStartItem(ev, value) }}
                                    >
                                        <Typography variant="subtitle1" gutterBottom>
                                            {value.title}
                                        </Typography>
                                    </DraggableSideItem>
                                ))
                            }
                        </AccordionDetails>
                    </Accordion>
                ))}
        </Box>
    )
}

export default ComponentsSideMenu