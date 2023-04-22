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

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
set(BUILD_TESTING OFF)

#add_executable(test1 src/EntryPoint.cpp)

#target_include_directories(test1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/)
#target_link_libraries(test1 zlib fmt::fmt)



# https://github.com/nothings/stb
# https://gitlab.com/libeigen/eigen
# https://github.com/skypjack/entt
# https://github.com/google/googletest
# https://github.com/cameron314/readerwriterqueue
# https://github.com/cameron314/concurrentqueue
# https://github.com/jonasmr/microprofile
# https://github.com/zlib-ng/zlib-ng
# https://github.com/uNetworking/uWebSockets
# https://github.com/libsdl-org/SDL
# https://github.com/odygrd/quill
# https://github.com/ocornut/imgui
# https://boringssl.googlesource.com/boringssl
# https://github.com/deepmind/mujoco
# https://github.com/bulletphysics/bullet3
# https://github.com/NVIDIA-Omniverse/PhysX  --> https://developer.nvidia.com/physx-sdk