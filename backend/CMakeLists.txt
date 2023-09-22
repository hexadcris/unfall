cmake_minimum_required(VERSION 3.5)
project(OPGUICore CXX)

set(CMAKE_AUTOMOC ON)
include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add macro for profiles catalog schema version
add_definitions(-DPROFILES_CATALOG_SCHEMA_VERSION="0.4.8")

if(APPLE)
    set(CMAKE_PREFIX_PATH /opt/homebrew/Cellar/qt@5/5.15.10)
endif()

if(WITH_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()


find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Xml REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(
      ${PROJECT_NAME} 
      PRIVATE 
        Qt5::Concurrent
        Qt5::Core
        Qt5::Widgets
        Qt5::Xml
        Qt5::Network
        Qt5::Sql
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/qhttpengine/src/qhttpengine_export.h.in 
              "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine_export.h"
)

file(GLOB_RECURSE ALL_FILES
        # OPGUI CORE FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/core/common/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/pcmSimulation/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/systemEditor/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/logger/*.cpp
        
        # OPEN API GENERATED FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/OAIgenerated/src/models/*.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/OAIgenerated/src/requests/*.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/OAIgenerated/src/handlers/*.cpp 

        # OPENAPI ROUTER FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/router/*.cpp

        # QHTTP ENGINE FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/qhttpengine/src/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/qhttpengine/src/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/qhttpengine/src/include/qhttpengine/*.h

        # PCM SIMULATION
        ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Models/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/DataStructuresXml/*.cpp
	      ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/DataStructuresXosc/*.cpp

        # PCM INCLUDE DEPENDENCY
        ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/common/pcm/PCM_Data/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/common/pcm/PCM_Importer/*.cpp   
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
          # CORE
          ${CMAKE_CURRENT_SOURCE_DIR}/core/common
          ${CMAKE_CURRENT_SOURCE_DIR}/core/pcmSimulation
          ${CMAKE_CURRENT_SOURCE_DIR}/core/systemEditor
          ${CMAKE_CURRENT_SOURCE_DIR}/core/logger

          # ROUTER
          ${CMAKE_CURRENT_SOURCE_DIR}/router
          
          # OPSIMULATIONMANAGERV2
          ${CMAKE_CURRENT_SOURCE_DIR}/core/OpSimulationManagerV2/
          ${CMAKE_CURRENT_SOURCE_DIR}/core/OpSimulationManagerV2/framework
          ${CMAKE_CURRENT_SOURCE_DIR}/core/OpSimulationManagerV2/importer
          ${CMAKE_CURRENT_SOURCE_DIR}/core/OpSimulationManagerV2/logger
          ${CMAKE_CURRENT_SOURCE_DIR}/core/OpSimulationManagerV2/parser
          
          # QHTTPENGINE
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/qhttpengine/src/include
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/qhttpengine/src/include/qhttpengine
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/qhttpengine/src/src
          
          # OPEN API GENERATED
          ${CMAKE_CURRENT_SOURCE_DIR}/OAIgenerated/src/models
          ${CMAKE_CURRENT_SOURCE_DIR}/OAIgenerated/src/requests
          ${CMAKE_CURRENT_SOURCE_DIR}/OAIgenerated/src/handlers
          
          # CMAKE GENERATED
          ${CMAKE_CURRENT_BINARY_DIR}

          # LEGACY PCM SIMULATION
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Interfaces
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Interfaces/openPASS-PCM
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Models
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Presenters
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Views
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/../window/Interfaces
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/../../application/Interfaces
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/../../common
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/../../common/pcm/DataStructuresBase
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/../../common/pcm/PCM_Data
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/../../common/pcm/PCM_Importer
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/../../../sim/src/common
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/../../../sim/src
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/DataStructuresXml
          ${CMAKE_CURRENT_SOURCE_DIR}/legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/DataStructuresXosc

          # QT INCLUDES
          ${Qt5Xml_INCLUDE_DIRS}
          ${Qt5Sql_INCLUDE_DIRS}
          ${Qt5Network_INCLUDE_DIRS}
          ${Qt5Core_INCLUDE_DIRS}
)


target_sources(
    ${PROJECT_NAME}
    PRIVATE
      ${ALL_FILES}
)

if(NOT WITH_DOC)
  set(WITH_DOC OFF)
endif()


if (WITH_DOC)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/doc)
endif()

               



