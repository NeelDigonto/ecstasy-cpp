cmake_minimum_required(VERSION 3.22)
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
    DESCRIPTION "My Dream Project"
    LANGUAGES C CXX
    )

include(FetchContent)
include(cmake/download_file.cmake)
# include(cmake/dll_utils.cmake)
# include(cmake/CPM.cmake)
include(CheckLanguage)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-stdlib=libc++ -Werror=return-type -flto") #warning disabled for msvc
endif()



set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_link_options(-fsanitize=address)
    #add_link_options(-fsanitize=leak)
    #add_link_options(-fsanitize=memory)
    #add_link_options(-fsanitize=thread)
    add_link_options(-fsanitize=undefined)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_link_options(-flto)
endif()

set(ECSTASY_LIB "ecstasy")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
    MESSAGE("CCACHE support enabled")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

set(FETCHCONTENT_QUIET FALSE)
get_filename_component(fc_cache "../.fetchcontent_cache"
                       REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
set(FETCHCONTENT_BASE_DIR ${fc_cache})

include_directories(ecstasy/include)

FetchContent_Declare(
    fmtlib
    GIT_REPOSITORY "https://github.com/fmtlib/fmt"
    GIT_TAG 10.2.1
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
include_directories(${eigen_SOURCE_DIR})

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY "https://github.com/skypjack/entt"
    GIT_TAG v3.12.2
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(EnTT)
include_directories(${EnTT_SOURCE_DIR}/src)

set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw"
    GIT_TAG 3.3.8
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glfw)

 FetchContent_Declare(
     stb
     GIT_REPOSITORY "https://github.com/nothings/stb"
     GIT_TAG master
     GIT_PROGRESS TRUE
 )
 FetchContent_MakeAvailable(stb)

FetchContent_Declare(
    implot
    GIT_REPOSITORY "https://github.com/epezent/implot"
    GIT_TAG v0.16
    GIT_PROGRESS TRUE
)
FetchContent_Populate(implot)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
    GIT_TAG "docking" #v1.90
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(imgui)

#FetchContent_Declare(
#    OpenEXR
#    GIT_REPOSITORY "https://github.com/AcademySoftwareFoundation/openexr"
#    GIT_TAG v3.2.1
#    GIT_PROGRESS TRUE
#)
#FetchContent_Populate(OpenEXR)

FetchContent_Declare(
    tinyexr
    GIT_REPOSITORY "https://github.com/syoyo/tinyexr"
    GIT_TAG v1.0.7
    GIT_PROGRESS TRUE
)
FetchContent_Populate(tinyexr)

add_subdirectory(ecstasy)
add_subdirectory(libs)

