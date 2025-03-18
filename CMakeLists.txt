cmake_minimum_required(VERSION 3.21)

include(cmake/prelude.cmake)

project(
  CircuitGenGraph
  VERSION 1.3.1
  DESCRIPTION "Graph for circuits dataset generation"
  HOMEPAGE_URL "https://github.com/vvzunin/CircuitGen_Graph"
  LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -std=c++17 -fPIC")
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
  ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DefaultAuxiliaryMethods.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/GraphVertexSequential.cpp
)

set(PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/enums.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/GraphVertex.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/GraphVertexBase.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/OrientedGraph.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/GraphUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/DefaultAuxiliaryMethods.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CircuitGenGraph/GraphMLTemplates.hpp
)

option(ENABLE_LOGGING "Enable logging" OFF)
if (ENABLE_LOGGING)
  add_compile_definitions(LOGFLAG)
endif()

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
  add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()

add_library(
  CircuitGenGraph::CircuitGenGraph ALIAS
  ${PROJECT_NAME})

include_directories(lib)
link_directories(lib)

if(ENABLE_LOGGING)
  add_library(
    easylogging++ STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/easyloggingpp/easylogging++.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/easyloggingpp/easylogging++.h
  )
endif()

add_library(
  fmt STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt/core.h
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt/format-inl.h
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt/format.h
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt/format.cc
)

target_include_directories(fmt PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

option(ENABLE_COVERAGE "Enable code coverage analysis" OFF)

if(ENABLE_COVERAGE)
  message(STATUS "Code coverage enabled")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -fprofile-arcs -ftest-coverage -O0)
    target_link_options(${PROJECT_NAME} PRIVATE -fprofile-arcs -ftest-coverage)
  else()
    message(WARNING "Code coverage is only supported with GCC or Clang")
  endif()
endif()

include(GNUInstallDirs)

if(ENABLE_LOGGING)
  target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    easylogging++
    fmt
  )
else()
target_link_libraries(
  ${PROJECT_NAME} PUBLIC
  fmt
)
endif()

find_program(CLANG_TIDY NAMES clang-tidy)

if (CLANG_TIDY)
  set(GRAPH_CLANG_TIDY
    ${CLANG_TIDY};
    -line-filter=[{"name":".hpp"},{"name":".cpp"}]
  )
  set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_CLANG_TIDY "${GRAPH_CLANG_TIDY}"
  )
else()
  message(FATAL_ERROR "clang-tidy not found, aborting")
endif()

target_include_directories(
  ${PROJECT_NAME} ${warning_guard}
  PUBLIC
  "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
  ${PROJECT_NAME} SYSTEM
  PUBLIC
  "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
)

target_include_directories(${PROJECT_NAME}
  PRIVATE

  # where the library itself will look for its internal headers
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  PUBLIC

  # where top-level project will look for the library's public headers
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt>

  # where external projects will look for the library's public headers
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  EXPORT_NAME ${PROJECT_NAME}
  OUTPUT_NAME ${PROJECT_NAME}
  PUBLIC_HEADER ${PUBLIC_HEADERS})

include(cmake/utils.cmake)

if(ENABLE_LOGGING)
  message(STATUS "Logs enabled")
  install(TARGETS ${PROJECT_NAME} easylogging++ fmt
    EXPORT "${PROJECT_NAME}Targets"

    # these get default values from GNUInstallDirs, no need to set them
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib

    # except for public headers, as we want them to be inside a library folder
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include/SomeLibrary
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
  )
else()
  message(STATUS "Logs disabled")
  install(TARGETS ${PROJECT_NAME} fmt
    EXPORT "${PROJECT_NAME}Targets"

    # these get default values from GNUInstallDirs, no need to set them
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib

    # except for public headers, as we want them to be inside a library folder
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include/SomeLibrary
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
  )
endif()

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h"
)

# generate and install export file
install(EXPORT "${PROJECT_NAME}Targets"
  FILE "${PROJECT_NAME}Targets.cmake"
  DESTINATION cmake
  NAMESPACE CircuitGenGraph::
)

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
