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

import { Box, Button, FormControl, FormControlLabel, Link, Menu, MenuItem, Radio, RadioGroup, Select, Typography, useTheme } from '@mui/material';
import { useState } from 'react'
import SimulationBox from './SimulationBox/SimulationBox';
import { ColorVariant, IGenericRecord, INPUT_FILE, Simulation, Variant } from '../../../index.d';
import { DivGrouped, FilterCheckbox, FooterComponent, SeparatorLine, SideWindowBase, StickyDiv } from '../../../styles/general.styles';
import UploadFile from '../../commonComponents/UploadInput/UploadFile';
import { useGlobalContext } from '../../../context/ContextProvider';
import ModalComponent from '../../Modal/ModalComponent';
import ArrowDropDownIcon from '@mui/icons-material/ArrowDropDown';
import WarningAmberIcon from '@mui/icons-material/WarningAmber';
import KeyboardArrowUpIcon from '@mui/icons-material/KeyboardArrowUp';
import { exportXml, runSimulationManager, verifyPath } from '../../../utils/api.util';
import { OpSimulationManagerXmlRequest, OpSimulationManagerXmlRequestLogLevelEnum } from 'opGUI-client-frontend';
import { SIMULATION_CONFIG_FIELDS_XML, SIMULATION_CONFIG_XML } from '../../../pcm-simulation.d';
interface Props {
    initialSims?: Simulation[]
}

const SimulationManager = (props: Props) => {
    const theme = useTheme();
    const { modal, setModal, setProgress, scrollByPixels } = useGlobalContext();
    const [readyToExecute, setReadyToExecute] = useState(true)
    const [simulations, setSimulations] = useState<Simulation[]>(props.initialSims || []);
    const [allSelected, setAllSelected] = useState(false);
    const [logLevel, setLogLevel] = useState<OpSimulationManagerXmlRequestLogLevelEnum>(OpSimulationManagerXmlRequestLogLevelEnum.NUMBER_0);
    const [configSet, setConfigSet] = useState('manual-setup');
    const [xmlFile, setXmlFile] = useState('');
    const addNewSimulation = () => {
        const id = (simulations.length + 1);
        setSimulations([...simulations, {
            id: id,
            selected: false,
            numberOfExecutions: 1,
            configurations: '',
            logFileSimulation: '',
            results: ''
        }]);
    }
    const [anchorEl, setAnchorEl] = useState<null | HTMLElement>(null);
    const open = Boolean(anchorEl);
    const handleClick = (event: React.MouseEvent<HTMLDivElement>) => {
        setAnchorEl(event.currentTarget);
    };

    const setUnsetAll = (state: boolean) => {
        const newSimulations = simulations.map((sim) => ({ ...sim, selected: state }));
        setAllSelected(state);
        setSimulations(newSimulations);
        handleClose();
    }

    const checkUndeterminated = () => {
        const selected = simulations.filter(sim => sim.selected).length;
        return (allSelected && simulations.length > selected)
    }
    const handleClose = () => {
        setAnchorEl(null);
    };

    const isEnabled = () => {
        return simulations.filter(sim => sim.selected).length === 0;
    }

    const removeSimulation = (id: number) => {
        setModal({
            ...modal,
            active: true,
            title: 'Remove Simulation',
            loadingBar: false,
            descriptionTitle: 'The simulation will be removed.',
            description: `After this action, the simulation cannot be recovered.`,
            icon: WarningAmberIcon,
            iconVariant: ColorVariant.warning,
            buttons: [
                {
                    title: 'Cancel',
                    variant: Variant.outlined
                },
                {
                    title: 'Remove Simulation',
                    action: () => setSimulations([...simulations.filter(sim => sim.id !== id)]),
                    testId: 'test-id-modalbutton-delete'
                },
            ],
        });
    }

    const cleanUpSimulations = () => {
        setModal({
            ...modal,
            active: true,
            title: 'Clear Simulations',
            descriptionTitle: 'All The Simulations Will Be Removed.',
            description: `After this action, the simulations cannot be recovered.`,
            loadingBar: false,
            icon: WarningAmberIcon,
            iconVariant: ColorVariant.warning,
            buttons: [
                {
                    title: 'Cancel',
                    variant: Variant.outlined
                },
                {
                    title: 'Clear Simulations',
                    action: () => {
                        setSimulations([]);
                        setReadyToExecute(true);
                    },
                    testId: 'test-id-modalbutton-clearall'
                },
            ],
        });
    }

    const copySimulation = (simumlation: Simulation) => {
        const newSimulation = { ...simumlation };
        newSimulation.id = (simulations.length + 1);
        setSimulations([...simulations, newSimulation]);
    }

    const handleChange = (event: React.ChangeEvent<HTMLInputElement>, index: number) => {
        const { name, value, checked } = event.target;
        const newSimulations = [...simulations];
        if (name === 'selected') {
            newSimulations[index][name] = checked;
            setSimulations(newSimulations);
            return;
        }
        newSimulations[index][name] = value;
        setSimulations(newSimulations);
    }

    const handleExport = async () => {
        const incompleteSims = simulations.filter(sim => (sim.selected && (sim.configurations === '' || sim.results === '' || sim.logFileSimulation === '')))
        if (incompleteSims.length > 0) {
            setModal({
                ...modal,
                active: true,
                title: 'Incomplete Data',
                description: `All the data for all the selected simulations should be filled.`,
                descriptionTitle: '',
                loadingBar: false,
                buttons: [
                    {
                        title: 'Cancel',
                        variant: Variant.outlined
                    }
                ],
            });
            return;
        }
        setModal({
            ...modal,
            active: true,
            title: 'Export XML',
            description: `Exporting XML config opSimulationManager`,
            loadingBar: true,
            descriptionTitle: '',
            buttons: [
                {
                    title: 'Done',
                    variant: Variant.contained,
                    disabled: true
                }
            ],
        });
        setTimeout(() => {
            setProgress(50);
        }, 100);
        const data: OpSimulationManagerXmlRequest = {
            logLevel: logLevel,
            libraries: 'modules',
            logFileSimulationManager: 'opSimulationManager.log',
            simulation: 'opSimulation',
            simulationConfigs: simulations.filter((sim) => sim.selected)
        }
        const response = await exportXml(data);
        if (response?.response) {
            setReadyToExecute(false);
            setProgress(100);
            setModal({
                ...modal,
                active: true,
                title: 'Export XML',
                description: `Exporting XML config opSimulationManager`,
                loadingBar: true,
                descriptionTitle: '',
                buttons: [
                    {
                        title: 'Done',
                        variant: Variant.contained,
                        disabled: false
                    }
                ],
            });
        }
        console.log(response?.response);

    }

    const handleFiles = async (name: string, path: string, index: number = -1) => {
        if (index >= 0) {
            const newSimulations = [...simulations];
            if (name === 'logFileSimulation') {
                path = `${path}/simulation${(index + 1)}.log`
            }
            newSimulations[index][name] = path;
            setSimulations(newSimulations);
            return;
        }
    }

    const openErrorXMLModal = () => {
        setModal({
            ...modal,
            active: true,
            loadingBar: false,
            title: 'File Format Error',
            description: 'The file you selected have an error',
            buttons: [{
                title: 'Cancel',
                variant: Variant.outlined
            }]
        });
    }

    const handleXML = async (file: File) => {
        const response = await verifyPath(file.name);
        if (!response?.ok) {
            setModal({
                ...modal,
                active: true,
                loadingBar: false,
                title: 'Path Not Found',
                icon: null,
                descriptionTitle: '',
                description: response?.realPath || '',
                buttons: [{
                    title: 'Close',
                }]
            });
            return;
        }
        const parser = new DOMParser();
        let errors = 0;
        const xmlString = await file.text();
        const xml = parser.parseFromString(xmlString, 'text/xml');
        const configs = xml.getElementsByTagName('simulationConfigs')[0];
        if (!configs || configs.childNodes.length === 1) {
            openErrorXMLModal();
            return;
        }
        const logLevelTag = xml.getElementsByTagName('logLevel');
        if (logLevelTag.length <= 0) {
            openErrorXMLModal();
            return;
        }
        let logLevel: string | number = logLevelTag[0].textContent || '';
        if (isNaN(parseFloat(logLevel))) {
            openErrorXMLModal();
            return;
        }
        logLevel = parseInt(logLevel);
        if (logLevel < OpSimulationManagerXmlRequestLogLevelEnum.NUMBER_0 || logLevel > OpSimulationManagerXmlRequestLogLevelEnum.NUMBER_3) {
            openErrorXMLModal();
            return;
        }
        const simulations: Simulation[] = [];
        for (let index = 0; index < configs.childNodes.length; index++) {
            const element = configs.childNodes[index];
            if ((element.nodeName === SIMULATION_CONFIG_XML && (element.textContent?.trim() === '' || element.textContent?.trim() === '\n'))) {
                errors++
            }
            if (element.textContent?.trim() === '' || element.textContent?.trim() === '\n') {
                continue;
            }
            const id = simulations.length + 1;
            const simulation: IGenericRecord = {
                id: id,
                numberOfExecutions: 1,
                selected: false
            };
            let ignored = 0;
            for (let index = 0; index < element.childNodes.length; index++) {
                const child = element.childNodes[index];
                if (child.textContent?.trim() === '' || child.textContent?.trim() === '\n') {
                    ignored++;
                    continue;
                }
                simulation[child.nodeName] = child.textContent;
            }
            const integrity = (element.childNodes.length - ignored);
            if ((integrity > SIMULATION_CONFIG_FIELDS_XML || integrity < SIMULATION_CONFIG_FIELDS_XML)) {
                errors++;
            }
            simulations.push(simulation as Simulation);
        }
        if (errors > 0) {
            openErrorXMLModal();
            return;
        }
        setSimulations(simulations);
        setXmlFile(response.realPath || '');
        setLogLevel(logLevel as OpSimulationManagerXmlRequestLogLevelEnum);
    }

    const runSelectedSimulations = () => {
        setModal({
            ...modal,
            active: true,
            title: 'Run Selected cases',
            description: `All The simulations will run`,
            descriptionTitle: '',
            loadingBar: false,
            buttons: [
                {
                    title: 'Cancel',
                    variant: Variant.outlined
                },
                {

                    title: 'Run and navigate',
                    action: async () => {
                        const response = await runSimulationManager();
                        if (response === 'Simulation already running') {
                            setModal({
                                ...modal,
                                active: true,
                                title: 'Error',
                                description: `A Simulation Is Already Running`,
                                descriptionTitle: '',
                                loadingBar: false,
                                buttons: [
                                    {
                                        title: 'Done',
                                    },
                                ],
                            });
                        }
                    }
                },
            ],
        });

    }

    return (
        <SideWindowBase sx={{ height: '100%' }}>
            <ModalComponent />
            <Box sx={{ display: 'flex', background: 'var(--primary-white)', padding: 3, paddingTop: 5, paddingLeft: 6 }}>
                <RadioGroup sx={{ gap: '.5rem', width: '100%' }} value={configSet} onChange={(event) => setConfigSet(event.target.value)}>
                    <FormControlLabel sx={{ flex: .25 }} value='manual-setup' control={<Radio color='primary' sx={{ height: 10 }} />} label='Manual Setup' />
                    <DivGrouped sx={{ height: 40 }}>
                        <FormControlLabel sx={{ flex: .6 }} value='load-xml' control={<Radio data-testid={'test-id-load-xml'} color='primary' sx={{ height: 10 }} />} label='Load XML config for openPASS simulation manager' />
                        <UploadFile route={xmlFile} type={INPUT_FILE.file} error={(configSet === 'load-xml' && xmlFile === '')} buttonDisabled={configSet !== 'load-xml'} styles={{ flex: 1, marginRight: 2 }} name='xmlFile' handleFiles={handleFiles} handleCustomFile={handleXML} />
                    </DivGrouped>
                    <DivGrouped sx={{ height: 40 }}>
                        <FormControlLabel sx={{ flex: .6 }} value='load-set' control={<Radio color='primary' sx={{ height: 10 }} />} label='Load sets of configs from' />
                        <UploadFile route='' error={(configSet === 'load-set')} buttonDisabled={configSet !== 'load-set'} styles={{ flex: 1, marginRight: 2 }} name='loadConfigSet' handleFiles={handleFiles} />
                    </DivGrouped>
                </RadioGroup>
            </Box>
            <StickyDiv sx={{ position: 'relative', cursor: 'pointer', height: theme.spacing(8), background: 'transparent', border: 'unset', justifyContent: 'flex-end', textTransform: 'uppercase', paddingRight: 5 }}>
                <Link onClick={() => scrollByPixels(200)}>Hide setups</Link>
                <KeyboardArrowUpIcon onClick={() => scrollByPixels(200)} color='primary' />
            </StickyDiv>
            <StickyDiv sx={{ paddingLeft: theme.spacing(3), marginTop: theme.spacing(5), }}>
                <Box display='flex' alignItems='center' gap={4} marginLeft={3} height='100%'>
                    <Box display='flex' data-testid="test-id-box-dropdown-selector" alignItems='center' onClick={handleClick} sx={{ cursor: 'pointer' }}>
                        <FilterCheckbox indeterminate={checkUndeterminated()} color="primary" sx={{ paddingRight: 4 }} name='allSelected' checked={allSelected} />
                        <ArrowDropDownIcon color='primary' />
                    </Box>
                    <Menu
                        id="basic-menu"
                        data-testid="test-id-menu-dropdown"
                        anchorEl={anchorEl}
                        open={open}
                        onClose={handleClose}
                        MenuListProps={{
                            'aria-labelledby': 'basic-button',
                        }}
                    >
                        <MenuItem data-testid="test-id-menu-item-select-all" onClick={() => setUnsetAll(true)}>
                            <Typography sx={{ color: theme.palette.primary.main }}>
                                SELECT ALL SIMULATIONS
                            </Typography>
                        </MenuItem>
                        <MenuItem data-testid="test-id-menu-item-unselect-all" onClick={() => setUnsetAll(false)}>
                            <Typography sx={{ color: theme.palette.primary.main }}>
                                UNSELECT ALL SIMULATIONS
                            </Typography>
                        </MenuItem>
                    </Menu>
                    <SeparatorLine />
                    <Typography>
                        {simulations.filter((sim) => sim.selected).length} / {simulations.length} simulations selected
                    </Typography>
                    <SeparatorLine />
                    <Typography id="logLevel">Log Level</Typography>
                    <FormControl sx={{ m: 1, minWidth: 120 }} size="small">
                        <Select
                            id="demo-simple-select"
                            data-testid="test-id-log-level-selector"
                            value={logLevel}
                            displayEmpty
                            autoWidth
                            onChange={(event) => setLogLevel(
                                event.target.value as OpSimulationManagerXmlRequestLogLevelEnum
                            )}
                        >
                            <MenuItem value={OpSimulationManagerXmlRequestLogLevelEnum.NUMBER_0}>Error</MenuItem>
                            <MenuItem value={OpSimulationManagerXmlRequestLogLevelEnum.NUMBER_1}>Warning</MenuItem>
                            <MenuItem value={OpSimulationManagerXmlRequestLogLevelEnum.NUMBER_2}>Info</MenuItem>
                            <MenuItem value={OpSimulationManagerXmlRequestLogLevelEnum.NUMBER_3}>Debug</MenuItem>
                        </Select>
                    </FormControl>
                </Box>
                <Button color='primary' variant='contained' sx={{ marginRight: 5 }} data-testid="test-id-add-button" onClick={addNewSimulation}>Add New Simulation</Button>
            </StickyDiv>
            <Box display='flex' minHeight={'100%'} flexDirection='column' data-testid="test-id-sim-container" >
                {simulations.map((sim, index) => (
                    <SimulationBox
                        key={sim.id}
                        sim={sim}
                        simName={`Simulation ${index + 1}`}
                        index={index}
                        handleChange={handleChange}
                        removeSimulation={removeSimulation}
                        copySimulation={copySimulation}
                        handleFiles={handleFiles}
                    />
                ))}
            </Box>
            <FooterComponent>
                <Box marginLeft={3} flex={1}>
                    <Button color='primary' data-testid="test-id-clearall-simulations-button" variant='outlined' onClick={cleanUpSimulations}> Clear All </Button>
                </Box>
                <Button color='primary' data-testid="test-id-export-button" variant='contained' disabled={isEnabled()} onClick={handleExport}>Export XML</Button>
                <Button color='primary' data-testid="test-id-run-button" variant='contained' sx={{ marginLeft: 5, marginRight: 7 }} disabled={readyToExecute} onClick={runSelectedSimulations}>Run Selected Cases</Button>
            </FooterComponent>
        </SideWindowBase >
    )
}

export default SimulationManager;