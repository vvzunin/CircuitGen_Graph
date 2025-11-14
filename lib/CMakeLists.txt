add_definitions((-w))

add_library(Lorina INTERFACE)
target_include_directories(Lorina SYSTEM INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lorina/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(Lorina INTERFACE fmt)

set(LORINA_EXAMPLES OFF CACHE BOOL "Disable examples build")

add_subdirectory(lorina)

install(
  TARGETS Lorina fmt
  EXPORT CircuitGenGraphTargets
  INCLUDES DESTINATION "${CircuitGenGraph_INSTALL_CMAKEDIR}"
)

if(ENABLE_LOGGING)
  add_library(
    easylogging++ STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/easyloggingpp/easylogging++.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/easyloggingpp/easylogging++.h
  )
endif()

remove_definitions(-w)
