set(
    FORMAT_PATTERNS
    src/*.cpp
    src/*/*.cpp src/*/*.hpp
    include/*/*.hpp
    include/*/*.hpp.in
    test/*/*.cpp test/*/*.hpp
    CACHE STRING
    "; separated patterns relative to the project source dir to format"
)

# Keep exact formatter command without hardcoded suffixes.
# CI pins 18.x via scripts/setup/install-clang-format-ci.sh; override FORMAT_COMMAND if needed.
set(FORMAT_COMMAND clang-format CACHE STRING "Formatter to use")

if(NOT TARGET format-check)
add_custom_target(
    format-check
...
    COMMENT "Linting the code"
    VERBATIM
)
endif()

if(NOT TARGET format-fix)
add_custom_target(
    format-fix
...
    COMMENT "Fixing the code"
    VERBATIM
)
endif()
