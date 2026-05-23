# Lorina submodule provides both 'lorina' and 'fmt' targets.
set(LORINA_EXAMPLES OFF CACHE BOOL "Disable examples build")
add_subdirectory(lorina)

# Create an alias for compatibility with existing target_link_libraries calls if any
if(TARGET lorina AND NOT TARGET Lorina)
  add_library(Lorina INTERFACE)
  target_link_libraries(Lorina INTERFACE $<BUILD_INTERFACE:lorina>)
endif()

if(ENABLE_LOGGING)
  if(NOT TARGET easylogging++)
  add_library(
    easylogging++ STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/easyloggingpp/easylogging++.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/easyloggingpp/easylogging++.h
  )
  endif()
endif()