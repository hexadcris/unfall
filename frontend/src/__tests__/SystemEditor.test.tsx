import { screen } from "@testing-library/dom";
import { cleanup, fireEvent, waitFor } from "@testing-library/react";
import { renderWithProviders } from "../utils/test.utils";
import SystemEditor from "../components/interfaces/SystemEditor/SystemEditor";
import userEvent from '@testing-library/user-event'
import { act } from "react-dom/test-utils";
import fetchMock from "jest-fetch-mock";
import components from "../utils/fakeComponentData.json";
jest.mock('react-xarrows', () => {
    return {
        __esModule: true,
        default: () => <span />,
        Xarrow: () => <div />,
        useXarrow: () => jest.fn(),
        Xwrapper: () => <div />,
        updateXarrow: () => jest.fn()
    };
});
describe('Render Test for System Editor Component', () => {
    beforeEach(async () => {
        fetchMock.enableMocks();
        fetchMock.mockResponseOnce(JSON.stringify([]));
        await act(() => renderWithProviders(<SystemEditor />));
    });

    afterEach(() => {
        fetchMock.resetMocks();
        cleanup();
    });
    test('Should render the footer with its content', () => {
        const footer = screen.getByTestId('test-id-footer-systemeditor');
        const clearAllButton = screen.getByTestId('test-id-button-system-clearall');
        const exportButton = screen.getByTestId('test-id-button-system-export');
        expect(footer).toBeInTheDocument();
        expect(clearAllButton).toBeInTheDocument();
        expect(exportButton).toBeInTheDocument();
    });
    test('Should render accordion zone', () => {
        const accordionSideBar = screen.getByTestId('test-id-box-accordion-zone');
        expect(accordionSideBar).toBeInTheDocument();
    });
    test('Should render drag and drop zone', () => {
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        expect(droppableZone).toBeInTheDocument();
    });
    test('Should render system editor info icon', async () => {
        const infoIcon = screen.getByTestId('test-id-icon-system-info');
        expect(infoIcon).toBeInTheDocument();
        userEvent.hover(infoIcon);
        const tip = await screen.findByRole('tooltip');
        expect(tip).toBeInTheDocument();
        expect(tip.textContent).toBe('Drag and drop one component to the dashboard to create and editate systems.');
    });
});

describe('Path To Components tests', () => {
    beforeEach(async () => {
        fetchMock.enableMocks();
    });
    afterEach(() => {
        fetchMock.resetMocks();
        cleanup();
    });
    test('Given all the components are empty accordion zone items should be empty', async () => {
        fetchMock.mockResponseOnce(
            JSON.stringify([])
        );
        await act(async () => renderWithProviders(<SystemEditor />));
        const accordionComponent = screen.getByTestId('test-id-box-accordion-zone');
        expect(accordionComponent).toBeInTheDocument();
        expect(accordionComponent?.textContent).toBe('');
    });
    test('Given api call brings data when the component is loaded then accordion details should have content', async () => {
        fetchMock.mockResponseOnce(
            JSON.stringify([
                {
                    "id": 0,
                    "inputs": [
                        {
                            "cardinality": 1,
                            "id": 0,
                            "title": "Longitudinal Signal",
                            "type": "LongitudinalSignal",
                            "unit": ""
                        }
                    ],
                    "library": "ActionLongitudinalDriver",
                    "position": {
                        "x": 0,
                        "y": 0
                    },
                    "title": "ActionLongitudinalDriver",
                    "type": "Action"
                },
                {
                    "id": 1,
                    "inputs": [
                        {
                            "cardinality": 1,
                            "id": 0,
                            "title": "Desired trajectory",
                            "type": "TrajectoryEvent",
                            "unit": ""
                        }
                    ],
                    "library": "Algorithm_RouteControl",
                    "outputs": [
                        {
                            "id": 0,
                            "title": "Control",
                            "type": "ControlData",
                            "unit": ""
                        }
                    ],
                    "parameters": [
                    ],
                    "position": {
                        "x": 0,
                        "y": 0
                    },
                    "title": "Algorithm_RouteControl",
                    "type": "Algorithm"
                },
                {
                    "id": 2,
                    "library": "ParametersVehicle",
                    "outputs": [
                        {
                            "id": 1,
                            "title": "Vehicle Parameters",
                            "type": "ParametersVehicleSignal",
                            "unit": ""
                        }
                    ],
                    "position": {
                        "x": 0,
                        "y": 0
                    },
                    "title": "ParametersVehicle",
                    "type": "Sensor"
                },
            ])
        );
        await act(async () => renderWithProviders(<SystemEditor />));
        const accordionActions = screen.getByTestId('test-id-accordion-Action');
        const accordionAlgorithms = screen.getByTestId('test-id-accordion-Algorithm');
        const accordionSensors = screen.getByTestId('test-id-accordion-Sensor');
        expect(accordionAlgorithms).toBeInTheDocument();
        expect(accordionSensors).toBeInTheDocument();
        expect(accordionActions).toBeInTheDocument();
        const actionsAccordionDetails = accordionActions.querySelector('.MuiAccordionDetails-root');
        const algorithmsAccordionDetails = accordionAlgorithms.querySelector('.MuiAccordionDetails-root');
        const sensorsAccordionDetails = accordionSensors.querySelector('.MuiAccordionDetails-root');
        expect(actionsAccordionDetails?.textContent).not.toBe('');
        expect(actionsAccordionDetails?.textContent).toBe('ActionLongitudinalDriver');
        expect(algorithmsAccordionDetails?.textContent).not.toBe('');
        expect(algorithmsAccordionDetails?.textContent).toBe('Algorithm_RouteControl');
        expect(sensorsAccordionDetails?.textContent).not.toBe('');
        expect(sensorsAccordionDetails?.textContent).toBe('ParametersVehicle');
    });
    test(
        'GIVEN interface is loading correctly ' +
        'WHEN the api call fails ' +
        'THEN a GET call is rejected', async () => {
            fetchMock.mockReject(Error('Bad Request'))
            await act(async () => renderWithProviders(<SystemEditor />));
            expect(fetchMock).toHaveBeenCalledTimes(1)
            expect(fetchMock).rejects.toThrowError(Error('Bad Request'))
        });
});

describe('System bar and stepper tests', () => {
    const systems = [
        {
            "components": [
            ],
            "connections": [
            ],
            "id": 0,
            "priority": 0,
            "title": "Follow Trajectory",
            "file": "Follow_Trajectory.xml"
        },
        {
            "components": [
            ],
            "connections": [
            ],
            "id": 1,
            "priority": 1,
            "title": "Copy Trajectory",
            "file": "Agent_Copy.xml"
        },
        {
            "components": [
            ],
            "connections": [
            ],
            "id": 2,
            "priority": 2,
            "title": "Copy Trajectory",
            "file": "Agent_Copy.xml"
        },
        {
            "components": [
            ],
            "connections": [
            ],
            "id": 3,
            "priority": 3,
            "title": "Copy Trajectory",
            "file": "Agent_Copy.xml"
        },
        {
            "components": [
            ],
            "connections": [
            ],
            "id": 4,
            "priority": 4,
            "title": "Copy Trajectory",
            "file": "Agent_Copy.xml"
        },
    ];
    beforeEach(async () => {
        fetchMock.enableMocks();
        fetchMock.mockResponseOnce(JSON.stringify([]));
        await act(() => renderWithProviders(<SystemEditor />));
    });

    afterEach(() => {
        fetchMock.resetMocks();
        cleanup();
    });
    test('By default label should say 0 - 0 systems', async () => {
        expect(await screen.findByText(/0 - 0 Systems/i)).toBeInTheDocument();
    });
    test('By default stepper button should be disabled since no system have been loaded', async () => {
        const stepperBackButton = await screen.findByTestId('test-id-button-stepper-back');
        const stepperNextButton = await screen.findByTestId('test-id-button-stepper-next');
        expect(stepperBackButton).toBeInTheDocument();
        expect(stepperNextButton).toBeInTheDocument();
        expect(stepperBackButton).toBeDisabled()
        expect(stepperNextButton).toBeDisabled()
    });
    test('Given User click on Load Systems Button system should load', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(
            JSON.stringify(
                [
                    {
                        "components": [
                        ],
                        "connections": [
                        ],
                        "id": 0,
                        "priority": 0,
                        "title": "Follow Trajectory",
                        "file": "Follow_Trajectory.xml"
                    },
                    {
                        "components": [
                        ],
                        "connections": [
                        ],
                        "id": 1,
                        "priority": 1,
                        "title": "Copy Trajectory",
                        "file": "Agent_Copy.xml"
                    }
                ]
            )
        );
        await act(() => {
            fireEvent.click(loadButton);
        });
        const system = screen.getByTestId('test-id-system-1');
        expect(system).toBeInTheDocument();
        expect(await screen.findByText(/2 - 2 Systems/i)).toBeInTheDocument();
    });
    test('Given User click on Load Systems Button system should but is empty', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        expect(await screen.findByText(/0 - 0 Systems/i)).toBeInTheDocument();
    });
    test(
        'GIVEN user click load systems ' +
        'WHEN the api call fails ' +
        'THEN a GET call is rejected', async () => {
            const loadButton = screen.getByTestId('test-id-button-load');
            fetchMock.mockReject(Error('Bad Request'));
            await act(() => {
                fireEvent.click(loadButton);
            });
            expect(fetchMock).toHaveBeenCalledTimes(2);
            expect(fetchMock).rejects.toThrowError(Error('Bad Request'));
        });
    test('Given User click on Load Systems and theres 5 or more system stepper should be enabled', async () => {
        const stepperBackButton = await screen.findByTestId('test-id-button-stepper-back');
        const stepperNextButton = await screen.findByTestId('test-id-button-stepper-next');
        expect(stepperBackButton).toBeInTheDocument();
        expect(stepperNextButton).toBeInTheDocument();
        expect(stepperBackButton).toBeDisabled()
        expect(stepperNextButton).toBeDisabled()
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify(systems));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const system = screen.getByTestId('test-id-system-1');
        expect(system).toBeInTheDocument();
        expect(await screen.findByText(/4 - 5 Systems/i)).toBeInTheDocument();
        expect(stepperNextButton).toBeEnabled()
    });

    test('Given User click on Load Systems and theres 5 or more systems when clicking stepper should show 2-5 systems', async () => {
        const stepperNextButton = await screen.findByTestId('test-id-button-stepper-next');
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify(systems));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const system = screen.getByTestId('test-id-system-1');
        expect(system).toBeInTheDocument();
        expect(await screen.findByText(/4 - 5 Systems/i)).toBeInTheDocument();
        expect(stepperNextButton).toBeEnabled()
        act(() => {
            fireEvent.click(stepperNextButton);
        });
        expect(await screen.findByText(/5 - 5 Systems/i)).toBeInTheDocument();
    });

    test('Given User click on Load Systems and theres 8 systems when clicking stepper should show 8-8 systems', async () => {
        const stepperNextButton = await screen.findByTestId('test-id-button-stepper-next');
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify(
            [
                {
                    "components": [
                    ],
                    "connections": [
                    ],
                    "id": 0,
                    "priority": 0,
                    "title": "Follow Trajectory",
                    "file": "Follow_Trajectory.xml"
                },
                {
                    "components": [
                    ],
                    "connections": [
                    ],
                    "id": 1,
                    "priority": 1,
                    "title": "Copy Trajectory",
                    "file": "Agent_Copy.xml"
                },
                {
                    "components": [
                    ],
                    "connections": [
                    ],
                    "id": 2,
                    "priority": 2,
                    "title": "Copy Trajectory",
                    "file": "Agent_Copy.xml"
                },
                {
                    "components": [
                    ],
                    "connections": [
                    ],
                    "id": 3,
                    "priority": 3,
                    "title": "Copy Trajectory",
                    "file": "Agent_Copy.xml"
                },
                {
                    "components": [
                    ],
                    "connections": [
                    ],
                    "id": 4,
                    "priority": 4,
                    "title": "Copy Trajectory",
                    "file": "Agent_Copy.xml"
                },
                {
                    "components": [
                    ],
                    "connections": [
                    ],
                    "id": 5,
                    "priority": 5,
                    "title": "Copy Trajectory",
                    "file": "Agent_Copy.xml"
                },
                {
                    "components": [
                    ],
                    "connections": [
                    ],
                    "id": 6,
                    "priority": 6,
                    "title": "Copy Trajectory",
                    "file": "Agent_Copy.xml"
                },
                {
                    "components": [
                    ],
                    "connections": [
                    ],
                    "id": 7,
                    "priority": 7,
                    "title": "Copy Trajectory",
                    "file": "Agent_Copy.xml"
                },
            ]
        ));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const system = screen.getByTestId('test-id-system-1');
        expect(system).toBeInTheDocument();
        expect(await screen.findByText(/4 - 8 Systems/i)).toBeInTheDocument();
        expect(stepperNextButton).toBeEnabled()
        act(() => {
            fireEvent.click(stepperNextButton);
        });
        expect(await screen.findByText(/8 - 8 Systems/i)).toBeInTheDocument();
    });

    test('Given user already advance 1 step in the stepper and theres 5 systems when clicking back should show 4 - 5 Systems again', async () => {
        const stepperBackButton = await screen.findByTestId('test-id-button-stepper-back');
        const stepperNextButton = await screen.findByTestId('test-id-button-stepper-next');
        expect(stepperBackButton).toBeDisabled();
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify(systems));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const system = screen.getByTestId('test-id-system-1');
        expect(system).toBeInTheDocument();
        expect(await screen.findByText(/4 - 5 Systems/i)).toBeInTheDocument();
        expect(stepperNextButton).toBeEnabled()
        act(() => {
            fireEvent.click(stepperNextButton);
        });
        expect(await screen.findByText(/5 - 5 Systems/i)).toBeInTheDocument();
        expect(stepperBackButton).toBeEnabled();
        act(() => {
            fireEvent.click(stepperBackButton);
        });
        expect(await screen.findByText(/4 - 5 Systems/i)).toBeInTheDocument();
        expect(stepperBackButton).toBeDisabled();
    });

    test('Given user already advance 1 step in the stepper and theres 8 systems when clicking back should show 4 - 8 Systems again', async () => {
        const stepperBackButton = await screen.findByTestId('test-id-button-stepper-back');
        const stepperNextButton = await screen.findByTestId('test-id-button-stepper-next');
        expect(stepperBackButton).toBeDisabled();
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([
            {
                "components": [
                ],
                "connections": [
                ],
                "id": 0,
                "priority": 0,
                "title": "Follow Trajectory",
                "file": "Follow_Trajectory.xml"
            },
            {
                "components": [
                ],
                "connections": [
                ],
                "id": 1,
                "priority": 1,
                "title": "Copy Trajectory",
                "file": "Agent_Copy.xml"
            },
            {
                "components": [
                ],
                "connections": [
                ],
                "id": 2,
                "priority": 2,
                "title": "Copy Trajectory",
                "file": "Agent_Copy.xml"
            },
            {
                "components": [
                ],
                "connections": [
                ],
                "id": 3,
                "priority": 3,
                "title": "Copy Trajectory",
                "file": "Agent_Copy.xml"
            },
            {
                "components": [
                ],
                "connections": [
                ],
                "id": 4,
                "priority": 4,
                "title": "Copy Trajectory",
                "file": "Agent_Copy.xml"
            },
            {
                "components": [
                ],
                "connections": [
                ],
                "id": 5,
                "priority": 5,
                "title": "Copy Trajectory",
                "file": "Agent_Copy.xml"
            },
            {
                "components": [
                ],
                "connections": [
                ],
                "id": 6,
                "priority": 6,
                "title": "Copy Trajectory",
                "file": "Agent_Copy.xml"
            },
            {
                "components": [
                ],
                "connections": [
                ],
                "id": 7,
                "priority": 7,
                "title": "Copy Trajectory",
                "file": "Agent_Copy.xml"
            },
        ]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const system = screen.getByTestId('test-id-system-1');
        expect(system).toBeInTheDocument();
        expect(await screen.findByText(/4 - 8 Systems/i)).toBeInTheDocument();
        expect(stepperNextButton).toBeEnabled()
        act(() => {
            fireEvent.click(stepperNextButton);
        });
        expect(await screen.findByText(/8 - 8 Systems/i)).toBeInTheDocument();
        expect(stepperBackButton).toBeEnabled();
        act(() => {
            fireEvent.click(stepperBackButton);
        });
        expect(await screen.findByText(/4 - 8 Systems/i)).toBeInTheDocument();
        expect(stepperBackButton).toBeDisabled();
    });
    test('Given a Simulation when clicking on the chevron down icon Dropdown menu should open', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
    });
    test('Given user clicks on add button should add a new system', async () => {
        const addSystemButton = screen.getByTestId('test-id-add-new-system');
        expect(addSystemButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(addSystemButton);
        });
        const systemAdded = await screen.findByText('System 1');
        expect(systemAdded).toBeInTheDocument();
    });
    test('Given user clicks on add button should add a new system and then tries to load systems shoudl show modal with confirmation', async () => {
        const addSystemButton = screen.getByTestId('test-id-add-new-system');
        expect(addSystemButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(addSystemButton);
        });
        const systemAdded = await screen.findByText('System 1');
        expect(systemAdded).toBeInTheDocument();
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        expect(await screen.findByText(/You already created some systems./i)).toBeInTheDocument();
    });
    test('Given user clicks on add button should add a new system and then tries to load systems shoudl show modal with confirmation', async () => {
        const addSystemButton = screen.getByTestId('test-id-add-new-system');
        expect(addSystemButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(addSystemButton);
        });
        const systemAdded = await screen.findByText('System 1');
        expect(systemAdded).toBeInTheDocument();
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        expect(await screen.findByText(/You already created some systems./i)).toBeInTheDocument();
        const loadSystem = screen.getByTestId('test-id-modalbutton-load');
        await act(() => {
            fireEvent.click(loadSystem);
        });
        expect(await screen.findByText(/Follow Trajectory/i)).toBeInTheDocument();
    });
    test('Given user clicks on load button then on clear all and confirms no system should be shown in the screen', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        expect(await screen.findByText(/Follow Trajectory/i)).toBeInTheDocument();
        const buttonClearAll = screen.getByTestId('test-id-button-system-clearall');
        act(() => {
            fireEvent.click(buttonClearAll);
        });
        const buttonModalClear = screen.getByTestId('test-id-modalbutton-clearall');
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        act(() => {
            fireEvent.click(buttonModalClear);
        });
        expect(droppableZone).toBeEmptyDOMElement();
        expect(await screen.findByText(/0 - 0 Systems/i)).toBeInTheDocument();
    });
    test('Given user clicks on export button without creating or loading system should show error modal ', async () => {
        const buttonExport = screen.getByTestId('test-id-button-system-export');
        act(() => {
            fireEvent.click(buttonExport);
        });
        expect(await screen.findByText(/No System were loaded or created/i)).toBeInTheDocument();
    });
    test('Given user clicks on export button when renaming a system should show modal error ', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const renameButton = screen.getByTestId('test-id-menu-item-rename');
        expect(renameButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(renameButton);
        });
        const buttonExport = screen.getByTestId('test-id-button-system-export');
        act(() => {
            fireEvent.click(buttonExport);
        });
        expect(await screen.findByText(/You are renaming a system at the moment/i)).toBeInTheDocument();
    });
    test('Given user clicks on export button everything should be ok ', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        fetchMock.mockResponseOnce(JSON.stringify({
            "response": "Ok"
        }));
        const buttonExport = screen.getByTestId('test-id-button-system-export');
        act(() => {
            fireEvent.click(buttonExport);
        });
        expect(await screen.findByText(/Systems loaded correctly/i)).toBeInTheDocument();
    });
    test('Given user clicks on export button everything should be ok ', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const system = screen.getByTestId('test-id-system-0');
        expect(system).toHaveClass('selected');
        fetchMock.mockResponseOnce(JSON.stringify({
            "response": "Ok"
        }));
        const buttonExport = screen.getByTestId('test-id-button-system-export');
        act(() => {
            fireEvent.click(buttonExport);
        });
        expect(await screen.findByText(/Systems loaded correctly/i)).toBeInTheDocument();
    });
    test('Given User open menudropdown and clicks delete should open modal and click cancel system should not be deleted', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const deleteItem = screen.getByTestId('test-id-menu-item-delete');
        expect(deleteItem).toBeInTheDocument();
        await act(() => {
            fireEvent.click(deleteItem);
        });
        const modal = screen.getByTestId('test-id-modal-Delete System');
        expect(modal).toBeInTheDocument();
        const cancelButton = await screen.findByText('Cancel');
        expect(cancelButton).toBeInTheDocument();
        await act(() => {
            fireEvent.click(cancelButton);
        });
        const systemAdded = await screen.findByText('Follow Trajectory');
        expect(systemAdded).toBeInTheDocument();
    });
    test('Given User open menudropdown and clicks delete should open modal and click delete system should be deleted', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const deleteItem = screen.getByTestId('test-id-menu-item-delete');
        expect(deleteItem).toBeInTheDocument();
        await act(() => {
            fireEvent.click(deleteItem);
        });
        const modal = screen.getByTestId('test-id-modal-Delete System');
        expect(modal).toBeInTheDocument();
        const deleteButton = screen.getByTestId('test-id-modalbutton-deleteSystem');
        expect(deleteButton).toBeInTheDocument();
        await act(() => {
            fireEvent.click(deleteButton);
        });
        expect(await screen.findByText(/0 - 0 Systems/i)).toBeInTheDocument();
    });
    test('Given user opens dropdown and click rename then rename input should show and then clicks on the confirm and the system name should change', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const renameButton = screen.getByTestId('test-id-menu-item-rename');
        expect(renameButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(renameButton);
        });
        const renameField = screen.getByTestId('test-id-rename-field');
        expect(renameField).toBeInTheDocument();
        const renameInput = renameField.querySelector('input');
        expect(renameInput).toBeInTheDocument();
        expect(renameInput).toHaveProperty('value', 'Follow Trajectory');
        act(() => {
            fireEvent.change(renameInput!, { target: { value: 'newTitle' } });
        });
        expect(renameInput).toHaveProperty('value', 'newTitle');
        const renameConfirm = screen.getByTestId('test-id-rename-confirm');
        expect(renameConfirm).toBeInTheDocument();
        await act(() => {
            fireEvent.click(renameConfirm);
        });
        expect(await screen.findByText(/newTitle/i)).toBeInTheDocument();
    });
    test(`Given user opens dropdown and click rename then rename 
    input should show and then clicks on the confirm with empty name and should show modal with error`, async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const renameButton = screen.getByTestId('test-id-menu-item-rename');
        expect(renameButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(renameButton);
        });
        const renameField = screen.getByTestId('test-id-rename-field');
        expect(renameField).toBeInTheDocument();
        const renameInput = renameField.querySelector('input');
        expect(renameInput).toBeInTheDocument();
        expect(renameInput).toHaveProperty('value', 'Follow Trajectory');
        act(() => {
            fireEvent.change(renameInput!, { target: { value: '' } });
        });
        expect(renameInput).toHaveProperty('value', '');
        const renameConfirm = screen.getByTestId('test-id-rename-confirm');
        expect(renameConfirm).toBeInTheDocument();
        await act(() => {
            fireEvent.click(renameConfirm);
        });
        expect(await screen.findByText(/You have to give a name for the system./i)).toBeInTheDocument();
    });
    test('Given user opens dropdown and click rename then rename input should show and then clicks on the cancel icon the system name should not change', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const renameButton = screen.getByTestId('test-id-menu-item-rename');
        expect(renameButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(renameButton);
        });
        const renameField = screen.getByTestId('test-id-rename-field');
        expect(renameField).toBeInTheDocument();
        const renameInput = renameField.querySelector('input');
        expect(renameInput).toBeInTheDocument();
        expect(renameInput).toHaveProperty('value', 'Follow Trajectory');
        act(() => {
            fireEvent.change(renameInput!, { target: { value: 'newTitle' } });
        });
        expect(renameInput).toHaveProperty('value', 'newTitle');
        const renameCancel = screen.getByTestId('test-id-rename-cancel');
        expect(renameCancel).toBeInTheDocument();
        await act(() => {
            fireEvent.click(renameCancel);
        });
        expect(await screen.findByText(/Follow Trajectory/i)).toBeInTheDocument();
    });
    test('Given user opens dropdown and click rename then click rename again should should modal cannot rename again', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const renameButton = screen.getByTestId('test-id-menu-item-rename');
        expect(renameButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(renameButton);
        });
        act(() => {
            fireEvent.click(renameButton);
        });
        expect(await screen.findByText(/Youre already renaming another system/i)).toBeInTheDocument();
    });
    test('Should hide the accordeon on click ', () => {
        const hideAccordeonButton = screen.getByTestId('test-id-button-hide-accordeon');
        act(() => {
            fireEvent.click(hideAccordeonButton);
        });
        const iconHiddenAccorder = screen.getByTestId('test-id-hidden-accordeon');
        expect(iconHiddenAccorder).toBeInTheDocument();
    });
    test('Should hide the connections on click ', () => {
        const hideConnectionsButton = screen.getByTestId('test-id-button-hide-connections');
        act(() => {
            fireEvent.click(hideConnectionsButton);
        });
        const iconHideConnections = screen.getByTestId('test-id-hide-connections');
        expect(iconHideConnections).toBeInTheDocument();
    });
    test('Given user opens the dropdown menu and click duplicate the system should duplicate ', async () => {
        const loadButton = screen.getByTestId('test-id-button-load');
        fetchMock.mockResponseOnce(JSON.stringify([systems[0]]));
        await act(() => {
            fireEvent.click(loadButton);
        });
        const icon = screen.getByTestId('test-id-system-open-menu-0');
        expect(icon).toBeInTheDocument();
        act(() => {
            fireEvent.click(icon);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const duplicateButton = screen.getByTestId('test-id-menu-item-duplicate');
        expect(duplicateButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(duplicateButton);
        });
        const newSystems = await screen.findAllByText(/Follow Trajectory/i);
        expect(newSystems.length).toBe(2);
    });
});
describe('Target Zone tests', () => {
    const newDataDropped = JSON.stringify({
        "id": 2,
        "type": "Action",
        "library": "Action_Test",
        "title": "Action_Test",
        "schedule": {
            "offset": 0,
            "cycle": 10,
            "response": 0
        },
        "parameters": [
            {
                "id": 0,
                "type": "double",
                "title": "Spring coefficient",
                "unit": "",
                "value": "1200000"
            },
            {
                "id": 1,
                "type": "double",
                "title": "Damper coefficient",
                "unit": "",
                "value": "12000"
            }
        ],
        "inputs": [
            {
                "id": 0,
                "type": "VectorDouble",
                "title": "InertiaForce",
                "unit": "N",
                "cardinality": 1
            }
        ],
        "outputs": [
            {
                "id": 0,
                "type": "VectorDouble",
                "title": "VerticalForce",
                "unit": "N"
            }
        ],
        "position": {
            "x": 679.1953125,
            "y": 68.734375
        },
        "offsetY": 7.265625,
        "offsetX": 50.8046875,
        "height": 22,
        "width": 269,
        "dropped": false,
        "color": "#1D2D53"
    });
    const systemsWithComponents = [
        {
            "schedule": {
                "cycle": 10,
                "offset": 0,
                "priority": 0,
                "response": 0
            },
            "components": [
                {
                    "id": 0,
                    "inputs": [
                        {
                            "cardinality": 1,
                            "id": 0,
                            "title": "Desired trajectory",
                            "type": "TrajectoryEvent",
                            "unit": ""
                        }
                    ],
                    "library": "Dynamics_CopyTrajectory",
                    "position": {
                        "x": 0,
                        "y": 0
                    },
                    "schedule": {
                        "cycle": 10,
                        "offset": 0,
                        "priority": 0,
                        "response": 0
                    },
                    "parameters": [
                        {
                            "id": 0,
                            "title": "Driver aggressiveness",
                            "type": "double",
                            "unit": "",
                            "value": "1"
                        },
                        {
                            "id": 1,
                            "title": "Max. engine power",
                            "type": "double",
                            "unit": "W",
                            "value": "100000"
                        },
                    ],
                    "title": "Dynamics_CopyTrajectory",
                    "type": "Action"
                },
                {
                    "id": 1,
                    "library": "OpenScenarioActions",
                    "outputs": [
                        {
                            "id": 0,
                            "title": "Trajectory Event",
                            "type": "TrajectoryEvent",
                            "unit": ""
                        },
                        {
                            "id": 1,
                            "title": "Custom Lane Change Event",
                            "type": "CustomLaneChangeEvent",
                            "unit": ""
                        },
                        {
                            "id": 2,
                            "title": "Gaze Follower Event",
                            "type": "GazeFollowerEvent",
                            "unit": ""
                        },
                        {
                            "id": 3,
                            "title": "Speed Action Event",
                            "type": "SpeedActionEvent",
                            "unit": ""
                        }
                    ],
                    "inputs": [
                        {
                            "cardinality": 1,
                            "id": 0,
                            "title": "Desired trajectory",
                            "type": "TrajectoryEvent",
                            "unit": ""
                        }
                    ],
                    "position": {
                        "x": 0,
                        "y": 0
                    },
                    "schedule": {
                        "cycle": 0,
                        "offset": 0,
                        "priority": 0,
                        "response": 0
                    },
                    "title": "OpenScenarioActions",
                    "type": "Algorithm"
                }
            ],
            "connections": [
                {
                    "id": 0,
                    "source": {
                        "component": 1,
                        "output": 0
                    },
                    "target": {
                        "component": 0,
                        "input": 0
                    }
                }
            ],
            "id": 0,
            "priority": 0,
            "title": "Follow Trajectory",
            "file": "Follow_Trajectory.xml"
        },
    ];
    beforeEach(async () => {
        fetchMock.enableMocks();
        fetchMock.mockResponseOnce(JSON.stringify(components));
        await act(() => renderWithProviders(<SystemEditor />));
        fetchMock.mockResponseOnce(JSON.stringify(systemsWithComponents));
        const loadSystem = screen.getByTestId('test-id-button-load');
        await act(() => {
            fireEvent.click(loadSystem);
        });
    });

    afterEach(() => {
        fetchMock.resetMocks();
        cleanup();
    });
    test('Should render a component with schedule', async () => {
        const component = screen.getByTestId('test-id-draggable-0');
        expect(component).toBeInTheDocument();
    });
    test('Should change color of component borders', async () => {
        const component = screen.getByTestId('test-id-draggable-0');
        expect(component).toBeInTheDocument();
        const colorButton = screen.getByTestId('test-id-color-button-0');
        expect(colorButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(colorButton);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown-color');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const turquoise = screen.getByTestId('test-id-menu-item-0');
        expect(turquoise).toBeInTheDocument();
        act(() => {
            fireEvent.click(turquoise);
        });
    });
    test('Should show delete modal and not delete', async () => {
        const openMenuButton = screen.getByTestId('test-id-open-menu-0');
        expect(openMenuButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(openMenuButton);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-draggale-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const deleteButton = screen.getByTestId('test-id-menu-item-delete');
        expect(deleteButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(deleteButton);
        });
        const modal = screen.getByTestId('test-id-modal-Delete Component');
        expect(modal).toBeInTheDocument();
        const cancelButton = await screen.findByText(/Cancel/i);
        expect(cancelButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(cancelButton);
        });
        const component = screen.getByTestId('test-id-draggable-0');
        expect(component).toBeInTheDocument();
    });
    test('Should show delete modal and delete', async () => {
        const component = screen.getByTestId('test-id-draggable-0');
        expect(component).toBeInTheDocument();
        const openMenuButton = screen.getByTestId('test-id-open-menu-0');
        expect(openMenuButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(openMenuButton);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-draggale-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const deleteButton = screen.getByTestId('test-id-menu-item-delete');
        expect(deleteButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(deleteButton);
        });
        const modal = screen.getByTestId('test-id-modal-Delete Component');
        expect(modal).toBeInTheDocument();
        const deleteComponentButton = screen.getByTestId('test-id-modalbutton-delete-component');
        expect(deleteComponentButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(deleteComponentButton);
        });
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        expect(droppableZone.childElementCount).toBe(1);
    });
    test('Should show delete modal and delete', async () => {
        const component = screen.getByTestId('test-id-draggable-0');
        expect(component).toBeInTheDocument();
        const openMenuButton = screen.getByTestId('test-id-open-menu-0');
        expect(openMenuButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(openMenuButton);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-draggale-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const duplicateButton = screen.getByTestId('test-id-menu-item-duplicate');
        expect(duplicateButton).toBeInTheDocument();
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        await act(() => {
            fireEvent.click(duplicateButton);
        });
        expect(droppableZone.childElementCount).toBe(4);
    });
    test('Should change the value of the schedule of a component', () => {
        const component = screen.getByTestId('test-id-draggable-0');
        expect(component).toBeInTheDocument();
        const scheduleElement1 = screen.getByTestId('schedule-input-offset-0').querySelector('input');
        expect(scheduleElement1).toBeInTheDocument();
        act(() => {
            fireEvent.change(scheduleElement1!, { target: { value: '20' } });
        });
        expect(scheduleElement1).toHaveProperty('value', '20');
    });
    test('Should be empty when changing since its not a number', () => {
        const component = screen.getByTestId('test-id-draggable-0');
        expect(component).toBeInTheDocument();
        const scheduleElement1 = screen.getByTestId('schedule-input-offset-0').querySelector('input');
        expect(scheduleElement1).toBeInTheDocument();
        act(() => {
            fireEvent.input(scheduleElement1!, { target: { value: 'dsadsa' } });
        });
        expect(scheduleElement1).toHaveProperty('value', '');
    });
    test('Should change the value of the parameter of a component', () => {
        const component = screen.getByTestId('test-id-draggable-0');
        expect(component).toBeInTheDocument();
        const parameter1 = screen.getByTestId('test-id-parameter-0-Driver aggressiveness').querySelector('input');
        expect(parameter1).toBeInTheDocument();
        act(() => {
            fireEvent.change(parameter1!, { target: { value: '20' } });
        });
        expect(parameter1).toHaveProperty('value', '20');
    });
    test('Should delete a connection with modal', async () => {
        const connection = screen.getByTestId('test-id-connection-0');
        expect(connection).toBeInTheDocument();
        act(() => {
            fireEvent.click(connection);
        });
        const modal = screen.getByTestId('test-id-modal-Delete Connection');
        expect(modal).toBeInTheDocument();
        const deleteConnectionButton = screen.getByTestId('test-id-delete-connection-button');
        expect(deleteConnectionButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(deleteConnectionButton);
        });
        await waitFor(() => {
            const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
            expect(droppableZone.childElementCount).toBe(2);
        });
    });
    test('Should cancel deletion of connection with modal', async () => {
        const connection = screen.getByTestId('test-id-connection-0');
        expect(connection).toBeInTheDocument();
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        act(() => {
            fireEvent.click(connection);
        });
        const modal = screen.getByTestId('test-id-modal-Delete Connection');
        expect(modal).toBeInTheDocument();
        const cancelButton = await screen.findByText(/Cancel/i);
        expect(cancelButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(cancelButton);
        });
        expect(droppableZone.childElementCount).toBe(3);
    });
    test('Shoud start a connection', () => {
        const outputButton = screen.getByTestId('test-id-output-button-1-Trajectory Event');
        expect(outputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(outputButton);
        });
        expect(outputButton).toHaveClass('selectedConnection')
    });
    test('Should remove the connection start if click two times on it', () => {
        const outputButton = screen.getByTestId('test-id-output-button-1-Trajectory Event');
        expect(outputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(outputButton);
        });
        expect(outputButton).toHaveClass('selectedConnection');
        act(() => {
            fireEvent.click(outputButton);
        });
        expect(outputButton).not.toHaveClass('selectedConnection');
    });
    test('Should show modal of connection already started if click on two different outputs without finishing a connection', () => {
        const outputButton = screen.getByTestId('test-id-output-button-1-Trajectory Event');
        const outputButtonTwo = screen.getByTestId('test-id-output-button-1-Custom Lane Change Event');
        act(() => {
            fireEvent.click(outputButton);
        });
        expect(outputButton).toHaveClass('selectedConnection');
        act(() => {
            fireEvent.click(outputButtonTwo);
        });
        expect(outputButtonTwo).not.toHaveClass('outputButtonTwo');
        const modal = screen.getByTestId('test-id-modal-Connection Already Started');
        expect(modal).toBeInTheDocument();
    });

    test('Should show modal that says cannot start connection from an input', () => {
        const inputButton = screen.getByTestId('test-id-input-button-1-Desired trajectory');
        expect(inputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(inputButton);
        });
        const modal = screen.getByTestId('test-id-modal-Need to select an output first to start a connection');
        expect(modal).toBeInTheDocument();
    });
    test('Should show modal that says cannot create a connection with the same component', () => {
        const outputButton = screen.getByTestId('test-id-output-button-1-Trajectory Event');
        expect(outputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(outputButton);
        });
        const inputButton = screen.getByTestId('test-id-input-button-1-Desired trajectory');
        expect(inputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(inputButton);
        });
        const modal = screen.getByTestId('test-id-modal-Cannot create a connection with the same component.');
        expect(modal).toBeInTheDocument();
    });
    test('Should show modal that says that there is already a connection on the same output and input', () => {
        const outputButton = screen.getByTestId('test-id-output-button-1-Trajectory Event');
        expect(outputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(outputButton);
        });
        const inputButton = screen.getByTestId('test-id-input-button-0-Desired trajectory');
        expect(inputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(inputButton);
        });
        const modal = screen.getByTestId('test-id-modal-Theres already a connection between this 2 components..');
        expect(modal).toBeInTheDocument();
    });
    test('Should create a new connection', () => {
        const outputButton = screen.getByTestId('test-id-output-button-1-Custom Lane Change Event');
        expect(outputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(outputButton);
        });
        const inputButton = screen.getByTestId('test-id-input-button-0-Desired trajectory');
        expect(inputButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(inputButton);
        });
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        expect(droppableZone.childElementCount).toBe(4);
    });
    test('Should move an element inside the droppable zone', () => {
        const testingElement = screen.getByTestId('test-id-draggable-ActionLongitudinalDriver')
        const mockDataTransfer = {
            clearData: jest.fn(),
            setData: jest.fn(),
        };
        act(() => {
            fireEvent.dragStart(testingElement, { dataTransfer: mockDataTransfer });
        });
        const testingElementDraggable = screen.getByTestId('test-id-draggable-0')
        act(() => {
            fireEvent.dragStart(testingElementDraggable, { dataTransfer: mockDataTransfer });
        });
        const dataToBeReturnedByGetData = JSON.stringify({
            "id": 27,
            "type": "Algorithm",
            "library": "OpenScenarioActions",
            "title": "OpenScenarioActions",
            "schedule": {
                "offset": 0,
                "cycle": 0,
                "response": 0,
                "priority": 0
            },
            "outputs": [
                {
                    "id": 0,
                    "type": "TrajectoryEvent",
                    "title": "Trajectory Event",
                    "unit": ""
                },
                {
                    "id": 1,
                    "type": "CustomLaneChangeEvent",
                    "title": "Custom Lane Change Event",
                    "unit": ""
                },
                {
                    "id": 2,
                    "type": "GazeFollowerEvent",
                    "title": "Gaze Follower Event",
                    "unit": ""
                },
                {
                    "id": 3,
                    "type": "SpeedActionEvent",
                    "title": "Speed Action Event",
                    "unit": ""
                }
            ],
            "position": {
                "x": 19,
                "y": 31
            },
            "dropped": true,
            "color": "#1D2D53",
            "index": 1,
            "offsetY": 28,
            "offsetX": 134,
            "height": 289,
            "width": 318,
            "droppped": true
        });
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        const mockDataTransferDrop = {
            getData: jest.fn().mockReturnValue(dataToBeReturnedByGetData),
            clearData: jest.fn(),
            setData: jest.fn(),
        };
        act(() => {
            fireEvent.drop(droppableZone, { dataTransfer: mockDataTransferDrop, });
        });
        const element = screen.getByTestId('test-id-draggable-1');
        expect(element.style.left).toBe("946px");
        expect(element.style.top).toBe("276px");
    });
    test('Should create a new element when dropped', async () => {
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        const mockDataTransferDrop = {
            getData: jest.fn().mockReturnValue(newDataDropped),
            clearData: jest.fn(),
            setData: jest.fn(),
        };
        await act(() => {
            fireEvent.drop(droppableZone, { dataTransfer: mockDataTransferDrop, });
        });
        expect(droppableZone.childElementCount).toBe(4);
    });
    test('Should show modal for dropping elements without selecting a system', async() => { 
        const system = screen.getByTestId('test-id-system-text-0');
        act(() => {
            fireEvent.click(system);
        });
        const droppableZone = screen.getByTestId('test-id-box-droppable-zone');
        const mockDataTransferDrop = {
            getData: jest.fn().mockReturnValue(newDataDropped),
            clearData: jest.fn(),
            setData: jest.fn(),
        };
        await act(() => {
            fireEvent.drop(droppableZone, { dataTransfer: mockDataTransferDrop, });
        });
        const modal = screen.getByTestId('test-id-modal-Select A System');
        expect(modal).toBeInTheDocument();
     });
});