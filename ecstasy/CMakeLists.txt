set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw"
    GIT_TAG 3.3.8
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glfw)
#
#FetchContent_Declare(
#  glfw3webgpu
#  GIT_REPOSITORY "https://github.com/eliemichel/glfw3webgpu.git"
#  GIT_TAG        main
#)
#FetchContent_MakeAvailable(glfw3webgpu)
#
#FetchContent_Declare(
#    stb
#    GIT_REPOSITORY "https://github.com/nothings/stb"
#    GIT_TAG 5736b15f7ea0ffb08dd38af21067c314d6a3aae9
#    GIT_PROGRESS TRUE
#)
#FetchContent_MakeAvailable(stb)
#
#set(WEBGPU_BACKEND "wgpu")
#FetchContent_Declare(
#  webgpu
#  GIT_REPOSITORY "https://github.com/eliemichel/WebGPU-distribution"
#  GIT_TAG        main
#)
#FetchContent_MakeAvailable(webgpu)

#FetchContent_Declare(
#    imgui
#    GIT_REPOSITORY "https://github.com/ocornut/imgui"
#    GIT_TAG v1.89.5
#    GIT_PROGRESS TRUE
#)
#FetchContent_MakeAvailable(imgui)
#
#FetchContent_Declare(
#    implot
#    GIT_REPOSITORY "https://github.com/epezent/implot"
#    GIT_TAG v0.14
#    GIT_PROGRESS TRUE
#)
#FetchContent_Populate(implot)

#set(FILAMENT_ENABLE_LTO OFF)
#set(FILAMENT_BUILD_FILAMAT OFF)
#set(FILAMENT_SUPPORTS_OPENGL OFF)
#set(FILAMENT_SUPPORTS_METAL OFF)
#set(FILAMENT_SUPPORTS_VULKAN ON)
#set(FILAMENT_INSTALL_BACKEND_TEST OFF)
#set(FILAMENT_USE_EXTERNAL_GLES3 OFF)
#set(FILAMENT_USE_SWIFTSHADER OFF)
#set(FILAMENT_SKIP_SAMPLES ON)
#FetchContent_Declare(
#    filament
#    GIT_REPOSITORY "https://github.com/google/filament.git"
#    GIT_TAG v1.40.2
#    GIT_PROGRESS TRUE
#)
#FetchContent_Populate(filament)

add_subdirectory(src)
#add_subdirectory(third_party)

add_library(${ECSTASY_LIB} STATIC ${ECSTASY_SOURCES})

#target_include_directories( ${ECSTASY_LIB} PRIVATE ${stb_SOURCE_DIR})
#target_include_directories( ${ECSTASY_LIB} PRIVATE ${EnTT_SOURCE_DIR}/src)
#target_include_directories(
#        ${ECSTASY_LIB} PRIVATE 
#        ${webgpu_SOURCE_DIR}/include
#        ${webgpu_SOURCE_DIR}/include/webgpu
#        )

target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_include_directories(${ECSTASY_LIB} PRIVATE /home/fathomless/lib/filament/out/debug/filament/include)
    target_link_directories(${ECSTASY_LIB} INTERFACE /home/fathomless/lib/filament/out/debug/filament/lib/x86_64)
else()
    target_include_directories(${ECSTASY_LIB} PRIVATE /home/fathomless/lib/filament/out/release/filament/include)
    target_link_directories(${ECSTASY_LIB} INTERFACE /home/fathomless/lib/filament/out/release/filament/lib/x86_64)
endif()

target_link_libraries(${ECSTASY_LIB} PRIVATE 
    fmt::fmt Eigen3::Eigen EnTT::EnTT glfw          
    filament
    backend
    bluegl
    bluevk
    filabridge
    filaflat
    utils
    geometry
    smol-v
    vkshaders
    ibl
    matdbg 
    filamat
    )