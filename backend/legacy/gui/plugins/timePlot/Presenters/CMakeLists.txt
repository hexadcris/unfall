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

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/PresenterTimePlot.h
)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/PresenterTimePlot.cpp
)

add_library(${PROJECT_NAME}_Presenters STATIC ${SOURCES} ${HEADERS})