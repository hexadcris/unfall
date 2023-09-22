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


import { cleanup, fireEvent, screen, waitFor, within } from "@testing-library/react";
import SetupConfig from "../components/interfaces/SetupConfigs/SetupConfig";
import { renderWithProviders } from "../utils/test.utils";
import { act } from "react-dom/test-utils";
import fetchMock from "jest-fetch-mock";
describe('Render Tests for SetupConfigs interface', () => {
    beforeEach(async () => {
        fetchMock.enableMocks();
        await act(() => renderWithProviders(<SetupConfig />));
    });
    afterEach(() => {
        fetchMock.resetMocks();
        cleanup();
    });
    test('Given its the first time they enter should render the component', async () => {
        const mainComponent = screen.getByTestId('test-id-interface-setupconfigs');
        expect(mainComponent).toBeInTheDocument();
    });
    test('Should render path to Pcm database option by default', () => {
        const uploadFilePcm = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        expect(uploadFilePcm).toBeInTheDocument();
        expect(uploadFilePcm?.value).toBe('');
    });
    test('should render at least 1 agent as the initial state', () => {
        const agentContainer = screen.getByTestId('test-id-grid-agent-container');
        expect(agentContainer).toBeInTheDocument();
        expect(agentContainer).not.toBeEmptyDOMElement();
    });
    test('Should render no id found as the initial state', () => {
        const emptyIdText = screen.queryByText('No IDs loaded.');
        expect(emptyIdText).toBeInTheDocument();
    });
    test('Should render sticky div section complete', () => {
        const checkboxFilter = screen.getByTestId('test-id-checkbox-setup-filter');
        const uploadFileFilter = screen.getByTestId('test-id-input-fileFromList').querySelector('input');
        const searchBarFilter = screen.getByTestId('test-id-input-setup-filter').querySelector('input');
        expect(checkboxFilter).toBeInTheDocument();
        expect(checkboxFilter).not.toBeChecked();
        expect(searchBarFilter).toBeInTheDocument();
        expect(searchBarFilter?.value).toBe('');
        expect(uploadFileFilter).toBeInTheDocument();
        expect(searchBarFilter?.value).toBe('');
    });
    test('Should render footer with all of its options', () => {
        const footer = screen.getByTestId('test-id-footer-setup');
        const convertToConfigsButton = screen.getByTestId('test-id-button-convert');
        const uploadFileFooter = screen.getByTestId('test-id-input-pathToConvertedCases');
        const exportToSimulationButton = screen.getByTestId('test-id-button-export');
        expect(footer).toBeInTheDocument();
        expect(footer).not.toBeEmptyDOMElement();
        expect(convertToConfigsButton).toBeInTheDocument();
        expect(convertToConfigsButton).toBeDisabled();
        expect(exportToSimulationButton).toBeInTheDocument();
        expect(exportToSimulationButton).toBeDisabled();
        expect(uploadFileFooter).toBeInTheDocument();
    });
});
describe('Functional Test for Setup Configs Component', () => {
    beforeEach(async () => {
        fetchMock.enableMocks();
        await act(() => renderWithProviders(<SetupConfig />));
    });
    afterEach(() => {
        fetchMock.resetMocks();
        cleanup();
    });
    test('Given User select a .db file inpout should not be empty and should not have class Mui-error ', async () => {
        // Mocking the response for the verifyPath function
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));

        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
        });
    });
    test('Given User select a .db file and path is not fould should show modal path not found', async () => {
        // Mocking the response for the verifyPath function
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": false,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));

        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            const modal = screen.getByTestId('test-id-modal-Path Not Found');
            expect(modal).toBeInTheDocument();
        });
    });
    test('Given User select a .xml file for and agent and path is not fould should show modal path not found', async () => {
        // Mocking the response for the verifyPath function
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": false,
            "realPath": "os/workspace/path/to/folder/dummy.xml"
        }));

        const fileName = 'os/workspace/path/to/folder/dummy.xml';
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-agentFile-0');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            const modal = screen.getByTestId('test-id-modal-Path Not Found');
            expect(modal).toBeInTheDocument();
        });
    });
    test('Given User select a .xml file for an agent input should not be empty and should not have class Mui-error ', async () => {
        // Mocking the response for the verifyPath function
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.xml"
        }));

        const fileName = 'os/workspace/path/to/folder/dummy.xml';
        const inputContainer = screen.getByTestId('test-id-input-agentFile-0');
        const inputFile = screen.getByTestId('test-id-input-agentFile-0').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-agentFile-0');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
        });
    });
    test('Given user clicks on checkbox should open dropdown menu ', () => { 
        const dropdownSelector = screen.getByTestId('test-id-box-dropdown-selector');
        expect(dropdownSelector).toBeInTheDocument();
        act(() => {
            fireEvent.click(dropdownSelector);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
     });
    test('Given user clicks on checkbox should open dropdown menu and select select all options all experiments should be selected', async () => { 
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["123123"]
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const simulationIdContainer = screen.getByTestId('test-id-container-simulation-id');
        const empyState = screen.getByTestId('test-id-label-empty-ids');
        expect(empyState).toBeInTheDocument();
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        expect(simulationIdContainer).toBeEmptyDOMElement();
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
            expect(simulationIdContainer).not.toBeEmptyDOMElement();
        });
        const dropdownSelector = screen.getByTestId('test-id-box-dropdown-selector');
        expect(dropdownSelector).toBeInTheDocument();
        act(() => {
            fireEvent.click(dropdownSelector);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const selectAll = screen.getByTestId('test-id-menu-item-select-all');
        act(() => {
            fireEvent.click(selectAll);
        });
        const checkbox = within(screen.getByTestId('test-simid-checkbox-123123')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        expect(checkbox).toBeChecked()
     });
    test('Given user clicks on checkbox should open dropdown menu and select unselect all options all experiments should be empty', async () => { 
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["123123"]
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const simulationIdContainer = screen.getByTestId('test-id-container-simulation-id');
        const empyState = screen.getByTestId('test-id-label-empty-ids');
        expect(empyState).toBeInTheDocument();
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        expect(simulationIdContainer).toBeEmptyDOMElement();
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
            expect(simulationIdContainer).not.toBeEmptyDOMElement();
        });
        const dropdownSelector = screen.getByTestId('test-id-box-dropdown-selector');
        expect(dropdownSelector).toBeInTheDocument();
        act(() => {
            fireEvent.click(dropdownSelector);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const selectAll = screen.getByTestId('test-id-menu-item-select-all');
        act(() => {
            fireEvent.click(selectAll);
        });
        const checkbox = within(screen.getByTestId('test-simid-checkbox-123123')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        expect(checkbox).toBeChecked();
        const unselectAll = screen.getByTestId('test-id-menu-item-unselect-all');
        act(() => {
            fireEvent.click(unselectAll);
        });
        expect(checkbox).not.toBeChecked();
     });
    test('Given .db file is selected should render multiple Simulation Id Component', async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["123123"]
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const simulationIdContainer = screen.getByTestId('test-id-container-simulation-id');
        const empyState = screen.getByTestId('test-id-label-empty-ids');
        expect(empyState).toBeInTheDocument();
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        expect(simulationIdContainer).toBeEmptyDOMElement();
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
            expect(simulationIdContainer).not.toBeEmptyDOMElement();
        });
    });
    test('Given .db file and file is empty should render empty list', async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": []
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const simulationIdContainer = screen.getByTestId('test-id-container-simulation-id');
        const empyState = screen.getByTestId('test-id-label-empty-ids');
        expect(empyState).toBeInTheDocument();
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        expect(simulationIdContainer).toBeEmptyDOMElement();
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
            expect(empyState).toBeInTheDocument();
            expect(simulationIdContainer).toBeEmptyDOMElement();
        });
    });
    test('Given simulation are charged with the pcm database when clicking on one should be selected', async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["123123"]
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const simulationIdContainer = screen.getByTestId('test-id-container-simulation-id');
        const empyState = screen.getByTestId('test-id-label-empty-ids');
        expect(empyState).toBeInTheDocument();
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        expect(simulationIdContainer).toBeEmptyDOMElement();
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
        });
        const checkbox = within(screen.getByTestId('test-simid-checkbox-123123')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        act(() => {
            fireEvent.click(checkbox);
        });
        expect(checkbox).toBeChecked();
    });
    test('Given path to convert cases input is clicked and folder is selected input should not be empty  ', async () => {
        global.showDirectoryPicker = jest.fn(() => {
            return { name: 'folderName' }
        });
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "empty": true,
            "realPath": "os/workspace/path/to/folder/test1"
        }));
        fetchMock.mockResponseOnce(JSON.stringify({
            "response": "location setted correctly"
        }));
        const testButton = screen.getByTestId('test-id-input-button-pathToConvertedCases');
        act(() => {
            fireEvent.click(testButton);
        });
        await waitFor(() => {
            const inputFile = screen.getByTestId('test-id-input-pathToConvertedCases').querySelector('input');
            expect(inputFile).toHaveAttribute('value', 'os/workspace/path/to/folder/test1');
        });
    });
    test(`Given .db file is selected and at least 1 sim is select 
        and path to converted cases is selected actions buttons should be enabled`, async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["123123"]
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
        });
        const checkbox = within(screen.getByTestId('test-simid-checkbox-123123')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        act(() => {
            fireEvent.click(checkbox);
        });
        expect(checkbox).toBeChecked();
        const convertButton = screen.getByTestId('test-id-button-convert');
        expect(convertButton).toBeInTheDocument();
        expect(convertButton).toBeEnabled();
    });
    test(`Given .db file is selected and at least 1 sim is select 
        and path to converted cases is selected actions buttons should be enabled and should show modal error because theres no agents selected.`, async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["123123"]
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
        });
        const checkbox = within(screen.getByTestId('test-simid-checkbox-123123')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        act(() => {
            fireEvent.click(checkbox);
        });
        expect(checkbox).toBeChecked();
        const convertButton = screen.getByTestId('test-id-button-convert');
        expect(convertButton).toBeInTheDocument();
        expect(convertButton).toBeEnabled();
        act(() => {
            fireEvent.click(convertButton);
        });
         await waitFor(async() => {
            expect(await screen.findByText(/At least one agent should be selected/i)).toBeInTheDocument();
        });
    });
    test(`Given .db file is selected and at least 1 sim is select 
        and path to converted cases is selected actions buttons should be enabled and should show modal error because theres no agents selected.`, async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["123123"]
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
        });
        const checkbox = within(screen.getByTestId('test-simid-checkbox-123123')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        act(() => {
            fireEvent.click(checkbox);
        });
        expect(checkbox).toBeChecked();
        const convertButton = screen.getByTestId('test-id-button-convert');
        expect(convertButton).toBeInTheDocument();
        expect(convertButton).toBeEnabled();
        const checkboxAgent = within(screen.getByTestId('test-id-checkbox-agent-Agent 1')).getByRole('checkbox')
        act(() => {
            fireEvent.click(checkboxAgent);
        });
        expect(checkboxAgent).toBeChecked();
        act(() => {
            fireEvent.click(convertButton);
        });
         await waitFor(async() => {
            expect(await screen.findByText(/The selected agents must have a file loaded./i)).toBeInTheDocument();
        });
    });
    const enableButton = async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["123123"]
        }));
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const inputContainer = screen.getByTestId('test-id-input-pathToPcm');
        const inputFile = screen.getByTestId('test-id-input-pathToPcm').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
        });
        const checkbox = within(screen.getByTestId('test-simid-checkbox-123123')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        act(() => {
            fireEvent.click(checkbox);
        });
        expect(checkbox).toBeChecked();
        const checkboxAgent = within(screen.getByTestId('test-id-checkbox-agent-Agent 1')).getByRole('checkbox')
        act(() => {
            fireEvent.click(checkboxAgent);
        });
        expect(checkboxAgent).toBeChecked();
        const fileNameAgent = 'os/workspace/path/to/folder/dummy.xml';
        const inputContainerAgent = screen.getByTestId(`test-id-input-agentFile-0`);
        const inputFileAgent = screen.getByTestId('test-id-input-agentFile-0').querySelector('input');
        const inputFileHiddenAgent = screen.getByTestId('test-id-input-hidden-agentFile-0');
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.xml"
        }));
        act(() => {
            fireEvent.change(inputFileHiddenAgent, { target: { files: [{ name: fileNameAgent }] } });
        });
        await waitFor(() => {
            expect(inputContainerAgent).not.toHaveClass('Mui-error');
            expect(inputFileAgent).toHaveAttribute('value', fileNameAgent);
        });
    }
    test('Given data is complete config modal should open ', async () => {
        await enableButton();

        const convertButton = screen.getByTestId('test-id-button-convert');
        expect(convertButton).toBeInTheDocument();
        expect(convertButton).toBeEnabled();
        fetchMock.mockResponseOnce(JSON.stringify({
            "response": "location setted correctly"
        }));
        act(() => {
            fireEvent.click(convertButton);
        });
        await waitFor(() => {
            const modal = screen.getByTestId('test-id-modal-Convert To Configs');
            expect(modal).toBeInTheDocument();
        });
    });
    test('Given data is complete export modal should open ', async () => {
        await enableButton();
        const convertButton = screen.getByTestId('test-id-button-convert');
        expect(convertButton).toBeInTheDocument();
        expect(convertButton).toBeEnabled();
        fetchMock.mockResponseOnce(JSON.stringify({
            "response": "location setted correctly"
        }));
        act(() => {
            fireEvent.click(convertButton);
        });
        await waitFor(async () => {
            const modal = screen.getByTestId('test-id-modal-Convert To Configs');
            expect(modal).toBeInTheDocument();
            const exportButton = screen.getByTestId('test-id-button-export');
            expect(exportButton).toBeInTheDocument();
            expect(exportButton).toBeEnabled();
            fetchMock.mockResponseOnce(JSON.stringify({
                "response": "location setted correctly"
            }));
            act(() => {
                fireEvent.click(exportButton);
            });
            await waitFor(() => {
                const modal = screen.getByTestId('test-id-modal-Export to simulation manager');
                expect(modal).toBeInTheDocument();
            });
        });
    });
    test('filterData is called when searchTerm changes', () => {
        const searchTermInput = screen.getByTestId('test-id-input-setup-filter').querySelector('input');
        act(() => {
            fireEvent.change(searchTermInput!, { target: { value: '1123' } });
        });
        expect(searchTermInput).toHaveProperty('value', '1123');

        act(() => {
            fireEvent.change(searchTermInput!, { target: { value: '1123, 1098' } });
        });
        expect(searchTermInput).toHaveProperty('value', '1123, 1098');
    });
    test('Given user input filter SimulationIdComponent should change', async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["1123", "1098"]
        }));
        const simulationIdContainer = screen.getByTestId('test-id-container-simulation-id');
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const searchTermInput = screen.getByTestId('test-id-input-setup-filter').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(simulationIdContainer).not.toBeEmptyDOMElement();
        });
        expect(simulationIdContainer.childElementCount).toBe(2);
        act(() => {
            fireEvent.change(searchTermInput!, { target: { value: '1123' } });
        });
        expect(searchTermInput).toHaveProperty('value', '1123');
        expect(simulationIdContainer.childElementCount).toBe(1);
    });
    test('Given no .db file is selected filter from list button should be disabled', () => {
        const uploadFileFilter = screen.getByTestId('test-id-input-fileFromList').querySelector('input');
        const uploadButton = screen.getByTestId('test-id-input-button-fileFromList');
        expect(uploadFileFilter).toBeInTheDocument();
        expect(uploadButton).toBeDisabled();
        expect(uploadButton).toBeInTheDocument();
    });
    test('Given there are simulations and filter file its empty nothing should change', async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["1123", "1098"]
        }));
        const simulationIdContainer = screen.getByTestId('test-id-container-simulation-id');
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(simulationIdContainer).not.toBeEmptyDOMElement();
        });
        const inputFileHiddenFilter = screen.getByTestId('test-id-input-hidden-fileFromList');
        const file = new File([''], 'dummy.csv', { type: 'text/csv' });
        act(() => {
            fireEvent.change(inputFileHiddenFilter, { target: { files: [file] } });
        });
        await waitFor(() => {
            const checkbox = within(screen.getByTestId('test-simid-checkbox-1123')).getByRole('checkbox');
            const checkboxSimulation = within(screen.getByTestId('test-simid-checkbox-1098')).getByRole('checkbox');
            expect(checkbox).toBeInTheDocument();
            expect(checkbox).not.toBeChecked();
            expect(checkboxSimulation).toBeInTheDocument();
            expect(checkboxSimulation).not.toBeChecked();
        });
    });
    test('Given There are simulations i should be able to filter from file', async () => {
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.db"
        }));
        // Mocking the response for the sendPCMFile function
        fetchMock.mockResponseOnce(JSON.stringify({
            "selectedExperiments": ["1123", "1098"]
        }));
        const simulationIdContainer = screen.getByTestId('test-id-container-simulation-id');
        const fileName = 'os/workspace/path/to/folder/dummy.db';
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-pathToPcm');
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [{ name: fileName }] } });
        });
        await waitFor(() => {
            expect(simulationIdContainer).not.toBeEmptyDOMElement();
        });
        const inputFileHiddenFilter = screen.getByTestId('test-id-input-hidden-fileFromList');
        const file = new File(['simulations\n1123\n1098'], 'dummy.csv', { type: 'text/csv' });
        act(() => {
            fireEvent.change(inputFileHiddenFilter, { target: { files: [file] } });
        });
        await waitFor(() => {
            const checkbox = within(screen.getByTestId('test-simid-checkbox-1123')).getByRole('checkbox');
            const checkboxSimulation = within(screen.getByTestId('test-simid-checkbox-1098')).getByRole('checkbox');
            expect(checkbox).toBeInTheDocument();
            expect(checkbox).toBeChecked();
            expect(checkboxSimulation).toBeInTheDocument();
            expect(checkboxSimulation).toBeChecked();
        });
    });
});