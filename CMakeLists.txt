cmake_minimum_required(VERSION 3.21)
find_program(CCACHE_PROGRAM ccache)

set(CMAKE_C_COMPILER "clang-15")
set(CMAKE_CXX_COMPILER "clang++-15")

set (CMAKE_C_STANDARD 23)
set (CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project("Ecstasy")

if(CCACHE_PROGRAM)
    MESSAGE("CCACHE support enabled")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

option(ECSTASY_BUILD_TESTS "Generate build targets for the ecstasy unit tests." ON)
option(ECSTASY_BUILD_COVERAGE "Generate coverage report. Require ECSTASY_BUILD_TESTS." OFF)
option(ECSTASY_BUILD_SHARED_LIB "Generate the Ecstasy shared library." OFF)
option(ECSTASY_BUILD_STATIC_LIB "Generate the Ecstasy static library." ON)
option(ECSTASY_BUILD_TOOLS "Generate tools." ON)
option(ECSTASY_BUILD_FUZZING "Generate fuzzing test tools." OFF)
option(ECSTASY_BUILD_EXAMPLE "Generate examples." ON)

set(BUILD_SHARED_LIBS ECSTASY_BUILD_SHARED_LIB)
set(BUILD_STATIC_LIBS ECSTASY_BUILD_STATIC_LIB)
set(BUILD_TESTING OFF)
