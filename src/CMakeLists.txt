set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphVertexBase.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphVertexConstant.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphVertexGates.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphVertexInput.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphVertexOutput.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphVertexSubGraph.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/OrientedGraph.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DefaultAuxiliaryMethods.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GraphVertexSequential.cpp
)

set(LIBS
  ${PROJECT_SOURCE_DIR}/lib/fmt/format.cc
)

set(PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/CircuitGenGraph/enums.hpp
  ${PROJECT_SOURCE_DIR}/include/CircuitGenGraph/GraphVertex.hpp
  ${PROJECT_SOURCE_DIR}/include/CircuitGenGraph/GraphVertexBase.hpp
  ${PROJECT_SOURCE_DIR}/include/CircuitGenGraph/OrientedGraph.hpp
  ${PROJECT_SOURCE_DIR}/include/CircuitGenGraph/GraphUtils.hpp
  ${PROJECT_SOURCE_DIR}/include/CircuitGenGraph/DefaultAuxiliaryMethods.hpp
  ${PROJECT_SOURCE_DIR}/include/CircuitGenGraph/GraphMLTemplates.hpp
)

if(ENABLE_LOGGING)
  add_compile_definitions(LOGFLAG)
endif()

# if(BUILD_SHARED_LIBS)
#   add_library(${PROJECT_NAME} SHARED ${SOURCES} ${LIBS})
# else()
#   add_library(${PROJECT_NAME} STATIC ${SOURCES} ${LIBS})
# endif()

# add_library(CircuitGenGraph::CircuitGenGraph ALIAS ${PROJECT_NAME})

include_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/lib)

if(ENABLE_LOGGING)
  add_library(
    easylogging++ STATIC
    ${PROJECT_SOURCE_DIR}/lib/easyloggingpp/easylogging++.cc
    ${PROJECT_SOURCE_DIR}/lib/easyloggingpp/easylogging++.h
  )
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
add_compile_options(-fno-rtti)
add_compile_options(-g3 -Wall -Wignored-qualifiers -DYYERROR_VERBOSE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

target_include_directories(
  ${PROJECT_NAME} ${warning_guard}
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
  ${PROJECT_NAME} SYSTEM
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  EXPORT_NAME ${PROJECT_NAME}
  OUTPUT_NAME ${PROJECT_NAME}
  PUBLIC_HEADER "${PUBLIC_HEADERS}"
)
