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

#pragma once

#include <QString>
#include <QList>

namespace OPGUIInit {

    /**
     * Initialises the opGUI application by checking the existence of 
     * necessary files and directories.
     *
     * @return Returns true if the initialization was successful, false otherwise.
     */
    bool Initialise_OPGUI();

    /**
     * Checks if the given string contains placeholders.
     *
     * @param str The string to check for placeholders.
     * @return Returns true if placeholders are detected, false otherwise.
     */
    bool containsPlaceholder(const QString& str);

    /**
     * Checks for the existence of a directory and optionally creates it if it does not exist.
     *
     * @param path The path of the directory to check.
     * @param description A description for logging purposes.
     * @param createIfNotExists Flag indicating if the directory should be created if it doesn't exist.
     * @return Returns true if the directory exists (or was successfully created), false otherwise.
     */
    bool checkAndCreateDir(const QString& path, const QString& description, bool createIfNotExists);

    /**
     * Checks for the existence of a file.
     *
     * @param filePath The path of the file to check.
     * @param description A description for logging purposes.
     * @return Returns true if the file exists, false otherwise.
     */
    bool checkFileExists(const QString& filePath, const QString& description);

} // namespace OPGUIInit
