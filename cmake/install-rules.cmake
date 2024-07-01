if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/CircuitGenGraph-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package CircuitGenGraph)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT CircuitGenGraph_Development
)

install(
    TARGETS CircuitGenGraph
    EXPORT CircuitGenGraphTargets
    RUNTIME #
    COMPONENT CircuitGenGraph_Runtime
    LIBRARY #
    COMPONENT CircuitGenGraph_Runtime
    NAMELINK_COMPONENT CircuitGenGraph_Development
    ARCHIVE #
    COMPONENT CircuitGenGraph_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    CircuitGenGraph_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE CircuitGenGraph_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(CircuitGenGraph_INSTALL_CMAKEDIR)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${CircuitGenGraph_INSTALL_CMAKEDIR}"
    COMPONENT CircuitGenGraph_Development
)

install(
    EXPORT CircuitGenGraphTargets
    NAMESPACE CircuitGenGraph::
    DESTINATION "${CircuitGenGraph_INSTALL_CMAKEDIR}"
    COMPONENT CircuitGenGraph_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
