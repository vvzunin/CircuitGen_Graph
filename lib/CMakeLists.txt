add_definitions((-w))

add_library(Lorina INTERFACE)
target_include_directories(Lorina SYSTEM INTERFACE lorina/include)

set(LORINA_EXAMPLES OFF CACHE BOOL "Disable examples build")

add_subdirectory(lorina)

if(ENABLE_LOGGING)
  add_library(
    easylogging++ STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/easyloggingpp/easylogging++.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/easyloggingpp/easylogging++.h
  )
endif()

remove_definitions(-w)
