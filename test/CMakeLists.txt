cmake_minimum_required(VERSION 3.14)

project(CircuitGenGraphTests LANGUAGES CXX)

set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -std=c++17 -fPIC")
SET(GCC_COVERAGE_LINK_FLAGS "-lgcov")
set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)
include(FetchContent)
include(CTest)

FetchContent_Declare(
  gtest

  # Url for googletest download.
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(gtest)

add_compile_options(-fno-rtti)
add_compile_options(-g3 -Wall -Wignored-qualifiers -DYYERROR_VERBOSE)
# option for looking for possible memory leaks
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

enable_testing()

if(ENABLE_LOGGING)
  configure_file(
    ../easylogging++.conf
    easylogging++.conf
    COPYONLY
  )
endif()

# ---- Tests ----

set(TEST_NAME CircuitGenGraph_tests)

# Collect all test source files into a single executable
add_executable(${TEST_NAME}
  src/GraphVertexGatesTests.cpp
  src/GraphVertexOutputTests.cpp
  src/GraphVertexSubGraphTests.cpp

  src/GraphVertexBaseTests.cpp
  src/GraphVertexConstantTests.cpp
  src/GraphVertexInputTests.cpp
  src/OrientedGraphTests.cpp
  src/MemoryManagementTests.cpp
  src/DefaultAuxiliaryMethodsTests.cpp
  src/GraphVertexSequentialTests.cpp
  src/GraphUtilsTests.cpp
)

# Include directories
target_include_directories(${TEST_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}
)

# Link test target
target_link_libraries(${TEST_NAME} PUBLIC
  GTest::gtest_main
  CircuitGenGraph::CircuitGenGraph
)

if(ENABLE_LOGGING)
  target_link_libraries( PRIVATE easylogging++)
endif()

if(ENABLE_COVERAGE)
  target_compile_options(${TEST_NAME} PRIVATE -fprofile-arcs -ftest-coverage)
  target_link_options(${TEST_NAME} PRIVATE -fprofile-arcs -ftest-coverage)
endif()

# Register one CTest test
include(GoogleTest)
gtest_discover_tests(${TEST_NAME} PROPERTIES DISCOVERY_TIMEOUT 60)

