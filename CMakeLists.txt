cmake_minimum_required(VERSION 3.21)

include(cmake/prelude.cmake)

project(
  CircuitGenGraph
  VERSION 1.1.0
  DESCRIPTION "Graph for circuits dataset generation"
  HOMEPAGE_URL "https://github.com/vvzunin/CircuitGen_Graph"
  LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphVertexBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphVertexConstant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphVertexGates.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphVertexInput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphVertexOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphVertexSubGraph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OrientedGraph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DefaultSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DefaultAuxiliaryMethods.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/easyloggingpp/easylogging++.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/external/easyloggingpp/easylogging++.h
)

set(PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/enums.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/GraphVertex.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/GraphVertexBase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/OrientedGraph.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/DefaultSettings.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/DefaultAuxiliaryMethods.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/GraphMLTemplates.hpp
)

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()

add_library(CircuitGenGraph::CircuitGenGraph ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

include(GNUInstallDirs)

target_include_directories(
    ${PROJECT_NAME} ${warning_guard}
    PRIVATE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external/easyloggingpp>"
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    ${PROJECT_NAME} SYSTEM
    PRIVATE
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/external/easyloggingpp>"
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE

    # where the library itself will look for its internal headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/easyloggingpp
    PUBLIC

    # where top-level project will look for the library's public headers
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    # where external projects will look for the library's public headers
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/easyloggingpp)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES 
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME ${PROJECT_NAME}
    OUTPUT_NAME ${PROJECT_NAME}
    PUBLIC_HEADER ${PUBLIC_HEADERS})

include(cmake/utils.cmake)

install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Targets"

    # these get default values from GNUInstallDirs, no need to set them
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib

    # except for public headers, as we want them to be inside a library folder
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include/SomeLibrary
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
)

# generate and install export file
install(EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Targets.cmake"
    DESTINATION cmake
    NAMESPACE CircuitGenGraph::
)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

include(CMakePackageConfigHelpers)

# generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)

# install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION cmake
)

if(PROJECT_IS_TOP_LEVEL)
    option(BUILD_EXAMPLES "Build examples tree." "${CircuitGenGraph_DEVELOPER_MODE}")

    if(BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

# ---- Developer mode ----
if(NOT CircuitGenGraph_DEVELOPER_MODE)
    return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
    message(
        AUTHOR_WARNING
        "Developer mode is intended for developers of CircuitGenGraph"
    )
endif()

include(cmake/dev-mode.cmake)
