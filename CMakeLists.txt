cmake_minimum_required(VERSION 3.21)
# https://github.com/nothings/stb
# https://gitlab.com/libeigen/eigen
# https://github.com/glfw/glfw
# https://github.com/ocornut/imgui
# https://github.com/fmtlib/fmt
# https://github.com/hexops/mach-gpu-dawn
# https://github.com/zlib-ng/zlib-ng
# https://github.com/deepmind/mujoco

# https://github.com/odygrd/quill
# https://github.com/skypjack/entt
# https://github.com/google/googletest
# https://github.com/cameron314/readerwriterqueue
# https://github.com/cameron314/concurrentqueue
# https://github.com/wolfpld/tracy
# https://github.com/uNetworking/uWebSockets
# https://boringssl.googlesource.com/boringssl
# https://github.com/bulletphysics/bullet3
# https://github.com/NVIDIA-Omniverse/PhysX  --> https://developer.nvidia.com/physx-sdk

project(Ecstasy
    DESCRIPTION "A Modern Webgpu Renderer"
    LANGUAGES C CXX
    )

include(FetchContent)
include(cmake/download_file.cmake)
include(cmake/dll_utils.cmake)

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)
endif()

set(ECSTASY_LIB "ecstasy")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
    MESSAGE("CCACHE support enabled")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

#set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_FLAGS "-std=c++23")

set(FETCHCONTENT_QUIET FALSE)
get_filename_component(fc_cache "../.fetchcontent_cache"
                       REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
set(FETCHCONTENT_BASE_DIR ${fc_cache})

include_directories(ecstasy/include)

FetchContent_Declare(
    fmtlib
    GIT_REPOSITORY "https://github.com/fmtlib/fmt"
    GIT_TAG 9.1.0
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(fmtlib)
include_directories(${fmtlib_SOURCE_DIR}/include) # public to all targets
#target_include_directories(${ECSTASY_LIB} PRIVATE ${fmtlib_SOURCE_DIR}/include) -- dont look

FetchContent_Declare(
    eigen
    GIT_REPOSITORY "https://gitlab.com/libeigen/eigen"
    GIT_TAG 3.4.0
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(eigen)

add_subdirectory(ecstasy)
add_subdirectory(sample)

