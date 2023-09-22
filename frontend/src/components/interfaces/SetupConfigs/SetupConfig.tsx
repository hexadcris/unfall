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

import { Box, Button, Grid, InputAdornment, Link, Menu, MenuItem, Typography, useTheme } from "@mui/material";
import { BaseInputStyled, FilterCheckbox, FooterComponent, LightTooltip, SeparatorLine, SideWindowBase, StickyDiv, StyledTab } from "../../../styles/general.styles";
import UploadFile from "../../commonComponents/UploadInput/UploadFile";
import SearchIcon from '@mui/icons-material/Search';
import AgentComponent from "./AgentComponent";
import SimulationIdComponent from "./SimulationIdComponent";
import { useEffect, useRef } from "react";
import { INPUT_FILE } from "../../../index.d";
import { useGlobalContext } from "../../../context/ContextProvider";
import { ISetupConfig, ISimulationId, SIMULATIONS } from "./setupConfigs.d";
import { useForm } from "../../../hooks/useForm";
import ModalComponent from "../../Modal/ModalComponent";
import ArrowDropDownIcon from '@mui/icons-material/ArrowDropDown';
import KeyboardArrowUpIcon from '@mui/icons-material/KeyboardArrowUp';
import { convertToConfigs, exportToSimulation, pathToConvertedCasesPost, sendPCMFile, verifyPath } from "../../../utils/api.util";
import { SystemRef } from "opGUI-client-frontend";

const SetupConfig = () => {
  const { values, handleChangeByName, handleInputChange, handleChange } = useForm<ISetupConfig>({
    simulations: [],
    agents: [
      { name: "Agent 1", file: "", selected: false },
      { name: "Agent 2", file: "", selected: false },
      { name: "Other", file: "", selected: false },
    ],
    pathToPcm: '',
    configSet: 'pcm',
    pathToConvertedCases: '',
    searchTerm: '',
    fileFromList: '',
    anchorEl: null,
    allSelected: false,
    isExportDisabled: true
  });
  const { simulations,
    pathToPcm,
    pathToConvertedCases,
    agents,
    searchTerm,
    fileFromList,
    anchorEl,
    allSelected,
    isExportDisabled } = values;
  const isMounted = useRef(false);
  const { setModal, modal, scrollByPixels, setShowDiscardModal } = useGlobalContext()
  const theme = useTheme();
  const agentContainerRef = useRef<HTMLDivElement>(null);

  const open = Boolean(anchorEl);
  const handleClick = (event: React.MouseEvent<HTMLDivElement>) => {
    handleChangeByName('anchorEl', event.currentTarget);
  };

  const setUnsetAll = (state: boolean) => {
    const newSimulations = simulations.map((sim) => ({ ...sim, selected: state }));
    handleChangeByName('allSelected', state, newSimulations);
  }

  const handleClose = () => {
    handleChangeByName('anchorEl', null);
  };

  const checkUndeterminated = () => {
    const selected = simulations.filter(sim => sim.selected).length;
    return (allSelected && simulations.length > selected)
  }

  const handleFiles = async (name: string, path: string) => {
    if (name === 'pathToPcm') {
      const responseVerify = await verifyPath(path);
      if (!responseVerify?.ok) {
        setModal({
          ...modal,
          active: true,
          loadingBar: false,
          title: 'Path Not Found',
          icon: null,
          descriptionTitle: '',
          description: responseVerify?.realPath || '',
          buttons: [{
            title: 'Close',
          }]
        });
        return;
      }
      let experiments: ISimulationId[] = [];
      const response = await sendPCMFile(responseVerify.realPath || '');
      if (response?.selectedExperiments && response?.selectedExperiments.length > 0) {
        experiments = response?.selectedExperiments.map<ISimulationId>((sim) => ({ id: parseInt(sim), selected: false }))
      }
      handleChangeByName(name, responseVerify.realPath, experiments);
      return;
    }
    const pathToConvertedCasesResponse = await pathToConvertedCasesPost(path);
    if (pathToConvertedCasesResponse?.response) {
      handleChangeByName(name, path);
    }
  }

  const handleCheckedSimulations = (id: number) => {
    const index = simulations.findIndex((sim) => sim.id === id);
    const newSimulations = [...simulations];
    newSimulations[index].selected = !newSimulations[index].selected;
    handleChangeByName(SIMULATIONS, newSimulations);
  }

  const isDisabled = () => {
    return !(simulations.filter((sim) => sim.selected).length > 0);
  }

  const filterData = () => {
    const copySimulations = [...simulations];
    const searchTerms = searchTerm.split(",").map((term) => term.trim());
    const filtered = copySimulations.filter((item) => (searchTerms.includes(item.id.toString()) || item.id.toString().includes(searchTerm)));
    return filtered;
  };

  const handleConvert = async () => {
    const selectedExperiments: Array<string> = simulations
      .filter((sim) => sim.selected)
      .map((sim) => sim.id.toString());
    const selectedAgents: SystemRef[] = agents
      .filter((age) => age.selected)
      .map((age) => ({
        file: age.file,
        name: age.name
      }));
    if (selectedAgents.length === 0) {
      setModal({
        ...modal,
        active: true,
        title: 'Incomplete Data',
        description: `At least one agent should be selected`,
        descriptionTitle: '',
        loadingBar: false,
        buttons: [
          {
            title: 'Close',
          }
        ],
      });
      return;
    }
    const incompleteAgents = selectedAgents.filter(agent => (agent.file === ""))
    if (incompleteAgents.length > 0) {
      setModal({
        ...modal,
        active: true,
        title: 'Incomplete Data',
        description: `The selected agents must have a file loaded.`,
        descriptionTitle: '',
        loadingBar: false,
        buttons: [
          {
            title: 'Close',
          }
        ],
      });
      return;
    }
    const response = await convertToConfigs(selectedExperiments, selectedAgents);
    if (response?.response) {
      handleChange({ isExportDisabled: false });
      setShowDiscardModal(true);
      setModal({
        ...modal,
        active: true,
        title: 'Convert To Configs',
        description: response.response,
        icon: null,
        loadingBar: false,
        descriptionTitle: '',
        buttons: [{
          title: 'Done'
        }]
      });
    }
  }

  const handleExport = async () => {
    const selectedExperiments: Array<string> = simulations
      .filter((sim) => sim.selected)
      .map((sim) => sim.id.toString());
    const response = await exportToSimulation(selectedExperiments);
    if (response?.response) {
      setShowDiscardModal(false);
      setModal({
        ...modal,
        active: true,
        title: 'Export to simulation manager',
        description: response.response,
        icon: null,
        loadingBar: false,
        descriptionTitle: '',
        buttons: [{
          title: 'Done'
        }]
      });
    }
  }

  const processCSV = (str: string, fileName: string, inputName: string) => {
    const rows = str.slice(str.indexOf('\n') + 1).split('\n');
    const updatedSimulations = simulations.map(sim => ({
      ...sim,
      selected: rows.includes(sim.id.toString())
    }));
    handleChangeByName(inputName, fileName, updatedSimulations);
  }

  const handleCsv = (file: File, name: string) => {
    const reader = new FileReader();
    reader.readAsText(file);
    reader.onload = async function (e) {
      if (e.target) {
        const text = e.target.result;
        processCSV(text! as string, file.name, name);
      }
    };
  };

  useEffect(() => {
    if (isMounted.current) {
      filterData();
      return;
    }
    isMounted.current = true;
  }, [searchTerm]);

  useEffect(() => {
    handleClose();
  }, [allSelected]);
  return (
    <SideWindowBase sx={{ height: '90%' }} className="setupConfig" data-testid="test-id-interface-setupconfigs" >
      <ModalComponent />
      <Box ref={agentContainerRef} sx={{ backgroundColor: theme.palette.common.white, borderBottom: `1px solid ${theme.palette.greyScale.main}` }}>
        <Grid container data-testid='test-id-grid-agent-container'>
          {agents && agents.map((data, index) => (
            <AgentComponent
              key={data.name}
              {...data}
              agents={agents}
              handleChange={handleChange}
              index={index}
            />
          ))
          }
        </Grid>
      </Box>
      <StickyDiv sx={{ alignItems: 'flex-start', position: 'relative', paddingTop: 3, height: theme.spacing(16), background: 'transparent', border: 'unset', justifyContent: 'flex-end', textTransform: 'uppercase', paddingRight: 5 }}>
        <Link sx={{ cursor: 'pointer' }} onClick={() => scrollByPixels(200)}>Hide setups</Link>
        <KeyboardArrowUpIcon sx={{ cursor: 'pointer' }} onClick={() => scrollByPixels(200)} color='primary' />
      </StickyDiv>
      <StickyDiv sx={{ position: 'relative', justifyContent: 'unset', backgroundColor: 'transparent', height: theme.spacing(11) }}>
        <StyledTab className="selected" id="pcm">
          <Typography sx={{ fontWeight: 500 }}>
            Based on pcm-database
          </Typography>
        </StyledTab>
        <StyledTab sx={{ borderLeft: 'unset' }}>
          <Typography sx={{ fontWeight: 500 }}>
            From end-to-end
          </Typography>
        </StyledTab>
        <StyledTab sx={{ borderLeft: 'unset' }}>
          <Typography sx={{ fontWeight: 500 }}>
            New configs
          </Typography>
        </StyledTab>
      </StickyDiv>
      <StickyDiv sx={{ position: 'relative', backgroundColor: 'transparent' }}>
        <Box marginLeft={4} sx={{ display: 'inline-flex', alignItems: 'center', gap: 5, padding: 1, paddingRight: 4, width: '100%' }}>
          <Box>
            <Typography sx={{ flex: .1 }}> Path to PCM database </Typography>
          </Box>
          <UploadFile type={INPUT_FILE.file} accept=".db" route={pathToPcm} error={pathToPcm === ''} styles={{ flex: .6 }} name='pathToPcm' handleFiles={handleFiles} />
        </Box>
      </StickyDiv>
      <StickyDiv sx={{ backgroundColor: theme.palette.grey[100] }}>
        <Box display='flex' alignItems='center' gap={4} marginLeft={3} paddingLeft={2} height='100%'>
          <Box display='flex' data-testid="test-id-box-dropdown-selector" alignItems='center' onClick={handleClick} sx={{ cursor: 'pointer' }}>
            <FilterCheckbox indeterminate={checkUndeterminated()} data-testid='test-id-checkbox-setup-filter' color="primary" name='allSelected' checked={allSelected} />
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
            {simulations.filter((sim) => sim.selected).length} / {simulations.length} IDs Selected
          </Typography>
          <SeparatorLine />
          <BaseInputStyled value={searchTerm} name="searchTerm" onChange={handleInputChange} placeholder="Search by ID" data-testid='test-id-input-setup-filter' startAdornment={
            <InputAdornment position="start">
              <SearchIcon sx={{ color: 'black' }} />
            </InputAdornment>
          }
            endAdornment={
              <LightTooltip title={<>To add more than one ID add ','<br />between the different IDs.</>}>
                <InputAdornment sx={{ cursor: 'pointer' }} position="end">
                  <i style={{}} className="icon-mask primary info-seatch"></i>
                </InputAdornment>
              </LightTooltip>

            }
          />
        </Box>
        <Box sx={{ display: 'inline-flex', alignItems: 'center', gap: 5, padding: 1, paddingRight: 4 }}>
          <Box>
            <Typography sx={{ flex: .1 }}> Filter from list </Typography>
          </Box>
          <UploadFile route={fileFromList} data-testid="test-id-button-filter" accept=".csv" buttonDisabled={simulations.length === 0} type={INPUT_FILE.file} handleCustomFile={handleCsv} error={false} styles={{ flex: 1 }} name='fileFromList' handleFiles={() => { }} />
        </Box>
      </StickyDiv>
      {
        simulations && simulations.length === 0 &&
        <Typography data-testid="test-id-label-empty-ids" sx={{ paddingLeft: 5, paddingTop: 5 }}>
          No IDs loaded.
        </Typography>
      }
      <Box data-testid="test-id-container-simulation-id" paddingTop={8} minHeight={'100%'} >
        {(simulations && simulations.length > 0) && filterData().map((data, index) => (
          <SimulationIdComponent key={data.id} {...data} index={index} handleChange={handleCheckedSimulations} />
        ))
        }
      </Box>
      <FooterComponent data-testid="test-id-footer-setup">
        <Box marginLeft={3} flex={1}>
          <Box sx={{ display: 'inline-flex', alignItems: 'center', gap: 5, padding: 1, paddingRight: 4, width: '90%' }}>
            <Box>
              <Typography sx={{ flex: .1 }}> Path to converted cases </Typography>
            </Box>
            <UploadFile route={pathToConvertedCases} error={false} styles={{ flex: 1 }} name='pathToConvertedCases' handleFiles={handleFiles} />
          </Box>
        </Box>
        <Button color='primary' onClick={handleConvert} data-testid="test-id-button-convert" variant='contained' name='convert' disabled={isDisabled()}>Convert to configs</Button>
        <Button color='primary' onClick={handleExport} data-testid="test-id-button-export" variant='contained' disabled={isExportDisabled} sx={{ marginLeft: 5, marginRight: 7 }} >Export to simulation</Button>
      </FooterComponent>
    </SideWindowBase >
  )
}

export default SetupConfig;