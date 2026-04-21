cmake_minimum_required(VERSION 3.26)

project(CircuitGenGraphExamples CXX)

include(../cmake/checks.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(CircuitGenGraph REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE CircuitGenGraph::CircuitGenGraph)
  target_compile_features("${NAME}" PRIVATE cxx_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(empty_example)
add_example(example_build_and_gate)
add_example(example_export_graphml_classic)
add_example(example_export_dot)
add_example(example_sequential_ff)
add_example(example_export_graphml_pseudo)

add_folders(Example)
