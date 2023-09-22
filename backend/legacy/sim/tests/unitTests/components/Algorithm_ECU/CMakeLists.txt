################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME Algorithm_ECU_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Algorithm_ECU)
set(COMMONPCM_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/common/pcm)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN

  SOURCES
    tst_ut_ecu.cpp
    ${COMPONENT_SOURCE_DIR}/ecu.cpp

  HEADERS
    tst_ut_ecu.h
    ${COMPONENT_SOURCE_DIR}/ecu.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
    Qt5::Test
)

