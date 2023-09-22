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

import { fireEvent, screen } from "@testing-library/react";
import Home from "../components/interfaces/Home/Home";
import { renderWithProviders } from "../utils/test.utils";
import { act } from "react-dom/test-utils";
import { PATHS } from "../index.d";

const mockedUsedNavigate = jest.fn();
jest.mock("react-router", () => ({
    ...(jest.requireActual("react-router") as any),
    useNavigate: () => mockedUsedNavigate
}));
describe('Render test for Home Screen component', () => {
    beforeEach(() => {
        renderWithProviders(
            <Home />
        );
    });
    afterEach(() => {
        jest.restoreAllMocks();
    });
    test('Given user enter in Home component should load ', () => {
        const component = screen.getByTestId('test-id-interface-home');
        expect(component).toBeInTheDocument();
        expect(component).not.toBeEmptyDOMElement();
    });
    test('By default should render run the simulations box', () => {
        const simulationBox = screen.getByTestId('test-id-simulationbox-home');
        expect(simulationBox).toBeInTheDocument();
        expect(simulationBox).not.toBeEmptyDOMElement();
    });
    test('By default should render simulation report box', () => {
        const reportBox = screen.getByTestId('test-id-reportbox-home');
        expect(reportBox).toBeInTheDocument();
        expect(reportBox).not.toBeEmptyDOMElement();
    });
    test('By default should render prepare the simulations configurations box', () => {
        const prepareBox = screen.getByTestId('test-id-preparebox-home');
        expect(prepareBox).toBeInTheDocument();
        expect(prepareBox).not.toBeEmptyDOMElement();
    });
    test('Given User click Run button should navigate to /simulation-manager', () => {
        const button = screen.getByTestId('test-id-button-simulationbox-home');
        expect(button).toBeInTheDocument();
        act(() => {
            fireEvent.click(button);
        });
        expect(mockedUsedNavigate).toHaveBeenCalledWith(PATHS.SIMULATION_MANAGER);
    });
    test('Given User click Monitor button should navigate to /simulation-report', () => {
        const button = screen.getByTestId('test-id-button-reportbox-home');
        expect(button).toBeInTheDocument();
        act(() => {
            fireEvent.click(button);
        });
        expect(mockedUsedNavigate).toHaveBeenCalledWith(PATHS.SIMULATION_REPORT);
    });
    test('Given User click Pre button should navigate to /setup-config', () => {
        const button = screen.getByTestId('test-id-button-preparebox-home');
        expect(button).toBeInTheDocument();
        act(() => {
            fireEvent.click(button);
        });
        expect(mockedUsedNavigate).toHaveBeenCalledWith(PATHS.SETUP_CONFIGS);
    });

});