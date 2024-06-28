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

# ---- Set sources ----
foreach (src ${SOURCES})
  list(APPEND NEW_SOURCES "../${src}")
endforeach(src ${SOURCES})

message(${NEW_SOURCES})

# ---- Tests ----

# Test GraphVertexConstantTests
add_executable(CircuitGenGraph_test_GraphVertexConstantTests src/GraphVertexConstantTests.cpp ${NEW_SOURCES})

target_link_libraries(CircuitGenGraph_test_GraphVertexConstantTests PRIVATE GTest::gtest_main ${EXTERNAL_LIBS})
target_compile_features(CircuitGenGraph_test_GraphVertexConstantTests PRIVATE cxx_std_17)
target_include_directories(
  CircuitGenGraph_test_GraphVertexConstantTests ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../src>"
)
add_test(NAME GraphVertexConstantTests COMMAND CircuitGenGraph_test_GraphVertexConstantTests)

# Test GraphVertexGatesTests
add_executable(CircuitGenGraph_test_GraphVertexGatesTests src/GraphVertexGatesTests.cpp ${NEW_SOURCES})

target_link_libraries(CircuitGenGraph_test_GraphVertexGatesTests PRIVATE GTest::gtest_main ${EXTERNAL_LIBS})
target_compile_features(CircuitGenGraph_test_GraphVertexGatesTests PRIVATE cxx_std_17)
target_include_directories(
 CircuitGenGraph_test_GraphVertexGatesTests ${warning_guard}
   PUBLIC
   "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../src>"
)
add_test(NAME GraphVertexGatesTests COMMAND CircuitGenGraph_test_GraphVertexGatesTests)

# Test GraphVertexInputTests
add_executable(CircuitGenGraph_test_GraphVertexInputTests src/GraphVertexInputTests.cpp ${NEW_SOURCES})

target_link_libraries(CircuitGenGraph_test_GraphVertexInputTests PRIVATE GTest::gtest_main ${EXTERNAL_LIBS})
target_compile_features(CircuitGenGraph_test_GraphVertexInputTests PRIVATE cxx_std_17)
target_include_directories(
 CircuitGenGraph_test_GraphVertexInputTests ${warning_guard}
   PUBLIC
   "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../src>"
)
add_test(NAME GraphVertexInputTests COMMAND CircuitGenGraph_test_GraphVertexInputTests)

# Test GraphVertexOutputTests
add_executable(CircuitGenGraph_test_GraphVertexOutputTests src/GraphVertexOutputTests.cpp ${NEW_SOURCES})

target_link_libraries(CircuitGenGraph_test_GraphVertexOutputTests PRIVATE GTest::gtest_main ${EXTERNAL_LIBS})
target_compile_features(CircuitGenGraph_test_GraphVertexOutputTests PRIVATE cxx_std_17)
target_include_directories(
  CircuitGenGraph_test_GraphVertexOutputTests ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../src>"
)
add_test(NAME GraphVertexOutputTests COMMAND CircuitGenGraph_test_GraphVertexOutputTests)

# Test GraphVertexSubGraphTests
add_executable(CircuitGenGraph_test_GraphVertexSubGraphTests src/GraphVertexSubGraphTests.cpp ${NEW_SOURCES})

target_link_libraries(CircuitGenGraph_test_GraphVertexSubGraphTests PRIVATE GTest::gtest_main ${EXTERNAL_LIBS})
target_compile_features(CircuitGenGraph_test_GraphVertexSubGraphTests PRIVATE cxx_std_17)
target_include_directories(
  CircuitGenGraph_test_GraphVertexSubGraphTests ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../src>"
)
add_test(NAME GraphVertexSubGraphTests COMMAND CircuitGenGraph_test_GraphVertexSubGraphTests)


# Test OrientedGrahTests
add_executable(CircuitGenGraph_test_OrientedGraphTests src/OrientedGraphTests.cpp ${NEW_SOURCES})

target_link_libraries(CircuitGenGraph_test_OrientedGraphTests PRIVATE GTest::gtest_main ${EXTERNAL_LIBS})
target_compile_features(CircuitGenGraph_test_OrientedGraphTests PRIVATE cxx_std_17)
target_include_directories(
  CircuitGenGraph_test_OrientedGraphTests ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../src>"
)
add_test(NAME OrientedGraphTests COMMAND CircuitGenGraph_test_OrientedGraphTests)
