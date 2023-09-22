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

import { Box, Divider, IconButton, Menu, MenuItem, Typography, useTheme } from '@mui/material'
import { useCardTarget } from '../../../hooks/useCardTarget';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import LoginOutlinedIcon from '@mui/icons-material/LoginOutlined';
import LogoutOutlinedIcon from '@mui/icons-material/LogoutOutlined';
import { ConnectionComponent, ConnectionList, DragComponent, ExtendedComponent, ISystemEditor } from './systemEditor.d';
import { useGlobalContext } from '../../../context/ContextProvider';
import DeleteOutlineOutlinedIcon from '@mui/icons-material/DeleteOutlineOutlined';
import ControlPointDuplicateOutlinedIcon from '@mui/icons-material/ControlPointDuplicateOutlined';
import { ColorBox, NumberInput } from '../../../styles/systemEditor.styles';
import WarningAmberIcon from '@mui/icons-material/WarningAmber';
import React, { useEffect, useState } from 'react';
import { ColorVariant, Variant } from '../../../index.d';
import { LightTooltip } from '../../../styles/general.styles';
import { Input, Output } from 'opGUI-client-frontend';
import Xarrow, { useXarrow, Xwrapper } from 'react-xarrows';
import DeleteIcon from '@mui/icons-material/Delete';
import { VITE_TEST_REACT } from '../../../utils/constants';

interface Props extends ISystemEditor {
    handleChange: (values: Partial<ISystemEditor>) => void;
    availableElements: DragComponent[]
};

const TargetZone: React.FC<Props> = ({ showAccordion, availableElements, handleChange, selectedSystem, connections, hideConnections }) => {
    const {
        allowDrop,
        dropElement,
        dragStartItem
    } = useCardTarget(availableElements!, handleChange);
    const colors = [
        '#6DD4CE',
        '#EDEF97',
        '#EA5EC2',
        '#542DF1',
        '#1D2D53',
        '#ADB9CE',
        '#F3DBDB',
        '#2E7D32'
    ];
    const { setShowDiscardModal } = useGlobalContext();
    const updateXarrow = useXarrow();
    const theme = useTheme();

    const [anchorEl, setAnchorEl] = useState<SVGElement | null>(null);
    const [colorMenu, setColorMenu] = useState<HTMLDivElement | null>(null);
    const [connectionStart, setConnectionStart] = useState<ConnectionComponent | null>(null);
    const [selectedComponent, setSelectedComponent] = useState<null | ExtendedComponent | DragComponent>(null);
    const open = Boolean(anchorEl);
    const { setModal, modal } = useGlobalContext();

    const handleOpenMenu = (event: React.MouseEvent<SVGElement>, index: number) => {
        setAnchorEl(event.currentTarget);
        setSelectedComponent({ ...availableElements[index], index: index });
    };
    const duplicateItem = () => {
        const newComponents = [...availableElements, {
            ...selectedComponent, position: {
                x: ((selectedComponent as DragComponent).position.x + 120)
            }
        }];
        handleChange({ availableElements: newComponents });
        setAnchorEl(null);
        setSelectedComponent(null);
    }
    const deleteItem = () => {
        if (selectedComponent) {
            setModal({
                ...modal,
                active: true,
                title: 'Delete Component',
                descriptionTitle: `The Component ${selectedComponent.title} will be deleted`,
                description: `After this action, the component cannot be recovered.`,
                loadingBar: false,
                icon: WarningAmberIcon,
                iconVariant: ColorVariant.warning,
                buttons: [
                    {
                        title: 'Cancel',
                        variant: Variant.outlined
                    },
                    {
                        title: 'Delete Component',
                        action: () => {
                            const newComponents = [...availableElements];

                            newComponents.splice(selectedComponent.index, 1);
                            const newConnections = connections.filter(con =>
                                !(con.indexComponentInput === selectedComponent.index || con.indexComponentOutput === selectedComponent.index)
                            ).map((con) => ({
                                ...con,
                                indexComponentInput: newComponents.findIndex((component) => component.id === con.target.component),
                                indexComponentOutput: newComponents.findIndex((component) => component.id === con.source.component)
                            }));
                            handleChange({ availableElements: newComponents, connections: newConnections });
                            setConnectionStart(null);
                            setAnchorEl(null);
                            setSelectedComponent(null);
                            setShowDiscardModal(true);
                        },
                        testId: 'test-id-modalbutton-delete-component'
                    },
                ],
            });
        }
    }

    const removeConnection = (id: number) => {
        setModal({
            ...modal,
            active: true,
            title: 'Delete Connection',
            descriptionTitle: `The connection will be deleted`,
            description: `After this action, the connection cannot be recovered.`,
            loadingBar: false,
            icon: WarningAmberIcon,
            iconVariant: ColorVariant.warning,
            buttons: [
                {
                    title: 'Cancel',
                    variant: Variant.outlined
                },
                {
                    title: 'Delete Connection',
                    action: () => {
                        const newConnections = connections.filter(con => con.id !== id);
                        handleChange({ connections: newConnections });
                    },
                    testId: 'test-id-delete-connection-button'
                },
            ],
        });

    }

    const handleConnectionEnd = (element: DragComponent, input: Input, index: number) => {
        if (connectionStart === null) {
            setModal({
                ...modal,
                active: true,
                title: 'Need to select an output first to start a connection',
                descriptionTitle: '',
                description: `Please select and output first.`,
                loadingBar: false,
                icon: null,
                buttons: [
                    {
                        title: 'Close',
                    }
                ],
            });
            return;
        }
        if (connectionStart.index === index) {
            setModal({
                ...modal,
                active: true,
                title: 'Cannot create a connection with the same component.',
                descriptionTitle: '',
                description: `Please select a different component.`,
                loadingBar: false,
                icon: null,
                buttons: [
                    {
                        title: 'Close',
                    }
                ],
            });
            return;
        }
        const conExist = connections.find((con) => (
            con.target.input === input.id &&
            con.source.output === connectionStart.output.id &&
            con.source.component === connectionStart.component.id &&
            con.target.component === element.id
        ));
        if (conExist) {
            setModal({
                ...modal,
                active: true,
                title: 'Theres already a connection between this 2 components..',
                descriptionTitle: '',
                description: `Please select a different component.`,
                loadingBar: false,
                icon: null,
                buttons: [
                    {
                        title: 'Close',
                    }
                ],
            });
            return;
        }
        const newConnections = [...connections];
        newConnections.push({
            id: newConnections?.length + 1,
            indexComponentOutput: connectionStart.index,
            indexComponentInput: index,
            source: {
                component: connectionStart.component.id,
                output: connectionStart.output.id
            },
            target: {
                component: element.id,
                input: input.id
            }
        });
        handleChange({ connections: newConnections });
        setConnectionStart(null);
    }

    const getIdOutputElement = (connection: ConnectionList): string => {
        const component = availableElements?.find(comp => comp.id === connection.source.component);
        if (!component) return "";
        if (!component.outputs) return "";
        const output = component.outputs.find(out => out.id === connection.source.output);
        if (!output) return "";
        const id = `${connection.source.output}-${output?.title}-${connection.source.component}-${connection.indexComponentOutput}`;
        return id;
    }

    const getIdInputElement = (connection: ConnectionList): string => {
        const component = availableElements.find(comp => comp.id === connection.target.component);
        if (!component) return "";
        if (!component.inputs) return "";
        const input = component.inputs.find(inp => inp.id === connection.target.input);
        const id = `${connection.target.input}-${input?.title}-${connection.target.component}-${connection.indexComponentInput}`;
        return id;
    }

    const handleConnectionStart = (element: DragComponent, output: Output, index: number) => {
        if (connectionStart === null) {
            setConnectionStart({ component: { ...element }, output: { ...output }, index });
            return;
        }
        if (element.id === connectionStart.component.id && output.id === connectionStart.output.id && index === connectionStart.index) {
            setConnectionStart(null);
            return;
        }
        setModal({
            ...modal,
            active: true,
            title: 'Connection Already Started',
            descriptionTitle: '',
            description: `You already started a connection, please complete it or cancel it`,
            loadingBar: false,
            icon: null,
            buttons: [
                {
                    title: 'Close',
                }
            ],
        });
        return;
    }

    const finalDrop = (event: React.DragEvent<HTMLDivElement>) => {
        if (selectedSystem === null) {
            setModal({
                ...modal,
                active: true,
                title: 'Select A System',
                descriptionTitle: '',
                description: `To add components a system must first be selected`,
                loadingBar: false,
                icon: null,
                buttons: [
                    {
                        title: 'Close',
                    }
                ],
            });
            return;
        }
        dropElement(event);
    }
    const handleNumericInput = (event: React.ChangeEvent<HTMLInputElement>) => {
        const value = event.target.value;
        const numericValue = value.replace(/[^0-9]/g, '');
        event.target.value = numericValue;
    }

    const handleValueChange = (event: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>, indexList: number, indexParam: number) => {
        const { name, value } = event.target;
        const newComponents = [...availableElements];
        newComponents[indexList][name][indexParam]["value"] = value;
        handleChange({ availableElements: newComponents });
    }

    const getColor = (connection: ConnectionList): string => {
        const component = availableElements.find(comp => comp.id === connection.source.component);
        const color = (!component) ? theme.palette.primary.main : component.color;
        return color;
    }

    const handleChangeSchedule = (event: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>, indexList: number, key: string) => {
        const { name, value } = event.target;
        const newComponents = [...availableElements];
        newComponents[indexList][name][key] = value;
        handleChange({ availableElements: newComponents });
    }

    const handleChangeColor = (color: string) => {
        const newComponents = [...availableElements];
        newComponents[selectedComponent?.index!]['color'] = color;
        handleChange({ availableElements: newComponents });
        setColorMenu(null);
        setSelectedComponent(null);
    }

    useEffect(() => {
        updateXarrow();

    }, [availableElements])

    return (
        <Box
            id='card-target'
            className="droppable-zone"
            sx={{
                backgroundColor: 'transparent',
                transition: 'all 0.5s ease-in-out',
                width: showAccordion ? '80%' : '100%',
                height: '95%',
                overflow: 'scroll',
                position: 'relative',
            }}
            data-testid="test-id-box-droppable-zone"
            onDragOver={allowDrop}
            onDrop={finalDrop}
        >
            {availableElements?.map((element, index: number) => (
                <Box
                    data-testid={`test-id-draggable-${index}`}
                    key={index}
                    draggable
                    onDragStart={(ev) => { dragStartItem(ev, { ...element, index }) }}
                    style={{
                        left: element.position.x,
                        top: element.position.y,
                    }}
                    sx={{
                        position: 'absolute',
                        zIndex: 2,
                        background: theme.palette.common.white,
                        width: theme.spacing(80),
                        border: `1px solid ${element.color}`,
                        borderRadius: theme.spacing(1),
                    }}
                    padding={theme.spacing(2)}
                >
                    <Box display='flex' alignItems='center' >
                        <ColorBox
                            data-testid={`test-id-color-button-${index}`}
                            sx={{
                                background: element.color,
                            }}
                            onClick={(ev) => {
                                setColorMenu(ev.currentTarget);
                                setSelectedComponent({ ...availableElements[index], index: index });
                            }}
                        />
                        <Typography flex={1} variant='h6'>
                            {element.title}
                        </Typography>
                        <KeyboardArrowDownOutlinedIcon
                            color="primary"
                            data-testid={`test-id-open-menu-${index}`}
                            onClick={(ev) => handleOpenMenu(ev, index)}
                            sx={{ justifySelf: 'flex-end', cursor: 'pointer', zIndex: 3 }}
                        />
                    </Box>
                    {element.schedule &&
                        <Box sx={{
                            'h6': {
                                paddingTop: theme.spacing(2)
                            },
                            borderTop: `1px solid ${theme.palette.greyScale.medium}`,
                            borderBottom: `1px solid ${theme.palette.greyScale.medium}`,
                            paddingBottom: theme.spacing(4)
                        }}
                            display='flex'
                            alignItems='center'
                            justifyContent='space-around'
                            padding={theme.spacing(2)}
                            marginTop={theme.spacing(2)}>
                            {Object.entries(element.schedule).map(([key, value], i) => (
                                <React.Fragment key={key}>
                                    <NumberInput
                                        data-testid={`schedule-input-${key}-${index}`}
                                        sx={{ marginLeft: 2, marginRight: 1 }}
                                        size="small"
                                        name={'schedule'}
                                        id={`${key}-${index}`}
                                        value={value}
                                        onChange={(e) => handleChangeSchedule(e, index, key)}
                                        inputProps={{
                                            inputMode: 'numeric',
                                            pattern: '[0-9]*',
                                            onInput: handleNumericInput
                                        }}
                                    />
                                    {i !== Object.entries(element.schedule).length - 1 && (
                                        <Typography variant='h6' fontWeight='bold'>
                                            /
                                        </Typography>
                                    )}
                                </React.Fragment>
                            ))}
                        </Box>
                    }
                    {
                        (element.parameters && element.parameters.length > 0) &&
                        <Divider sx={{ background: element.color, marginTop: theme.spacing(4) }} />
                    }
                    {element.parameters &&
                        < Box
                            marginTop={theme.spacing(4)}
                        >
                            {
                                element.parameters.map((param, indexParam) => (
                                    <Box
                                        sx={{
                                            ':first-of-type': {
                                                borderTop: `1px solid ${theme.palette.greyScale.medium}`
                                            }
                                        }}
                                        key={param.title + indexParam}
                                        display='flex'
                                        alignItems='center'
                                        gap={theme.spacing(2)}
                                        borderBottom={`1px solid ${theme.palette.greyScale.medium}`}
                                        padding={theme.spacing(2)}>
                                        <Typography flex={1}>
                                            {param.unit !== '' ? `${param.title} [${param.unit}]` : param.title}
                                        </Typography>
                                        <NumberInput
                                            data-testid={`test-id-parameter-${index}-${param.title}`}
                                            sx={{ marginLeft: 2, marginBottom: theme.spacing(2) }}
                                            size="small"
                                            name='parameters'
                                            value={param.value}
                                            onChange={(e) => handleValueChange(e, index, indexParam)}
                                            id={`${param.title}-${index}`}
                                            inputProps={{
                                                inputMode: 'numeric',
                                                pattern: '[0-9]*',
                                                onInput: handleNumericInput
                                            }}
                                        />
                                    </Box>
                                ))
                            }
                        </Box>
                    }
                    {
                        (element.inputs && element.inputs.length > 0) &&
                        <Divider sx={{ background: element.color, marginTop: theme.spacing(4) }} />
                    }
                    {element.inputs &&
                        <Box
                            marginTop={theme.spacing(4)}
                        >
                            {element.inputs.map((input, indexInput) => (
                                <Box
                                    sx={{
                                        ':first-of-type': {
                                            borderTop: `1px solid ${theme.palette.greyScale.medium}`
                                        }
                                    }}
                                    id={`${input.id}-${input.title}-${element.id}-${index}`}
                                    key={input.title + indexInput}
                                    display='flex'
                                    alignItems='center'
                                    gap={theme.spacing(2)}
                                    borderBottom={`1px solid ${theme.palette.greyScale.medium}`}
                                    padding={theme.spacing(2)}>
                                    <LightTooltip placement="bottom-start" title="Input element">
                                        <IconButton
                                            data-testid={`test-id-input-button-${index}-${input.title}`}
                                            onClick={() => handleConnectionEnd(element, input, index)}
                                            sx={{
                                                padding: 'unset',
                                                background: (connectionStart != null) ?
                                                    connectionStart.component.color :
                                                    'transparent',
                                            }}
                                            aria-label='Remove Icon'>
                                            <LoginOutlinedIcon sx={{ color: (connectionStart != null) ? theme.palette.common.black : theme.palette.primary.accent3 }} />
                                        </IconButton>
                                    </LightTooltip>
                                    <Typography flex={1}>
                                        {input.unit !== '' ? `${input.title} [${input.unit}]` : input.title}
                                    </Typography>
                                </Box>
                            ))
                            }
                        </Box>
                    }

                    {
                        (element.outputs && element.outputs.length > 0) &&
                        <Divider sx={{ background: element.color, marginTop: theme.spacing(4) }} />
                    }
                    {element.outputs &&
                        <Box marginTop={theme.spacing(4)}>
                            {element.outputs.map((output, indexOutput) => (
                                <Box
                                    sx={{
                                        ':first-of-type': {
                                            borderTop: `1px solid ${theme.palette.greyScale.medium}`
                                        }
                                    }}
                                    id={`${output.id}-${output.title}-${element.id}-${index}`}
                                    key={output.title + indexOutput}
                                    display='flex'
                                    alignItems='center'
                                    gap={theme.spacing(2)}
                                    borderBottom={`1px solid ${theme.palette.greyScale.medium}`}
                                    padding={theme.spacing(2)}>
                                    <Box display='flex' gap={theme.spacing(2)} flex={1}></Box>
                                    <Typography >
                                        {output.unit !== '' ? `${output.title} [${output.unit}]` : output.title}
                                    </Typography>
                                    <LightTooltip placement="bottom-start" title="Output element">
                                        <IconButton
                                            data-testid={`test-id-output-button-${index}-${output.title}`}
                                            onClick={() => handleConnectionStart(element, output, index)}
                                            className={`${(connectionStart && connectionStart.index === index && connectionStart.component.id === element.id && connectionStart.output.id === output.id) ? 'selectedConnection' : ''}`}
                                            sx={{
                                                padding: 'unset',
                                                background: (connectionStart != null && connectionStart.component.id === element.id && output.id === connectionStart.output.id && connectionStart.index === index) ?
                                                    connectionStart.component.color :
                                                    'transparent',
                                            }}
                                            aria-label='Output Icon'>
                                            <LogoutOutlinedIcon sx={{ color: theme.palette.primary.accent3 }} />
                                        </IconButton>
                                    </LightTooltip>
                                </Box>
                            ))
                            }
                        </Box>
                    }
                </Box >
            ))
            }
            {(!hideConnections && VITE_TEST_REACT !== 'dev') &&
                <Xwrapper>
                    {connections.length > 0 && connections.map((connection, index) => (
                        <Xarrow
                            key={index}
                            start={getIdOutputElement(connection)}
                            end={getIdInputElement(connection)}
                            color={getColor(connection)}
                            showHead={true}
                            labels={connection.labels ? connection.labels : ''}
                            passProps={{
                                onClick: () => removeConnection(connection.id),
                                cursor: 'pointer',
                                onMouseOver: () => {
                                    connection.labels = <DeleteIcon sx={{ color: theme.palette.error.main }} />;
                                    updateXarrow();
                                },
                                onMouseOut: () => {
                                    connection.labels = '';
                                    updateXarrow();
                                },
                            }}
                        />
                    ))}
                </Xwrapper>
            }
            {(!hideConnections && VITE_TEST_REACT === 'dev') &&
                <>
                    {connections.length > 0 && connections.map((connection, index) => (
                        <Box
                            key={index}
                            data-testid={`test-id-connection-${connection.id}`}
                            onClick={() => removeConnection(connection.id)}
                        >
                            {connection.labels}
                        </Box>
                    ))}
                </>
            }

            <Menu
                id="basic-menu"
                data-testid="test-id-menu-draggale-dropdown"
                anchorEl={anchorEl}
                open={open}
                onClose={() => {
                    setAnchorEl(null);
                    setSelectedComponent(null);
                }}
                MenuListProps={{
                    'aria-labelledby': 'basic-button',
                }}
            >
                <MenuItem data-testid="test-id-menu-item-duplicate" onClick={duplicateItem}>
                    <ControlPointDuplicateOutlinedIcon color="primary" />
                    <Typography sx={{ color: theme.palette.primary.main, paddingLeft: 2 }}>
                        DUPLICATE
                    </Typography>
                </MenuItem>
                <MenuItem data-testid="test-id-menu-item-delete" onClick={deleteItem}>
                    <DeleteOutlineOutlinedIcon color="primary" />
                    <Typography sx={{ color: theme.palette.primary.main, paddingLeft: 2 }}>
                        DELETE
                    </Typography>
                </MenuItem>
            </Menu>
            <Menu
                id="color-menu"
                data-testid="test-id-menu-dropdown-color"
                anchorEl={colorMenu}
                open={Boolean(colorMenu)}
                onClose={() => {
                    setColorMenu(null);
                    setSelectedComponent(null);
                }}
                MenuListProps={{
                    'aria-labelledby': 'basic-button',
                }}
            >
                <Box display='flex'>
                    {colors.slice(0, 4).map((color, idx) => (
                        <MenuItem sx={{ paddingX: theme.spacing(2) }} key={idx} onClick={() => handleChangeColor(color)} data-testid={`test-id-menu-item-${idx}`}>
                            <ColorBox sx={{ background: color }} />
                        </MenuItem>
                    ))}
                </Box>
                <Box display='flex'>
                    {colors.slice(4).map((color, idx) => (
                        <MenuItem sx={{ paddingX: theme.spacing(2) }} key={idx + 4} onClick={() => handleChangeColor(color)} data-testid={`test-id-menu-item-${idx + 4}`}>
                            <ColorBox sx={{ background: color }} />
                        </MenuItem>
                    ))}
                </Box>
            </Menu>
        </Box >
    )
}

export default TargetZone