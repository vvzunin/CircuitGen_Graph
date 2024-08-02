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

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

enable_testing()

configure_file(
  ../easylogging++.conf
  easylogging++.conf
  COPYONLY
)

# ---- Tests ----

function(add_new_test name)
  add_executable(
    CircuitGenGraph_test_${name}
    src/${name}.cpp
  )

  target_link_libraries(CircuitGenGraph_test_${name} PRIVATE GTest::gtest_main)
  target_link_libraries(CircuitGenGraph_test_${name} PRIVATE CircuitGenGraph::CircuitGenGraph easylogging++)

  target_include_directories(
    CircuitGenGraph_test_${name} ${warning_guard}
    PUBLIC
      "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../src>"
      "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
  )
  add_test(NAME ${name} COMMAND CircuitGenGraph_test_${name})
endfunction()

add_new_test(GraphVertexConstantTests)
add_new_test(GraphVertexGatesTests)
add_new_test(GraphVertexInputTests)
add_new_test(GraphVertexOutputTests)
add_new_test(GraphVertexSubGraphTests)
add_new_test(OrientedGraphTests)