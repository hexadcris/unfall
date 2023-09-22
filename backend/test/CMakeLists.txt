cmake_minimum_required(VERSION 3.5)
project(OPGUICoreTests CXX)

set(CMAKE_AUTOMOC ON)
include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH /opt/homebrew/)
    set(CMAKE_PREFIX_PATH /opt/homebrew/Cellar/qt@5/5.15.10)
endif()

enable_testing()

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Xml REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
find_package(GTest REQUIRED)


message(STATUS "GTest include directories: ${GTEST_INCLUDE_DIRS}")
message(STATUS "GTest libraries: ${GTEST_BOTH_LIBRARIES}")
#get_target_property(GTEST_MAIN_PATH GTest::gtest_main LOCATION)
#message("GTest::gtest_main is located at: ${GTEST_MAIN_PATH}")


set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(${PROJECT_NAME} test_main.cpp)

target_link_libraries(
        ${PROJECT_NAME} 
        PRIVATE 
            Qt5::Concurrent
            Qt5::Core
            Qt5::Widgets
            Qt5::Xml
            Qt5::Network
            Qt5::Sql
            GTest::GTest
)

add_test( runUnitTests ${PROJECT_NAME} )

add_custom_target(run_tests ALL
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Always running tests after build"
)

set(TEST_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_helpers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIExportOpsimulationManagerXmlApi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIVerifyPathApi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIDeleteInformationApi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIPathToConvCases.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUISendPCMFile.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIRunOpSimulationManagerApi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUILoadComponentsApi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIExportComponentApi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIQtLogger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIConvertToConfigs.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIExportToSimulation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUIExportSystemsApi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_OPGUILoadSystemsApi.cpp
)

file(GLOB_RECURSE ALL_FILES
        # OPGUI CORE FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../core/common/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../core/pcmSimulation/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../core/logger/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../core/systemEditor/*.cpp
        
        # OPEN API GENERATED FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../OAIgenerated/src/models/*.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/../OAIgenerated/src/requests/*.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/../OAIgenerated/src/handlers/*.cpp 

        # OPENAPI ROUTER FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../router/*.cpp

        # QHTTP ENGINE FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/qhttpengine/src/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/qhttpengine/src/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/qhttpengine/src/include/qhttpengine/*.h

        # PCM SIMULATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Models/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/DataStructuresXml/*.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/DataStructuresXosc/*.cpp

        # PCM INCLUDE DEPENDENCY
        ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/common/pcm/PCM_Data/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/common/pcm/PCM_Importer/*.cpp

)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
            # GTEST
            ${CMAKE_CURRENT_SOURCE_DIR}

            ${CMAKE_CURRENT_SOURCE_DIR}/../build

            # CORE
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/common
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/pcmSimulation
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/logger
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/systemEditor

            # ROUTER
            ${CMAKE_CURRENT_SOURCE_DIR}/../router
            
            # OPSIMULATIONMANAGERV2
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/OpSimulationManagerV2/
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/OpSimulationManagerV2/framework
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/OpSimulationManagerV2/importer
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/OpSimulationManagerV2/logger
            ${CMAKE_CURRENT_SOURCE_DIR}/../core/OpSimulationManagerV2/parser
            
            # QHTTPENGINE
            ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/qhttpengine/src/include
            ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/qhttpengine/src/include/qhttpengine
            ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/qhttpengine/src/src
            
            # OPEN API GENERATED
            ${CMAKE_CURRENT_SOURCE_DIR}/../OAIgenerated/src/models
            ${CMAKE_CURRENT_SOURCE_DIR}/../OAIgenerated/src/requests
            ${CMAKE_CURRENT_SOURCE_DIR}/../OAIgenerated/src/handlers
            
            # CMAKE GENERATED
            ${CMAKE_CURRENT_BINARY_DIR}

            # LEGACY PCM SIMULATION
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Interfaces
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Interfaces/openPASS-PCM
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Models
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Presenters
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Views
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/../window/Interfaces
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/../../application/Interfaces
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/../../common
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/../../common/pcm/DataStructuresBase
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/../../common/pcm/PCM_Data
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/../../common/pcm/PCM_Importer
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/../../../sim/src/common
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/../../../sim/src
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/DataStructuresXml
            ${CMAKE_CURRENT_SOURCE_DIR}/../legacy/gui/plugins/pcmSimulation/Models/ConfigurationGeneratorPcm/DataStructuresXosc

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
            ${TEST_FILES}
)


configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/test_scene.db
        ${CMAKE_CURRENT_BINARY_DIR}/test_scene.db COPYONLY
)

