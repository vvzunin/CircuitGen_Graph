add_library(Lorina INTERFACE)

target_include_directories(Lorina SYSTEM INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lorina/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(TARGET fmt::fmt)
  set(CIRCUITGENGRAPH_FMT_TARGET fmt::fmt)
elseif(TARGET fmt)
  set(CIRCUITGENGRAPH_FMT_TARGET fmt)
else()
  find_package(fmt CONFIG REQUIRED)
  set(CIRCUITGENGRAPH_FMT_TARGET fmt::fmt)
endif()

target_link_libraries(Lorina INTERFACE
  $<BUILD_INTERFACE:${CIRCUITGENGRAPH_FMT_TARGET}>
  $<INSTALL_INTERFACE:fmt::fmt>
)

set(LORINA_EXAMPLES OFF CACHE BOOL "Disable examples build")

add_subdirectory(lorina)

if(ENABLE_LOGGING)
  add_library(
    easylogging++ STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/easyloggingpp/easylogging++.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/easyloggingpp/easylogging++.h
  )
endif()
