add_definitions((-w))

add_library(Lorina INTERFACE)
target_include_directories(Lorina SYSTEM INTERFACE lorina/include)

set(LORINA_EXAMPLES OFF CACHE BOOL "Disable examples build")

add_subdirectory(lorina)

remove_definitions(-w)
