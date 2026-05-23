set(SPELL_COMMAND codespell CACHE STRING "Spell checker to use")

if(NOT TARGET spell-check)
add_custom_target(
    spell-check
...
    COMMENT "Checking spelling"
    VERBATIM
)
endif()

if(NOT TARGET spell-fix)
add_custom_target(
    spell-fix
...
    COMMENT "Fixing spelling errors"
    VERBATIM
)
endif()
