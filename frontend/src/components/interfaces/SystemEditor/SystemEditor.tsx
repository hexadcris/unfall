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

import { Box } from "@mui/system";
import { FooterComponent, LightTooltip, SideWindowBase, StickyDiv, StyledTab } from "../../../styles/general.styles";
import { Button, Link, Menu, MenuItem, TextField, Typography, useTheme } from "@mui/material";
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import DeleteOutlineOutlinedIcon from '@mui/icons-material/DeleteOutlineOutlined';
import ControlPointDuplicateOutlinedIcon from '@mui/icons-material/ControlPointDuplicateOutlined';
import ChevronLeftIcon from '@mui/icons-material/ChevronLeft';
import KeyboardArrowUpIcon from '@mui/icons-material/KeyboardArrowUp';
import ChevronRightIcon from '@mui/icons-material/ChevronRight';
import RemoveRedEyeOutlinedIcon from '@mui/icons-material/RemoveRedEyeOutlined';
import VisibilityOffOutlinedIcon from '@mui/icons-material/VisibilityOffOutlined';
import WarningAmberIcon from '@mui/icons-material/WarningAmber';
import AddIcon from '@mui/icons-material/Add';
import { useForm } from "../../../hooks/useForm";
import { ConnectionList, DragComponent, ISystemEditor, System } from "./systemEditor.d";
import { useEffect } from "react";
import { AddIconBox, HideButton, StepperButton } from "../../../styles/systemEditor.styles";
import { useGlobalContext } from "../../../context/ContextProvider";
import { ColorVariant, Variant } from "../../../index.d";
import ModalComponent from "../../Modal/ModalComponent";
import TargetZone from "./TargetZone";
import ComponentsSideMenu from "./ComponentsSideMenu";
import { ComponentUI as Component, ComponentUITypeEnum, SystemUI } from 'opGUI-client-frontend';
import { exportSystemsApi, getComponentsApi, loadSystemsApi } from "../../../utils/api.util";
import CheckIcon from '@mui/icons-material/Check';
import CloseIcon from '@mui/icons-material/Close';

const SystemEditor = () => {
  const theme = useTheme();
  const { scrollByPixels, setModal, modal, setShowDiscardModal } = useGlobalContext()
  const { values, handleChange } = useForm<ISystemEditor>({
    pathToComponents: '',
    componentsOriginal: [],
    components: {
      actions: [],
      algorithms: [],
      sensors: []
    },
    systems: [],
    deletedSystems: [],
    hideConnections: false,
    selectedSystem: null,
    currentStartIndex: 0,
    currentEndIndex: 0,
    showAccordion: true,
    availableElements: [],
    connections: [],
    newSystemName: "",
    renamingSystemId: null
  });
  const {
    systems,
    hideConnections,
    selectedSystem,
    currentStartIndex,
    currentEndIndex,
    showAccordion,
    availableElements,
    connections,
    deletedSystems,
    renamingSystemId,
    newSystemName,
    componentsOriginal
  } = values;


  const groupByType = (arr: Component[]): Record<ComponentUITypeEnum, Component[]> => {
    return arr.reduce((acc, curr) => {
      if (!acc[curr.type]) {
        acc[curr.type] = [];
      }
      acc[curr.type].push(curr);
      return acc;
    }, {} as Record<ComponentUITypeEnum, Component[]>);
  }

  const getComponents = async () => {
    let data = await getComponentsApi();
    if (!data) {
      setModal({
        ...modal,
        active: true,
        title: 'An error ocurred while loading the components',
        descriptionTitle: '',
        description: `Please try to communicate with an admin to solve the problem.`,
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
    data = data?.map((data, index) => ({ ...data, id: index }));
    handleChange({ components: groupByType(data), componentsOriginal: data });
  }

  const handleCloseMenu = () => {
    handleChange({ systems: systems.map((data) => ({ ...data, anchorEl: null })) });
  }

  const handleNextClick = () => {
    const remainingSystems = systems.length - currentEndIndex;
    if (remainingSystems >= 4) {
      handleChange({ currentStartIndex: currentStartIndex + 4, currentEndIndex: currentEndIndex + 4 });
      return
    }
    if (remainingSystems > 0) {
      handleChange({
        currentStartIndex: currentStartIndex + remainingSystems,
        currentEndIndex: currentEndIndex + remainingSystems
      });
    }
  };

  const exportSystems = async () => {
    if (systems.length === 0) {
      setModal({
        ...modal,
        active: true,
        title: 'No System were loaded or created',
        descriptionTitle: '',
        description: `You should first load or create a system`,
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
    if (renamingSystemId !== null) {
      setModal({
        ...modal,
        active: true,
        title: 'You are renaming a system at the moment',
        descriptionTitle: '',
        description: `Either complete or cancel the renaming process to continue.`,
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
    const systemsExport = [...systems, ...deletedSystems];
    const systemsExportFinal: SystemUI[] = systemsExport
      .filter(sys => !(typeof sys.id === 'string' && sys.deleted))
      .map((sys, index) => ({
        ...sys,
        id: index,
        connections: (selectedSystem !== null && sys.id === selectedSystem.id) ? connections : (sys.connections) ? sys.connections : [],
        components: (selectedSystem !== null && sys.id === selectedSystem.id) ? availableElements.map((comp) => ({
          ...comp,
          position: {
            x: Math.round(comp.position.x),
            y: Math.round(comp.position.y),
          },
          schedule: {
            offset: comp.schedule.offset ? parseFloat(comp.schedule.offset) : 0,
            cycle: comp.schedule.cycle ? parseFloat(comp.schedule.cycle) : 0,
            response: comp.schedule.response ? parseFloat(comp.schedule.response) : 0,
            priority: comp.schedule.priority ? parseFloat(comp.schedule.priority) : 0,
          }
        })) : sys.components,
      }));
    const res = await exportSystemsApi(systemsExportFinal);
    if (res?.response) {
      setModal({
        ...modal,
        active: true,
        title: 'Systems loaded correctly',
        descriptionTitle: '',
        description: res.response,
        loadingBar: false,
        icon: null,
        buttons: [
          {
            title: 'Close',
          }
        ],
      });
      setShowDiscardModal(false);
    }
  }

  const handleBackClick = () => {
    if (currentStartIndex >= 4) {
      handleChange({ currentStartIndex: currentStartIndex - 4, currentEndIndex: currentEndIndex - 4 });
      return
    }
    handleChange({ currentStartIndex: 0, currentEndIndex: Math.min(4, systems.length) });
  };

  const clearAllSystems = () => {
    setModal({
      ...modal,
      active: true,
      title: 'Clear Systems',
      descriptionTitle: 'All The Systems Will Be Removed.',
      description: `After this action, the systems cannot be recovered.`,
      loadingBar: false,
      icon: WarningAmberIcon,
      iconVariant: ColorVariant.warning,
      buttons: [
        {
          title: 'Cancel',
          variant: Variant.outlined
        },
        {
          title: 'Clear Systems',
          action: () => {
            handleChange({ systems: [], selectedSystem: null, currentStartIndex: 0, currentEndIndex: 0, availableElements: [], connections: [] });
            setShowDiscardModal(false);
          },
          testId: 'test-id-modalbutton-clearall'
        },
      ],
    });
  }

  const handleLoadSystems = async () => {
    const data = (await loadSystemsApi()) as System[];
    let system: System | null = null;
    let frontAvailableElement: any = [];
    let frontConnections: ConnectionList[] = [];
    if (data && data.length > 0) {
      system = data[0];
      frontAvailableElement = system.components.map((component) => {
        const idOriginal = componentsOriginal.find((comp) => comp.title === component.title);
        return {
          ...component,
          id: (idOriginal) ? idOriginal.id : component.id,
          dropped: true,
          color: (component as DragComponent).color || theme.palette.primary.main,
        }
      });
      if (system.connections) {
        frontConnections = system.connections.map((con) => {
          const originalElementOutput = system?.components.find((comp) => comp.id === con.source.component);
          const originalElementInput = system?.components.find((comp) => comp.id === con.target.component);
          const newIndexElementOutput = frontAvailableElement.findIndex((com: Component) => com.title === originalElementOutput?.title);
          const newElementOutput = frontAvailableElement.find((com: Component) => com.title === originalElementOutput?.title);
          const newIndexElementInput = frontAvailableElement.findIndex((com: Component) => com.title === originalElementInput?.title);
          const newElementInput = frontAvailableElement.find((com: Component) => com.title === originalElementInput?.title);
          return {
            ...con,
            indexComponentInput: newIndexElementInput,
            indexComponentOutput: newIndexElementOutput,
            source: {
              ...con.source,
              component: newElementOutput?.id
            },
            target: {
              ...con.target,
              component: newElementInput?.id
            }
          }
        }) as ConnectionList[];
      }

    }
    handleChange({
      systems: data.map((sys, index) => ({ ...sys, id: index })) || [],
      currentEndIndex: Math.min(4, data.length),
      selectedSystem: system,
      availableElements: frontAvailableElement,
      connections: frontConnections
    });
  }

  const loadSystems = async () => {
    try {
      if (systems.length > 0) {
        setModal({
          ...modal,
          active: true,
          title: 'You already created some systems.',
          descriptionTitle: `The systems that you created will be deleted by loading.`,
          description: `After this action, the systems cannot be recovered.`,
          loadingBar: false,
          icon: WarningAmberIcon,
          iconVariant: ColorVariant.warning,
          buttons: [
            {
              title: 'Cancel',
              variant: Variant.outlined
            },
            {
              title: 'Load Systems',
              action: async () => {
                await handleLoadSystems();
              },
              testId: 'test-id-modalbutton-load'
            },
          ],
        });
        return;
      }
      await handleLoadSystems();

    } catch (error) {
      console.log(error);
    }
  }

  const duplicateSystem = (system: System) => {
    if (selectedSystem !== null && selectedSystem.id === system.id) {
      system.connections = connections;
      system.components = availableElements;
    }
    const newFile = system.file?.split('.');
    const newSystems = systems.map((data) => ({ ...data, anchorEl: null }));
    newSystems.push({
      ...system,
      id: generateUnique(),
      title: system.title,
      anchorEl: null,
      file: `${newFile ? newFile[0] : system.title}.xml`,
      deleted: false
    });
    let newCurrentEndIndex = currentEndIndex;
    if (newCurrentEndIndex <= 4) {
      newCurrentEndIndex = Math.min(4, newSystems.length);
    }
    handleChange({
      systems: newSystems,
      currentEndIndex: newCurrentEndIndex
    });
    setShowDiscardModal(true);
  }

  const generateUnique = () => {
    const dateString = Date.now().toString(36);
    const random = Math.random().toString(36).substring(2);
    return dateString + random;
  };

  const addNewSystem = () => {
    const newSystems = systems.map((data) => ({ ...data, anchorEl: null }));
    newSystems.push({
      id: generateUnique(),
      title: `System ${systems.length + 1}`,
      anchorEl: null,
      priority: 0,
      components: [],
      connections: [],
      deleted: false,
      file: `System ${systems.length + 1}.xml`
    });
    let newCurrentEndIndex = currentEndIndex;
    if (newCurrentEndIndex <= 4) {
      newCurrentEndIndex = Math.min(4, newSystems.length);
    }
    handleChange({
      systems: newSystems,
      currentEndIndex: newCurrentEndIndex
    });
    setShowDiscardModal(true);
  }

  const deleteSystem = (system: System) => {
    setModal({
      ...modal,
      active: true,
      title: 'Delete System',
      descriptionTitle: `The system ${system.title} will be deleted`,
      description: `The system will be removed, to complete the action you should export the systems.`,
      loadingBar: false,
      icon: WarningAmberIcon,
      iconVariant: ColorVariant.warning,
      buttons: [
        {
          title: 'Cancel',
          variant: Variant.outlined
        },
        {
          title: 'Delete System',
          action: () => {
            const newSystems = systems.map((data) => ({ ...data, anchorEl: null }));
            const index = newSystems.findIndex(sys => sys.id === system.id);
            const newDeletedSystems = [...deletedSystems];
            newDeletedSystems.push({ ...system, deleted: true, anchorEl: null });
            newSystems.splice(index, 1);
            if (currentEndIndex > 4) {
              handleChange({
                systems: newSystems,
                currentStartIndex: currentStartIndex - 4,
                currentEndIndex: currentEndIndex - 4,
                deletedSystems: newDeletedSystems,
                selectedSystem: (system.id === selectedSystem?.id) ? null : selectedSystem,
                availableElements: (system.id === selectedSystem?.id) ? [] : availableElements,
                connections: (system.id === selectedSystem?.id) ? [] : connections
              });
              return;
            }
            handleChange({
              systems: newSystems,
              currentEndIndex: Math.min(4, newSystems.length),
              deletedSystems: newDeletedSystems,
              selectedSystem: (system.id === selectedSystem?.id) ? null : selectedSystem,
              availableElements: (system.id === selectedSystem?.id) ? [] : availableElements,
              connections: (system.id === selectedSystem?.id) ? [] : connections
            });
          },
          testId: 'test-id-modalbutton-deleteSystem'
        },
      ],
    });
  }

  const handleSelectSystem = (system: System) => {
    const frontAvailableElement = system.components.map((component) => {
      const idOriginal = componentsOriginal.find((comp) => comp.title === component.title);
      return {
        ...component,
        id: (idOriginal) ? idOriginal.id : component.id,
        dropped: true,
        color: (component as DragComponent).color || theme.palette.primary.main,
      }
    });
    let frontConnections: ConnectionList[] = [];
    if (system.connections) {
      frontConnections = system.connections.map((con) => {
        const originalElementOutput = system.components.find((comp) => comp.id === con.source.component);
        const originalElementInput = system.components.find((comp) => comp.id === con.target.component);
        const newIndexElementOutput = frontAvailableElement.findIndex((com) => com.title === originalElementOutput?.title);
        const newElementOutput = frontAvailableElement.find((com) => com.title === originalElementOutput?.title);
        const newIndexElementInput = frontAvailableElement.findIndex((com) => com.title === originalElementInput?.title);
        const newElementInput = frontAvailableElement.find((com) => com.title === originalElementInput?.title);
        return {
          ...con,
          indexComponentInput: newIndexElementInput,
          indexComponentOutput: newIndexElementOutput,
          source: {
            ...con.source,
            component: newElementOutput?.id
          },
          target: {
            ...con.target,
            component: newElementInput?.id
          }
        }
      }) as ConnectionList[];
    }

    if (selectedSystem === null) {
      handleChange({ selectedSystem: system, availableElements: frontAvailableElement, connections: frontConnections });
      return;
    }
    const newSystems = [...systems];
    const index = newSystems.findIndex(sys => sys.id === selectedSystem.id);
    newSystems[index].components = availableElements;
    newSystems[index].connections = connections;
    if (selectedSystem.id === system.id) {
      handleChange({ systems: newSystems, selectedSystem: null, availableElements: [], connections: [] });
      return;
    }
    handleChange({ systems: newSystems, selectedSystem: system, availableElements: frontAvailableElement, connections: frontConnections });
  };

  const handleOpenMenu = (event: React.MouseEvent<SVGElement>, id: number | string) => {
    const newSystems = [...systems];
    const index = newSystems.findIndex(sys => sys.id === id);
    newSystems[index]['anchorEl'] = event.currentTarget;
    handleChange({ systems: newSystems });
  };

  const handleRename = (system: System) => {
    if (renamingSystemId === null) {
      handleChange({
        renamingSystemId: system.id,
        newSystemName: system.title,
        systems: systems.map((data) => ({ ...data, anchorEl: null }))
      });
      return;
    }
    setModal({
      ...modal,
      active: true,
      description: 'Youre already renaming another system',
      icon: null,
      descriptionTitle: '',
      title: "Cannot rename 2 systems at the same time.",
      buttons: [{ title: "Close" }]
    });
  };

  const handleConfirmRename = (system: System) => {
    if (newSystemName === "") {
      setModal({
        ...modal,
        active: true,
        description: 'The name cannot be empty',
        icon: null,
        descriptionTitle: '',
        title: "You have to give a name for the system.",
        buttons: [{ title: "Close" }]
      });
      return;
    }
    const newSystems = systems.map((data) => ({ ...data, anchorEl: null }));
    const index = newSystems.findIndex((sys) => sys.id === system.id);
    newSystems[index].title = newSystemName;
    handleChange({
      renamingSystemId: null,
      newSystemName: "",
      systems: newSystems
    });
    setShowDiscardModal(true);
  };

  const handleCancelRename = () => {
    handleChange({ renamingSystemId: null, newSystemName: "" });
  };

  useEffect(() => {
    getComponents();
  }, []);

  return (
    <SideWindowBase className="system-editor" sx={{ height: '100%', position: 'unset' }}>
      <ModalComponent />
      <StickyDiv sx={{ borderBottom: 'unset', padding: 4, position: 'relative', display: 'flex', backgroundColor: theme.palette.common.white }}>
        <Button onClick={loadSystems} color='primary' data-testid="test-id-button-load" variant='contained' name='convert' >Load Systems</Button>
      </StickyDiv>
      <StickyDiv sx={{ position: 'relative', cursor: 'pointer', alignItems: 'unset', paddingTop: theme.spacing(2), height: theme.spacing(12), background: 'transparent', border: 'unset', justifyContent: 'flex-end', textTransform: 'uppercase', paddingRight: 5 }}>
        <Link onClick={() => scrollByPixels(120)} >Hide setups</Link>
        <KeyboardArrowUpIcon onClick={() => scrollByPixels(120)} color='primary' />
      </StickyDiv>
      <StickyDiv sx={{ justifyContent: 'unset', backgroundColor: theme.palette.primary.accent1Medium, height: theme.spacing(11) }}>
        <Box sx={{
          display: 'flex',
          width: showAccordion ? '80%' : '99%',
          flexDirection: 'row',
          height: '100%',
          alignItems: 'center',
          transition: 'width 0.5s ease-in-out',
        }}>
          {systems.slice(currentStartIndex, currentStartIndex + 4).map((data) => (
            <StyledTab
              className={selectedSystem?.id === data.id ? 'selected' : ''}
              data-testid={`test-id-system-${data.id}`}
              sx={{ zIndex: 1, paddingX: 5, cursor: 'default', gap: 2 }}
              key={data.id}>
              {renamingSystemId === data.id ? (
                <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center' }}>
                  <TextField
                    value={newSystemName}
                    onChange={(e) => handleChange({ newSystemName: e.target.value })}
                    placeholder={data.title}
                    variant="outlined"
                    size="small"
                    data-testid="test-id-rename-field"
                  />
                  <CheckIcon data-testid="test-id-rename-confirm" onClick={() => handleConfirmRename(data)} style={{ cursor: 'pointer', color: 'green' }} />
                  <CloseIcon data-testid="test-id-rename-cancel" onClick={handleCancelRename} style={{ cursor: 'pointer', color: 'red' }} />
                </Box>
              ) : (
                <Typography data-testid={`test-id-system-text-${data.id}`} onClick={() => handleSelectSystem(data)} sx={{ fontWeight: 500 }}>
                  {data.title}
                </Typography>
              )}
              <KeyboardArrowDownOutlinedIcon data-testid={`test-id-system-open-menu-${data.id}`} onClick={(event: React.MouseEvent<SVGElement>) => handleOpenMenu(event, data.id)} color="primary" sx={{ cursor: 'pointer', zIndex: 3 }} />
              <Menu
                id="basic-menu"
                data-testid="test-id-menu-dropdown"
                anchorEl={data.anchorEl}
                open={Boolean(data.anchorEl)}
                onClose={handleCloseMenu}
                MenuListProps={{
                  'aria-labelledby': 'basic-button',
                }}
              >
                <MenuItem data-testid="test-id-menu-item-rename" onClick={() => handleRename(data)}>
                  <i className="icon-mask primary rename"></i>
                  <Typography sx={{ color: theme.palette.primary.main, paddingLeft: 2 }} >
                    RENAME
                  </Typography>
                </MenuItem>
                <MenuItem data-testid="test-id-menu-item-duplicate" onClick={() => duplicateSystem(data)}>
                  <ControlPointDuplicateOutlinedIcon color="primary" />
                  <Typography sx={{ color: theme.palette.primary.main, paddingLeft: 2 }}>
                    DUPLICATE
                  </Typography>
                </MenuItem>
                <MenuItem data-testid="test-id-menu-item-delete" onClick={() => deleteSystem(data)}>
                  <DeleteOutlineOutlinedIcon color="primary" />
                  <Typography sx={{ color: theme.palette.primary.main, paddingLeft: 2 }}>
                    DELETE
                  </Typography>
                </MenuItem>
              </Menu>
            </StyledTab>
          ))
          }
          <LightTooltip placement="bottom-start" title="Creating a new system..">
            <AddIconBox data-testid="test-id-add-new-system" onClick={() => addNewSystem()}>
              <AddIcon sx={{
                height: theme.spacing(6),
                width: theme.spacing(6),
              }} />
            </AddIconBox>
          </LightTooltip>
          <Box position='absolute' display='flex' justifyContent='end' alignItems='center' width='inherit' gap={theme.spacing(2)}>
            <Typography variant="body1">
              {currentEndIndex} - {systems.length} Systems
            </Typography>
            <StepperButton data-testid="test-id-button-stepper-back" onClick={handleBackClick} disabled={currentStartIndex === 0}><ChevronLeftIcon /></StepperButton>
            <StepperButton data-testid="test-id-button-stepper-next" onClick={handleNextClick} disabled={currentEndIndex >= systems.length}><ChevronRightIcon /></StepperButton>
          </Box>
        </Box>
        <Box sx={{
          height: '100%',
          width: '20%',
          borderLeft: `1px solid ${theme.palette.greyScale.main}`,
          display: 'flex',
          alignItems: 'center',
          paddingRight: theme.spacing(4),
          justifyContent: 'flex-end',
          transition: 'transform 0.5s ease-in-out',
          transform: showAccordion ? 'translateX(0%)' : 'translateX(95%)'
        }}>
          <LightTooltip placement="left" title="Drag and drop one component to the dashboard to create and editate systems.">
            <InfoOutlinedIcon data-testid="test-id-icon-system-info" sx={{ cursor: 'pointer' }} />
          </LightTooltip>
        </Box>
      </StickyDiv>

      <Box sx={{ display: 'flex', width: '100%', height: 'calc(100% - 40px);', position: 'relative' }} data-testid="test-id-box-systemzone">
        <HideButton
          sx={{
            transition: 'all 0.5s ease-in-out',
            right: showAccordion ? '20%' : theme.spacing(3.5)
          }}
          onClick={() => handleChange({ showAccordion: !showAccordion })}
          data-testid="test-id-button-hide-accordeon"
        >
          {showAccordion ?
            (<ChevronRightIcon data-testid="test-id-show-accordeon" sx={{ color: theme.palette.common.white }} />) :
            (<ChevronLeftIcon data-testid="test-id-hidden-accordeon" sx={{ color: theme.palette.common.white }} />)
          }
        </HideButton>
        <HideButton
          sx={{
            transition: 'all 0.5s ease-in-out',
            background: 'transparent',
            right: showAccordion ? '23%' : theme.spacing(12)
          }}
          onClick={() => handleChange({ hideConnections: !hideConnections })}
          data-testid="test-id-button-hide-connections"
        >
          {!hideConnections ?
            (<RemoveRedEyeOutlinedIcon data-testid="test-id-show-connections" sx={{ color: theme.palette.primary.main }} />) :
            (<VisibilityOffOutlinedIcon data-testid="test-id-hide-connections" sx={{ color: theme.palette.primary.main }} />)
          }
        </HideButton>
        <TargetZone handleChange={handleChange} {...values} />
        <ComponentsSideMenu handleChange={handleChange} {...values} />
      </Box>
      <FooterComponent sx={{ justifyContent: 'space-between', height: theme.spacing(12), zIndex: 3 }} data-testid="test-id-footer-systemeditor">
        <Button color='primary' data-testid="test-id-button-system-clearall" variant='contained' sx={{ marginLeft: 7, }} name='convert' onClick={() => clearAllSystems()} >Clear All</Button>
        <Button color='primary' data-testid="test-id-button-system-export" variant='contained' sx={{ marginRight: 7 }} name="exportSystems" onClick={exportSystems}>Export </Button>
      </FooterComponent>
    </SideWindowBase>
  )
}

export default SystemEditor
