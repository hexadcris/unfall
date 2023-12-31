################################################################################
# Copyright (c) 2020-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
set(COMPONENT_TEST_NAME SensorOSI_Tests)
set(COMPONENT_SOURCE_DIR ${OPENPASS_SIMCORE_DIR}/components/Sensor_OSI/src)

add_openpass_target(
  NAME ${COMPONENT_TEST_NAME} TYPE test COMPONENT module
  DEFAULT_MAIN
  LINKOSI

  SOURCES
    sensorOSI_Tests.cpp
    ${COMPONENT_SOURCE_DIR}/objectDetectorBase.cpp
    ${COMPONENT_SOURCE_DIR}/sensorGeometric2D.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/DataTypes.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/MovingObject.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/OpenDriveTypeMapper.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/TrafficLight.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldData.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldDataException.cpp
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldObjectAdapter.cpp

  HEADERS
    sensorOSI_Tests.h
    sensorOSI_TestsCommon.h
    ${COMPONENT_SOURCE_DIR}/objectDetectorBase.h
    ${COMPONENT_SOURCE_DIR}/sensorGeometric2D.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/DataTypes.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/MovingObject.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/OpenDriveTypeMapper.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/TrafficLight.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldData.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldDataException.h
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/WorldObjectAdapter.h

  INCDIRS
    ${COMPONENT_SOURCE_DIR}
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI
    ${OPENPASS_SIMCORE_DIR}/core/opSimulation/modules/World_OSI/OWL/fakes

  LIBRARIES
    Qt5::Core
    Common
)

