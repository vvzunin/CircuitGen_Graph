
# Минимальная версия CMake для сборки
cmake_minimum_required(VERSION 3.26)

project(
  CircuitGenGraph
  VERSION 1.6.0
  DESCRIPTION "Graph for circuits dataset generation"
  HOMEPAGE_URL "https://hub.mos.ru/circuitgen/CircuitGen_Graph"
  LANGUAGES CXX
)

list(APPEND PROJECT_SUPPORTED_SYSTEMS "Linux")

include(cmake/project-is-top-level.cmake)
include(cmake/utils.cmake)
include(cmake/checks.cmake)
include(cmake/folders.cmake)
include(cmake/variables.cmake)
include(cmake/lint-targets.cmake)
include(cmake/spell-targets.cmake)
include(cmake/CMakeGraphVizOptions.cmake)
include(GNUInstallDirs)
include(CheckIPOSupported)

check_ipo_supported(RESULT ipo_supported)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${ipo_supported})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -std=c++17 -fPIC")
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_LOGGING "Enable logging" OFF)

option(ENABLE_COVERAGE "Enable code coverage analysis" OFF)

# Подключаем папку с исходниками (новый формат)
add_subdirectory(src)
if(ENABLE_COVERAGE)
  message(STATUS "Code coverage enabled")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -fprofile-arcs -ftest-coverage -O0)
    target_link_options(${PROJECT_NAME} PRIVATE -fprofile-arcs -ftest-coverage)
  else()
    message(WARNING "Code coverage is only supported with GCC or Clang")
  endif()
endif()

if(ENABLE_LOGGING)
  target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    easylogging++
  )
endif()
include_directories(lib)
link_directories(lib)

add_subdirectory(lib)
target_link_libraries(${PROJECT_NAME} PRIVATE Lorina)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt)

find_program(CLANG_TIDY NAMES clang-tidy)
if(PROJECT_IS_TOP_LEVEL)
  option(CircuitGenGraph_ENABLE_CLANG_TIDY "Run clang-tidy during build" OFF)
endif()

if(CLANG_TIDY AND CircuitGenGraph_ENABLE_CLANG_TIDY)
  set(GRAPH_CLANG_TIDY
    ${CLANG_TIDY};
    -line-filter=[{"name":".hpp"},{"name":".cpp"}]
  )
  set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_CLANG_TIDY "${GRAPH_CLANG_TIDY}"
  )
elseif(CLANG_TIDY)
  message(STATUS "clang-tidy found but disabled (set CircuitGenGraph_ENABLE_CLANG_TIDY=ON to enable)")
else()
  message(STATUS "clang-tidy not found")
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

  # where external projects will look for the library's public headers
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

set_target_properties(
  "${PROJECT_NAME}" PROPERTIES
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}"
  EXPORT_NAME "${PROJECT_NAME}"
  OUTPUT_NAME "${PROJECT_NAME}"
  PUBLIC_HEADER "${PUBLIC_HEADERS}")

include(cmake/utils.cmake)

if(ENABLE_LOGGING)
  message(STATUS "Logs enabled")
  install(TARGETS ${PROJECT_NAME} easylogging++
    EXPORT "${PROJECT_NAME}Targets"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
else()
  message(STATUS "Logs disabled")
  install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Targets"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

install(EXPORT "${PROJECT_NAME}Targets"
  FILE "${PROJECT_NAME}Targets.cmake"
  DESTINATION cmake
  NAMESPACE CircuitGenGraph::
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION cmake
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION cmake
)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

if(PROJECT_IS_TOP_LEVEL)
  option(CircuitGenGraph_BUILD_EXAMPLES "Build examples tree." "${CircuitGenGraph_DEVELOPER_MODE}")
  if(CircuitGenGraph_BUILD_EXAMPLES)
    add_subdirectory(examples)
  endif()
endif()

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_MCSS_DOCS "Build documentation using Doxygen and m.css" OFF)
  if(BUILD_MCSS_DOCS)
    include(cmake/docs.cmake)
  endif()
  if(ENABLE_COVERAGE)
    include(cmake/coverage.cmake)
  endif()
endif()

print_cmake_info(Green)
print_build_info(Green)
print_system_info(Green)

if(PROJECT_IS_TOP_LEVEL)
  print_project_info(Green)
endif()

if(ENABLE_LOGGING)
  configure_file(
    easylogging++.conf
    "${PROJECT_BINARY_DIR}/easylogging++.conf"
    COPYONLY
  )
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

target_compile_options(${PROJECT_NAME} PUBLIC -Wpedantic)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)
add_folders(Project)