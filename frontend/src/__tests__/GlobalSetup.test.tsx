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

import { screen } from "@testing-library/react";
import GlobalSetup from "../components/interfaces/GlobalSetup/GlobalSetup";
import { renderWithProviders } from "../utils/test.utils";

describe('Render Test for Global Setup Interface', () => {
  beforeEach(() => {
    renderWithProviders(<GlobalSetup />);
  });
  test('Should Render System Editor Section with its input', () => {
    const systemEditorSection = screen.getByTestId('test-id-box-system-editor-section');
    expect(systemEditorSection).toBeInTheDocument();
    expect(systemEditorSection).not.toBeEmptyDOMElement();
    const systemEditorTitle = screen.getByText("SYSTEM EDITOR");
    expect(systemEditorTitle).toBeInTheDocument();
    const pathToComponentsText = screen.getByText("Path to components");
    expect(pathToComponentsText).toBeInTheDocument();
    const pathToComponentsInput = screen.getByTestId('test-id-input-pathToComponents');
    expect(pathToComponentsInput).toBeInTheDocument();
    expect(pathToComponentsInput).not.toBeEmptyDOMElement();
  });
  test("renders PRE section", () => {
    const preSection = screen.getByText("PRE");
    expect(preSection).toBeInTheDocument();
    const pathToEndToEndTests = screen.getByText("Path to End-to-End tests");
    expect(pathToEndToEndTests).toBeInTheDocument();
  });
  test("renders RUN section", () => {
    const runSection = screen.getByText("RUN");
    expect(runSection).toBeInTheDocument();
    const pathToLibraries = screen.getByText("Path to libraries/modules");
    expect(pathToLibraries).toBeInTheDocument();
    const pathToCore = screen.getByText("Path to openPASS core");
    expect(pathToCore).toBeInTheDocument();
    const pathToSimManager = screen.getByText("Path to openPASS simulation manager");
    expect(pathToSimManager).toBeInTheDocument();
  });

  test("renders FooterComponent with Save Changes button", () => {
    const footer = screen.getByTestId("test-id-footer-setup");
    expect(footer).toBeInTheDocument();
    expect(footer).not.toBeEmptyDOMElement();

    const saveChangesButton = screen.getByTestId("test-id-button-saveChanges");
    expect(saveChangesButton).toBeInTheDocument();
    expect(saveChangesButton).toHaveTextContent("Save Changes");
  });


});