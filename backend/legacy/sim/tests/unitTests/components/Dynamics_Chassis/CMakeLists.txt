################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME Dynamics_Chassis_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Dynamics_Chassis)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module

  SOURCES
    tst_ut_Chassis3D.cpp
    ${COMPONENT_SOURCE_DIR}/ForceWheelZ.cpp
    ${COMPONENT_SOURCE_DIR}/WheelOscillation.cpp

  HEADERS
    ${COMPONENT_SOURCE_DIR}/VehicleBasics.h
    ${COMPONENT_SOURCE_DIR}/ForceWheelZ.h
    ${COMPONENT_SOURCE_DIR}/WheelOscillation.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}

  LIBRARIES
    Qt5::Core
    Qt5::Test
)
