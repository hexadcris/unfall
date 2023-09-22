################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#               2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# build tuple of configs and add to list
# PYOPENPASS_CONFIG_LIST
# ( CONFIG, RESOURCES ) -> target 1
# ( CONFIG, RESOURCES ) -> target 2 (custom)

# TODO: Check PYOPENPASS_CONFIGS
# filenames must ..
# - start with "test"
# - end with ".json"
# - be usable within a cmake target (e.g. no spaces in between)
# - must not unique in list
list(APPEND PYOPENPASS_CONFIGS
  ${CMAKE_CURRENT_LIST_DIR}/test_end_to_end.json
  ${CMAKE_CURRENT_LIST_DIR}/test_spawner.json
  )

list(APPEND PYOPENPASS_RESOURCES
  ${CMAKE_CURRENT_LIST_DIR}/../../contrib/examples
  ${CMAKE_CURRENT_LIST_DIR}/../../contrib/examples
  )

# EXTEND LIST BY EXTERNALLY SET CONFIGS AND RESOURCES (always set both!)
list(APPEND PYOPENPASS_CONFIGS   ${PYOPENPASS_EXTRA_CONFIGS})
list(APPEND PYOPENPASS_RESOURCES ${PYOPENPASS_EXTRA_RESOURCES})

# CHECK IF LISTS HAVE SAME LENGTH
list(LENGTH PYOPENPASS_CONFIGS LEN_CONFIGS)
list(LENGTH PYOPENPASS_RESOURCES LEN_RESOURCES)
if(NOT LEN_CONFIGS EQUAL LEN_RESOURCES)
  message(FATAL_ERROR "PYOPENPASS_CONFIGS and PYOPENPASS_RESOURCES must be the same length")
endif()

# use pytest filter syntax to exclude tests by name
if(WIN32)
  set(PYOPENPASS_TEST_FILTER "-k not linux64")
  set(OP_EXE_NAME opSimulation.exe)
elseif(UNIX)
  set(PYOPENPASS_TEST_FILTER "-k not win64")
  set(OP_EXE_NAME opSimulation)
endif()

# Find python3 interpreter
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Main pyOpenPASS target
add_custom_target(pyOpenPASS
  COMMENT "Main pyOpenPASS target, executing all test files"
)

# Add a target for every test file, respectively -> taking filename
# https://cmake.org/pipermail/cmake/2012-July/051336.html
math(EXPR LEN_CONFIGS "${LEN_CONFIGS}-1")
foreach(i RANGE ${LEN_CONFIGS})
  list(GET PYOPENPASS_CONFIGS   ${i} CURRENT_CONFIG)
  list(GET PYOPENPASS_RESOURCES ${i} CURRENT_RESOURCES)

  if (NOT EXISTS ${CURRENT_CONFIG})
    message(FATAL_ERROR "${CURRENT_CONFIG} does not exist")
  endif()

  get_filename_component(CURRENT_TESTCASE ${CURRENT_CONFIG} NAME_WE)

  add_custom_target(
    pyOpenPASS_${CURRENT_TESTCASE}
    COMMAND ${CMAKE_COMMAND} -E copy ${CURRENT_CONFIG} ${CMAKE_CURRENT_LIST_DIR}/pyOpenPASS/${CURRENT_TESTCASE}.json
    COMMAND ${Python3_EXECUTABLE} -m pytest --simulation=${CMAKE_INSTALL_PREFIX}/${OP_EXE_NAME} --mutual=${CURRENT_RESOURCES}/Common --resources=${CURRENT_RESOURCES}/Configurations --report-path=${CMAKE_INSTALL_PREFIX}/testreport ${CMAKE_CURRENT_LIST_DIR}/pyOpenPASS/${CURRENT_TESTCASE}.json ${PYOPENPASS_TEST_FILTER} -v --junitxml="result_${CURRENT_TESTCASE}.xml"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/pyOpenPASS
    )

  add_dependencies(pyOpenPASS pyOpenPASS_${CURRENT_TESTCASE})
endforeach()
