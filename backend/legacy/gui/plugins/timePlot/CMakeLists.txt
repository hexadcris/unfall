################################################################################
# Copyright (c) 2020 ITK Engineering GmbH
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

project(openPASS-TimePlot)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# include QtWidgets package
find_package(Qt5Widgets REQUIRED)

# set include directories
include_directories(.)
#include_directories(Interfaces)
include_directories(Models)
include_directories(Presenters)
include_directories(Views)
include_directories(../window/Interfaces)
include_directories(../../application/Interfaces)
include_directories(../../common)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# include interfaces, models and views
add_subdirectory(Models)
add_subdirectory(Presenters)
add_subdirectory(Views)

add_openpass_target(
    NAME TimePlot TYPE library LINKAGE shared COMPONENT gui

    HEADERS
        PluginTimePlot.h

    SOURCES
        PluginTimePlot.cpp

    LIBRARIES
        Qt5::Widgets
        Window
        ${PROJECT_NAME}_Models
        ${PROJECT_NAME}_Views
        ${PROJECT_NAME}_Presenters
)