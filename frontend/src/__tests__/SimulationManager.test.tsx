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
import { OpSimulationManagerXml, SimulationConfig } from "../pcm-simulation";
import SimulationManager from "../components/interfaces/SimulationManager/SimulationManager";
import { act } from "react-dom/test-utils";
import { Simulation } from "..";
import fetchMock from "jest-fetch-mock";
import { renderWithProviders } from "../utils/test.utils";

describe('Test Suit For Simulation Manager Component', () => {
    beforeEach(() => {
        renderWithProviders(<SimulationManager />);
    });

    afterEach(() => {
        cleanup();
    });

    test('Type checking', async () => {
        const opSimulationManager: OpSimulationManagerXml = {
            logLevel: '0',
            libraries: [],
            logFileSimulationManager: '',
            simulation: '',
            simulationConfigs: {
                logFileSimulation: '',
                configurations: '',
                results: '',
                numberOfExecutions: 1
            }
        }
        expect(opSimulationManager.logLevel).toEqual(expect.any(String));
        expect(parseInt(opSimulationManager.logLevel || '0')).toBeGreaterThanOrEqual(0);
        expect(parseInt(opSimulationManager.logLevel || '0')).toBeLessThanOrEqual(4);
        expect(opSimulationManager.libraries).toEqual(expect.any(Array));
        expect(opSimulationManager.logFileSimulationManager).toEqual(expect.any(String));
        expect(opSimulationManager.simulation).toEqual(expect.any(String));
        expect(opSimulationManager.simulationConfigs).toEqual(expect.any(Object));
    });

    test(`Given an empty manual setup WHEN the user provides all the
        parameters then the interfaces are validated`, () => {
        const configSims: SimulationConfig = {
            logFileSimulation: 'opSimulation.log',
            configurations: 'configs',
            results: 'results',
            numberOfExecutions: 1
        };
        const xmlManagerValues: OpSimulationManagerXml = {
            logLevel: 0,
            logFileSimulationManager: 'opSimulationManager.log',
            libraries: 'modules',
            simulation: 'opSimulation',
            simulationConfigs: configSims
        }
        expect(xmlManagerValues).toEqual({
            logLevel: 0,
            logFileSimulationManager: "opSimulationManager.log",
            libraries: "modules",
            simulation: "opSimulation",
            simulationConfigs: {
                logFileSimulation: "opSimulation.log",
                configurations: "configs",
                results: "results",
                numberOfExecutions: 1
            }
        });
    });

    test('should not display any simulations and button should be disabled', () => {
        const exportXmlButton = screen.getByTestId('test-id-export-button');
        const simulationContainer = screen.getByTestId('test-id-sim-container');
        expect(exportXmlButton).toBeInTheDocument();
        expect(exportXmlButton).toBeDisabled();
        expect(simulationContainer).toBeInTheDocument();
        expect(simulationContainer).toHaveTextContent('');
    });

    test('Should change log level when selected', async () => {
        const selectEl = screen.getByTestId('test-id-log-level-selector');
        expect(await screen.findByText(/Error/i)).toBeInTheDocument();
        const button = within(selectEl).getByRole('button');
        fireEvent.mouseDown(button);
        const listbox = within(screen.getByRole('presentation')).getByRole(
            'listbox'
        );
        const options = within(listbox).getAllByRole('option');
        const optionValues = options.map((li) => li.getAttribute('data-value'));
        expect(optionValues).toEqual(['0', '1', '2', '3']);
        act(() => {
            fireEvent.click(options[1]);
        });
        expect(await screen.findByText(/Warning/i)).toBeInTheDocument();
    });

    test('Should add sim on click', async () => {
        const simulationContainer = screen.getByTestId('test-id-sim-container');
        const addSimButton = screen.getByTestId('test-id-add-button');
        expect(simulationContainer).toBeInTheDocument();
        expect(simulationContainer).toHaveTextContent('');
        fireEvent.click(addSimButton);
        await waitFor(() => {
            const simulationBox = screen.getByTestId('test-sim-1');
            expect(simulationBox).toBeInTheDocument();
            expect(simulationContainer).not.toHaveTextContent(''); // Verify new children
        });
    });

    test('should create and select a simulation', async () => {
        const addSimButton = screen.getByTestId('test-id-add-button');
        act(() => {
            fireEvent.click(addSimButton);
        });
        const simulationBox = screen.getByTestId('test-sim-1');
        expect(simulationBox).toBeInTheDocument();
        const checkbox = within(screen.getByTestId('test-id-checkbox-1')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        act(() => {
            fireEvent.click(checkbox);
        });
        expect(checkbox).toBeChecked()
    });

    test('Given some syms are selected button should be enabled', () => {
        const addSimButton = screen.getByTestId('test-id-add-button');
        act(() => {
            fireEvent.click(addSimButton);
        });
        const simulationBox = screen.getByTestId('test-sim-1');
        expect(simulationBox).toBeInTheDocument();
        const checkbox = within(screen.getByTestId('test-id-checkbox-1')).getByRole('checkbox')
        expect(checkbox).toBeInTheDocument();
        act(() => {
            fireEvent.click(checkbox);
        });
        const exportXmlButton = screen.getByTestId('test-id-export-button');
        expect(exportXmlButton).toBeEnabled();
    });

});
describe('Load XML config for openPASS simulation manager', () => {
    beforeEach(async () => {
        fetchMock.enableMocks();
        renderWithProviders(<SimulationManager />);
    });

    afterEach(() => {
        fetchMock.resetMocks();
        cleanup();
    });

    test('Load Button should be disabled if radio not selected', () => {
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeDisabled();
    });
    test('When Load XML option is selected input file should have class Mui-error', () => {
        const radio = screen.getByTestId('test-id-load-xml');
        act(() => {
            fireEvent.click(radio);
        });
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeEnabled();
        const inputFile = screen.getByTestId('test-id-input-xmlFile');
        expect(inputFile).toHaveClass('Mui-error');
    });
    test('should display path not found modal if file is not found inside workspace folder', async () => {
        const xmlContent = `<opSimulationManager>
        <logLevel>3</logLevel>
        <logFileSimulationManager>opSimulationManager.log</logFileSimulationManager>
        <simulation>opSimulation</simulation>
        <libraries>modules</libraries>
        <simulationConfigs>
          <simulationConfig>
            <logFileSimulation>opSimulation.log</logFileSimulation>
            <configurations>configs</configurations>
            <results>results</results>
          </simulationConfig>
          <simulationConfig>
            <logFileSimulation>opSimulation.log2</logFileSimulation>
            <configurations>configs</configurations>
            <results>results2</results>
          </simulationConfig>
        </simulationConfigs>
      </opSimulationManager>
      `;
        const mockFile = new File([xmlContent], 'dummy.xml', {
            type: 'text/xml',
        });
        const radio = screen.getByTestId('test-id-load-xml');
        act(() => {
            fireEvent.click(radio);
        });
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeEnabled();
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-xmlFile');
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": false,
            "realPath": "os/workspace/path/to/folder/dummy.xml",
            "empty": false
        }));
        File.prototype.text = function () {
            return Promise.resolve(xmlContent);
        };
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [mockFile] } });
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-Path Not Found');
            expect(modalNotFound).toBeInTheDocument();
        });
    });
    test('Should Display modal error when simulationConfigs tag is not present in xml file', async () => {
        const xmlContent = `<opSimulationManager>
        <logLevel>3</logLevel>
        <logFileSimulationManager>opSimulationManager.log</logFileSimulationManager>
        <simulation>opSimulation</simulation>
        <libraries>modules</libraries>
      </opSimulationManager>
      `;
        const mockFile = new File([xmlContent], 'dummy.xml', {
            type: 'text/xml',
        });
        const radio = screen.getByTestId('test-id-load-xml');
        act(() => {
            fireEvent.click(radio);
        });
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeEnabled();
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-xmlFile');
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.xml",
            "empty": false
        }));
        File.prototype.text = function () {
            return Promise.resolve(xmlContent);
        };
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [mockFile] } });
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-File Format Error');
            expect(modalNotFound).toBeInTheDocument();
        });
    });
    test('Should Display modal error when logLevel tag is not present in xml file', async () => {
        const xmlContent = `<opSimulationManager>
        <logFileSimulationManager>opSimulationManager.log</logFileSimulationManager>
        <simulation>opSimulation</simulation>
        <libraries>modules</libraries>
        <simulationConfigs>
          <simulationConfig>
            <logFileSimulation>opSimulation.log</logFileSimulation>
            <configurations>configs</configurations>
            <results>results</results>
          </simulationConfig>
          <simulationConfig>
            <logFileSimulation>opSimulation.log2</logFileSimulation>
            <configurations>configs</configurations>
            <results>results2</results>
          </simulationConfig>
        </simulationConfigs>
      </opSimulationManager>
      `;
        const mockFile = new File([xmlContent], 'dummy.xml', {
            type: 'text/xml',
        });
        const radio = screen.getByTestId('test-id-load-xml');
        act(() => {
            fireEvent.click(radio);
        });
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeEnabled();
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-xmlFile');
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.xml",
            "empty": false
        }));
        File.prototype.text = function () {
            return Promise.resolve(xmlContent);
        };
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [mockFile] } });
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-File Format Error');
            expect(modalNotFound).toBeInTheDocument();
        });
    });
    test('Should Display modal error when logLevel tag is not a number present in xml file', async () => {
        const xmlContent = `<opSimulationManager>
        <logFileSimulationManager>opSimulationManager.log</logFileSimulationManager>
        <logLevel>dashdah</logLevel>
        <simulation>opSimulation</simulation>
        <libraries>modules</libraries>
        <simulationConfigs>
          <simulationConfig>
            <logFileSimulation>opSimulation.log</logFileSimulation>
            <configurations>configs</configurations>
            <results>results</results>
          </simulationConfig>
          <simulationConfig>
            <logFileSimulation>opSimulation.log2</logFileSimulation>
            <configurations>configs</configurations>
            <results>results2</results>
          </simulationConfig>
        </simulationConfigs>
      </opSimulationManager>
      `;
        const mockFile = new File([xmlContent], 'dummy.xml', {
            type: 'text/xml',
        });
        const radio = screen.getByTestId('test-id-load-xml');
        act(() => {
            fireEvent.click(radio);
        });
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeEnabled();
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-xmlFile');
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.xml",
            "empty": false
        }));
        File.prototype.text = function () {
            return Promise.resolve(xmlContent);
        };
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [mockFile] } });
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-File Format Error');
            expect(modalNotFound).toBeInTheDocument();
        });
    });
    test('Should Display modal error when logLevel tag is not a valid number present in xml file', async () => {
        const xmlContent = `<opSimulationManager>
        <logFileSimulationManager>opSimulationManager.log</logFileSimulationManager>
        <logLevel>10</logLevel>
        <simulation>opSimulation</simulation>
        <libraries>modules</libraries>
        <simulationConfigs>
          <simulationConfig>
            <logFileSimulation>opSimulation.log</logFileSimulation>
            <configurations>configs</configurations>
            <results>results</results>
          </simulationConfig>
          <simulationConfig>
            <logFileSimulation>opSimulation.log2</logFileSimulation>
            <configurations>configs</configurations>
            <results>results2</results>
          </simulationConfig>
        </simulationConfigs>
      </opSimulationManager>
      `;
        const mockFile = new File([xmlContent], 'dummy.xml', {
            type: 'text/xml',
        });
        const radio = screen.getByTestId('test-id-load-xml');
        act(() => {
            fireEvent.click(radio);
        });
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeEnabled();
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-xmlFile');
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.xml",
            "empty": false
        }));
        File.prototype.text = function () {
            return Promise.resolve(xmlContent);
        };
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [mockFile] } });
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-File Format Error');
            expect(modalNotFound).toBeInTheDocument();
        });
    });
    test('Should Display modal error when logLevel simulationConfig doesnt have all its atributes : logFileSimulation, configurations, results', async () => {
        const xmlContent = `<opSimulationManager>
        <logFileSimulationManager>opSimulationManager.log</logFileSimulationManager>
        <logLevel>1</logLevel>
        <simulation>opSimulation</simulation>
        <libraries>modules</libraries>
        <simulationConfigs>
          <simulationConfig>
            <logFileSimulation>opSimulation.log</logFileSimulation>
            <configurations>configs</configurations>
          </simulationConfig>
          <simulationConfig>
            <logFileSimulation>opSimulation.log2</logFileSimulation>
            <configurations>configs</configurations>
            <results>results2</results>
          </simulationConfig>
        </simulationConfigs>
      </opSimulationManager>
      `;
        const mockFile = new File([xmlContent], 'dummy.xml', {
            type: 'text/xml',
        });
        const radio = screen.getByTestId('test-id-load-xml');
        act(() => {
            fireEvent.click(radio);
        });
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeEnabled();
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-xmlFile');
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.xml",
            "empty": false
        }));
        File.prototype.text = function () {
            return Promise.resolve(xmlContent);
        };
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [mockFile] } });
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-File Format Error');
            expect(modalNotFound).toBeInTheDocument();
        });
    });
    test('Should display one or multiple simulations given input file is already selected', async () => {
        const xmlContent = `<opSimulationManager>
        <logLevel>3</logLevel>
        <logFileSimulationManager>opSimulationManager.log</logFileSimulationManager>
        <simulation>opSimulation</simulation>
        <libraries>modules</libraries>
        <simulationConfigs>
          <simulationConfig>
            <logFileSimulation>opSimulation.log</logFileSimulation>
            <configurations>configs</configurations>
            <results>results</results>
          </simulationConfig>
          <simulationConfig>
            <logFileSimulation>opSimulation.log2</logFileSimulation>
            <configurations>configs</configurations>
            <results>results2</results>
          </simulationConfig>
        </simulationConfigs>
      </opSimulationManager>
      `;
        const mockFile = new File([xmlContent], 'dummy.xml', {
            type: 'text/xml',
        });
        const fileName = 'os/workspace/path/to/folder/dummy.xml';
        const radio = screen.getByTestId('test-id-load-xml');
        act(() => {
            fireEvent.click(radio);
        });
        const inputFileButton = screen.getByTestId('test-id-input-button-xmlFile');
        expect(inputFileButton).toBeEnabled();
        const inputFile = screen.getByTestId('test-id-input-xmlFile').querySelector('input');
        const inputFileHidden = screen.getByTestId('test-id-input-hidden-xmlFile');
        const inputContainer = screen.getByTestId('test-id-input-xmlFile');
        File.prototype.text = function () {
            return Promise.resolve(xmlContent);
        };
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "realPath": "os/workspace/path/to/folder/dummy.xml",
            "empty": false
        }));
        act(() => {
            fireEvent.change(inputFileHidden, { target: { files: [mockFile] } });
        });
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
            const simulationContainer = screen.getByTestId('test-id-sim-container');
            expect(simulationContainer).toBeInTheDocument();
            expect(simulationContainer).not.toBeEmptyDOMElement();
        });
    });
});

describe('Export and Run Xml Test', () => {
    beforeEach(async () => {
        fetchMock.enableMocks();
    });

    afterEach(() => {
        fetchMock.resetMocks();
        cleanup();
    });

    const checkCorrectModal = async (modalName: string, initialSims: Simulation[]) => {
        await act(() => renderWithProviders(<SimulationManager initialSims={initialSims} />));
        const exportXmlButton = screen.getByTestId('test-id-export-button');
        expect(exportXmlButton).toBeEnabled();
        if (modalName !== 'test-id-modal-Incomplete Data') {
            fetchMock.mockResponseOnce(JSON.stringify({ response: 'OK' }));
            await act(() => {
                fireEvent.click(exportXmlButton);
            });
            const modal = screen.getByTestId(modalName);
            expect(modal).toBeInTheDocument();
            return;
        }
        act(() => {
            fireEvent.click(exportXmlButton);
        });
        const modal = screen.getByTestId(modalName);
        expect(modal).toBeInTheDocument();
    }

    test('Should open modal for export XML button missing data', async () => {
        await checkCorrectModal('test-id-modal-Incomplete Data', [
            {
                id: 1,
                logFileSimulation: '',
                numberOfExecutions: 1,
                configurations: '',
                results: '',
                selected: true
            }
        ]);
    });

    test('Should open modal for export XML button complete data', async () => {
        await checkCorrectModal('test-id-modal-Export XML', [
            {
                id: 1,
                logFileSimulation: 'os/workspace/path/to/folder/test1',
                numberOfExecutions: 1,
                configurations: 'os/workspace/path/to/folder/test1',
                results: 'os/workspace/path/to/folder/test1',
                selected: true
            }
        ]);
    });

    test(
        'GIVEN a defined simulation component ' +
        'WHEN the export button is not triggered ' +
        'THEN the Run Selected Cases is not available', () => {
            renderWithProviders(<SimulationManager />)
            const addSimButton = screen.getByTestId('test-id-add-button');
            act(() => {
                fireEvent.click(addSimButton);
            });
            const simulationBox = screen.getByTestId('test-sim-1');
            expect(simulationBox).toBeInTheDocument();
            const checkbox = within(screen.getByTestId('test-id-checkbox-1')).getByRole('checkbox')
            expect(checkbox).toBeInTheDocument();
            act(() => {
                fireEvent.click(checkbox);
            });
            const runSimulationButton = screen.getByTestId('test-id-run-button')
            expect(runSimulationButton).not.toBeEnabled();
        });

    test(
        'GIVEN a defined simulation component ' +
        'WHEN the export button is triggered correctly' +
        'THEN the Run Selected Cases is available', async () => {
            const simulation: Simulation = { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
            renderWithProviders(
                <SimulationManager initialSims={[simulation]} />
            );
            const runButton = screen.getByTestId('test-id-run-button')
            const exportXmlButton = screen.getByTestId('test-id-export-button');

            expect(runButton).not.toBeEnabled();
            fetchMock.mockResponseOnce(JSON.stringify({ response: 'OK' }));
            await act(() => {
                fireEvent.click(exportXmlButton);
            });
            expect(runButton).toBeEnabled();
        });

    test(
        'GIVEN a valid exported simulation file ' +
        'WHEN the run button is clicked ' +
        'THEN a GET call is executed correctly', async () => {

            const simulation: Simulation = { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
            renderWithProviders(
                <SimulationManager initialSims={[simulation]} />
            );
            const runButton = screen.getByTestId('test-id-run-button')
            const exportXmlButton = screen.getByTestId('test-id-export-button');

            expect(runButton).not.toBeEnabled();
            fetchMock.mockResponseOnce(JSON.stringify({ response: 'OK' }));
            await act(() => {
                fireEvent.click(exportXmlButton);
            });
            expect(runButton).toBeEnabled();
            fetchMock.mockResponseOnce(JSON.stringify({ response: 'OK' }));
            act(() => {
                fireEvent.click(runButton);
            })
            const modal = screen.getByTestId('test-id-modal-Run Selected cases');
            expect(modal).toBeInTheDocument();
            const confirm = screen.getByText('Run and navigate')
            act(() => {
                fireEvent.click(confirm)
            })
            expect(fetchMock).toHaveBeenCalledTimes(1)
            expect(fetchMock).toHaveReturned();
        });

    test(
        'GIVEN a valid exported simulation file ' +
        'WHEN the run button is clicked and has executed correctly ' +
        'THEN the simulation modal is displayed', async () => {

            const simulation: Simulation = { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
            renderWithProviders(
                <SimulationManager initialSims={[simulation]} />
            );
            const runButton = screen.getByTestId('test-id-run-button')
            const exportXmlButton = screen.getByTestId('test-id-export-button');

            expect(runButton).not.toBeEnabled();
            fetchMock.mockResponseOnce(JSON.stringify({ response: 'OK' }));
            await act(() => {
                fireEvent.click(exportXmlButton);
            });
            expect(runButton).toBeEnabled();
            fetchMock.mockResponseOnce(JSON.stringify({ data: 'OK' }));
            act(() => {
                fireEvent.click(runButton);
            })
            const modal = screen.getByTestId('test-id-modal-Run Selected cases');
            expect(modal).toBeInTheDocument();
        });

    test(
        'GIVEN a valid exported simulation file ' +
        'WHEN the run button is clicked ' +
        'THEN a GET call is rejected', async () => {
            const simulation: Simulation = { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
            renderWithProviders(
                <SimulationManager initialSims={[simulation]} />
            );
            const runButton = screen.getByTestId('test-id-run-button')
            const exportXmlButton = screen.getByTestId('test-id-export-button');

            expect(runButton).not.toBeEnabled();
            fetchMock.mockResponseOnce(JSON.stringify({ response: 'OK' }));
            await act(() => {
                fireEvent.click(exportXmlButton);
            });
            expect(runButton).toBeEnabled();

            fetchMock.mockReject(Error('Bad Request'))
            act(() => {
                fireEvent.click(runButton);
            });
            const modal = screen.getByTestId('test-id-modal-Run Selected cases');
            expect(modal).toBeInTheDocument();
            const confirm = screen.getByText('Run and navigate')

            act(() => {
                fireEvent.click(confirm)
            });

            expect(fetchMock).toHaveBeenCalledTimes(1)
            expect(fetchMock).rejects.toThrowError(Error('Bad Request'))
        });
    test('Given user click delete simulation Should open modal for delete simulation', () => {
        const simulation: Simulation = { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
        renderWithProviders(
            <SimulationManager initialSims={[simulation]} />
        );
        const deleteSimulationButton = screen.getByTestId('test-id-simbutton-delete-1');
        expect(deleteSimulationButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(deleteSimulationButton);
        });
        const modal = screen.getByTestId('test-id-modal-Remove Simulation');
        expect(modal).toBeInTheDocument();

    });
    test('Given user click Remove Simulation button inside modal should delete selected simulation', async () => {
        const simulation: Simulation = { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
        renderWithProviders(
            <SimulationManager initialSims={[simulation]} />
        );
        const deleteSimulationButton = screen.getByTestId('test-id-simbutton-delete-1');
        expect(deleteSimulationButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(deleteSimulationButton);
        });
        const modal = screen.getByTestId('test-id-modal-Remove Simulation');
        expect(modal).toBeInTheDocument();
        const deleteSimulationModalButton = screen.getByTestId('test-id-modalbutton-delete');
        expect(deleteSimulationModalButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(deleteSimulationModalButton);
        });
        expect(modal).not.toBeInTheDocument();
        const simulationContainer = screen.getByTestId('test-id-sim-container');
        expect(simulationContainer).toHaveTextContent('');
    });
    test('Given user click delete simulation Should open modal for delete simulation', () => {
        const simulation: Simulation = { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
        renderWithProviders(
            <SimulationManager initialSims={[simulation]} />
        );
        const clearAllSimulationsButton = screen.getByTestId('test-id-clearall-simulations-button');
        expect(clearAllSimulationsButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(clearAllSimulationsButton);
        });
        const modal = screen.getByTestId('test-id-modal-Clear Simulations');
        expect(modal).toBeInTheDocument();
    });
    test('Given user click clear simulations button inside modal should delete all simulations', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true },
            { id: 2, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        const clearAllSimulationsButton = screen.getByTestId('test-id-clearall-simulations-button');
        expect(clearAllSimulationsButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(clearAllSimulationsButton);
        });
        const modal = screen.getByTestId('test-id-modal-Clear Simulations');
        expect(modal).toBeInTheDocument();
        const clearAllSimulationsModalButton = screen.getByTestId('test-id-modalbutton-clearall');
        expect(clearAllSimulationsModalButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(clearAllSimulationsModalButton);
        });
        expect(modal).not.toBeInTheDocument();
        const simulationContainer = screen.getByTestId('test-id-sim-container');
        expect(simulationContainer).toHaveTextContent('');
        expect(simulationContainer).toBeEmptyDOMElement();
    });
    test('Given user click on checkbox to unselect all simulations dropdown menu should show  and all simulations should be unselected', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true },
            { id: 2, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: true }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        const dropdownSelector = screen.getByTestId('test-id-box-dropdown-selector');
        expect(dropdownSelector).toBeInTheDocument();
        act(() => {
            fireEvent.click(dropdownSelector);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const unselectAllItem = screen.getByTestId('test-id-menu-item-unselect-all');
        expect(unselectAllItem).toBeInTheDocument();
        act(() => {
            fireEvent.click(unselectAllItem);
        });
        await waitFor(() => {
            expect(unselectAllItem).not.toBeInTheDocument();
            const checkboxSim1 = within(screen.getByTestId('test-id-checkbox-1')).getByRole('checkbox')
            expect(checkboxSim1).toBeInTheDocument();
            expect(checkboxSim1).not.toBeChecked();
        });
    });
    test('Given user click on checkbox to select all simulations dropdown menu should show  and all simulations should be selected', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: false },
            { id: 2, logFileSimulation: 'logfile.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: false }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        const dropdownSelector = screen.getByTestId('test-id-box-dropdown-selector');
        expect(dropdownSelector).toBeInTheDocument();
        act(() => {
            fireEvent.click(dropdownSelector);
        });
        const menuDropdown = screen.getByTestId('test-id-menu-dropdown');
        expect(menuDropdown).toBeInTheDocument();
        expect(menuDropdown).not.toBeEmptyDOMElement();
        const selectAllItem = screen.getByTestId('test-id-menu-item-select-all');
        expect(selectAllItem).toBeInTheDocument();
        act(() => {
            fireEvent.click(selectAllItem);
        });
        await waitFor(() => {
            expect(selectAllItem).not.toBeInTheDocument();
            const checkboxSim1 = within(screen.getByTestId('test-id-checkbox-1')).getByRole('checkbox')
            expect(checkboxSim1).toBeInTheDocument();
            expect(checkboxSim1).toBeChecked();
        });
    });
    test('Given a simulation if user click on duplicate button clicked simulation should be duplicated', () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: 'logFileSimulation.log', numberOfExecutions: 1, configurations: 'configs', results: 'results', selected: false }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        const duplicateSimulationButton = screen.getByTestId('test-id-simbutton-duplicate-1');
        expect(duplicateSimulationButton).toBeInTheDocument();
        act(() => {
            fireEvent.click(duplicateSimulationButton);
        });
        const simulationContainer = screen.getByTestId('test-id-sim-container');
        expect(simulationContainer).not.toBeEmptyDOMElement();
        const simulation2 = screen.getByTestId('test-sim-2');
        expect(simulation2).toBeInTheDocument();
    });

    test('Should says path not found when selecting pathToConfigs in wrong folder ', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: '', numberOfExecutions: 1, configurations: '', results: '', selected: false }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        global.showDirectoryPicker = jest.fn(() => {
            return { name: 'folderName' }
        });
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": false,
            "empty": true,
            "realPath": "os/workspace/path/to/folder/test1"
        }));
        const pathToConfigs = screen.getByTestId('test-id-input-button-configurations');
        act(() => {
            fireEvent.click(pathToConfigs);
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-Path Not Found');
            expect(modalNotFound).toBeInTheDocument();
        });
    });

    test('Should says change ', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: '', numberOfExecutions: 1, configurations: '', results: '', selected: false }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        global.showDirectoryPicker = jest.fn(() => {
            return { name: 'folderName' }
        });
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": false,
            "empty": true,
            "realPath": "os/workspace/path/to/folder/folderName"
        }));
        const pathToConfigs = screen.getByTestId('test-id-input-button-configurations');
        act(() => {
            fireEvent.click(pathToConfigs);
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-Path Not Found');
            expect(modalNotFound).toBeInTheDocument();
        });
    });

    test('Should show delete information modal', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: '', numberOfExecutions: 1, configurations: '', results: '', selected: false }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        global.showDirectoryPicker = jest.fn(() => {
            return { name: 'folderName' }
        });
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "empty": false,
            "realPath": "os/workspace/path/to/folder/test1"
        }));
        const pathToConfigs = screen.getByTestId('test-id-input-button-results');
        act(() => {
            fireEvent.click(pathToConfigs);
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-Data Found On Folder');
            expect(modalNotFound).toBeInTheDocument();
        });
    });

    test('Should show delete information modal an click in the keep it option should not called an extra fetch', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: '', numberOfExecutions: 1, configurations: '', results: '', selected: false }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        global.showDirectoryPicker = jest.fn(() => {
            return { name: 'folderName' }
        });
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "empty": false,
            "realPath": "os/workspace/path/to/folder/test1"
        }));
        const pathToConfigs = screen.getByTestId('test-id-input-button-results');
        act(() => {
            fireEvent.click(pathToConfigs);
        });
        await waitFor(() => {
            const modalNotFound = screen.getByTestId('test-id-modal-Data Found On Folder');
            expect(modalNotFound).toBeInTheDocument();
            const keepItButton = screen.getByTestId('test-id-modalbutton-keepit');
            expect(keepItButton).toBeInTheDocument();
            act(() => {
                fireEvent.click(keepItButton);
            });
            const fileName = 'os/workspace/path/to/folder/test1';
            const inputFile = screen.getByTestId('test-id-input-results').querySelector('input');
            const inputContainer = screen.getByTestId('test-id-input-results');
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
            expect(modalNotFound).not.toBeInTheDocument();
        });
    });

    test('Should show delete information modal an click in the delete information option should not called an extra fetch', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: '', numberOfExecutions: 1, configurations: '', results: '', selected: false }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        global.showDirectoryPicker = jest.fn(() => {
            return { name: 'folderName' }
        });
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "empty": false,
            "realPath": "os/workspace/path/to/folder/test1"
        }));
        const pathToConfigs = screen.getByTestId('test-id-input-button-results');
        act(() => {
            fireEvent.click(pathToConfigs);
        });
        await waitFor(async () => {
            const modalNotFound = screen.getByTestId('test-id-modal-Data Found On Folder');
            expect(modalNotFound).toBeInTheDocument();
            const deleteInformationButton = screen.getByTestId('test-id-modalbutton-delete-information');
            expect(deleteInformationButton).toBeInTheDocument();
            fetchMock.mockResponseOnce(JSON.stringify({
                "response": "ok"
            }));
            await act(() => {
                fireEvent.click(deleteInformationButton);
            });
            const fileName = 'os/workspace/path/to/folder/test1';
            const inputFile = screen.getByTestId('test-id-input-results').querySelector('input');
            const inputContainer = screen.getByTestId('test-id-input-results');
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
            expect(modalNotFound).not.toBeInTheDocument();
            expect(fetchMock).toHaveBeenCalledTimes(2);
        });
    });

    test('Should change pathToConfigs to value returned in mocked fetch', async () => {
        const simulations: Simulation[] = [
            { id: 1, logFileSimulation: '', numberOfExecutions: 1, configurations: '', results: '', selected: false }
        ]
        renderWithProviders(
            <SimulationManager initialSims={simulations} />
        );
        global.showDirectoryPicker = jest.fn(() => {
            return { name: 'test1' }
        });
        fetchMock.mockResponseOnce(JSON.stringify({
            "ok": true,
            "empty": false,
            "realPath": "os/workspace/path/to/folder/test1"
        }));
        const pathToConfigs = screen.getByTestId('test-id-input-button-configurations');
        act(() => {
            fireEvent.click(pathToConfigs);
        });
        const fileName = 'os/workspace/path/to/folder/test1';
        const inputFile = screen.getByTestId('test-id-input-configurations').querySelector('input');
        const inputContainer = screen.getByTestId('test-id-input-configurations');
        await waitFor(() => {
            expect(inputContainer).not.toHaveClass('Mui-error');
            expect(inputFile).toHaveAttribute('value', fileName);
            const simulationContainer = screen.getByTestId('test-id-sim-container');
            expect(simulationContainer).toBeInTheDocument();
            expect(simulationContainer).not.toBeEmptyDOMElement();
        });
    });
});
