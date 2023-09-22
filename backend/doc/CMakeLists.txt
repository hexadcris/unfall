################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

project(docu)
cmake_minimum_required(VERSION 3.14)

set(ADD_SPHINX_OPTION "")

if(WIN32)
  find_program(EXECUTABLE NAMES sphinx sphinx-build.exe)
elseif(UNIX)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  set(EXECUTABLE ${Python3_EXECUTABLE})
  set(ADD_SPHINX_OPTION -m sphinx)
endif()

if(WITH_DOC)
  if(TARGET doc)
    message(STATUS "Target doc already defined. Skipping.")
  else()
    add_custom_target(doc ALL
        COMMAND ${CMAKE_COMMAND}
            -DSRC=${CMAKE_CURRENT_LIST_DIR}/source
            -DDST=${CMAKE_BINARY_DIR}/doc
            -P ${CMAKE_CURRENT_SOURCE_DIR}/PrepareDoc.cmake
            COMMENT "Copy OS documentation and replace placeholders"
        COMMAND ${EXECUTABLE}            # sphinx-build
            ${ADD_SPHINX_OPTION}
            -M html                             # generate HTML
            ${CMAKE_BINARY_DIR}/doc/source      # source path
            ${CMAKE_BINARY_DIR}/doc             # destination path
            -D api_doc_build=OFF                # turn exhale ON/OFF
            -W --keep-going                     # treat warnings as errors but keep-going
            -n # https://www.sphinx-doc.org/en/master/usage/configuration.html#confval-nitpicky
            COMMENT "Build Sphinx documentation"

        COMMAND ${CMAKE_COMMAND} 
          -E cmake_echo_color --green
          "The HTML pages are in ${CMAKE_BINARY_DIR}/doc/html/index.html ${SPHINX_EXECUTABLE} ")
          

    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
      ${CMAKE_BINARY_DIR}/doc)

#    # make HTML doc available on install
#    install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html/
#            DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
  endif()
endif()
