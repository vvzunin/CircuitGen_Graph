include(GNUInstallDirs)

if(NOT TARGET fmt::fmt)
  find_package(fmt CONFIG REQUIRED)
endif()

if(NOT TARGET Lorina)
  add_library(Lorina INTERFACE)

  target_include_directories(Lorina SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lorina/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(Lorina INTERFACE
    fmt::fmt
  )
endif()

set(LORINA_EXAMPLES OFF CACHE BOOL "Disable examples build" FORCE)
add_subdirectory(lorina EXCLUDE_FROM_ALL)

if(ENABLE_LOGGING AND NOT TARGET easylogging++)
  add_library(
    easylogging++ STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/easyloggingpp/easylogging++.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/easyloggingpp/easylogging++.h
  )
endif()
