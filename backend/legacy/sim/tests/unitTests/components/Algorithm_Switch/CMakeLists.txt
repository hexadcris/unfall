################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME Algorithm_Switch_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Algorithm_Switch)
set(COMMONPCM_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/common/pcm)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    tst_ut_switch.cpp
    ${COMPONENT_SOURCE_DIR}/switch.cpp

  HEADERS
    tst_ut_switch.h
    ${COMPONENT_SOURCE_DIR}/switch.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${COMMONPCM_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
    Qt5::Test
)

