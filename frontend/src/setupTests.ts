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

// jest-dom adds custom jest matchers for asserting on DOM nodes.
// allows you to do things like:
// expect(element).toHaveTextContent(/react/i)
// learn more: https://github.com/testing-library/jest-dom
import '@testing-library/jest-dom';
import '@testing-library/react';
jest.mock('./utils/constants', () => ({
    VITE_ENVIRONMENT: 'dev',
    VITE_API_DEV_URL: 'https://8cea34a7-8df9-463b-ab26-75fe56c1390e.mock.pstmn.io/',
    VITE_API_LOCAL_URL: 'http://localhost:8848',
    VITE_TEST_REACT: 'dev'
}));

declare global {
    function showDirectoryPicker({ }: any): any;
}
