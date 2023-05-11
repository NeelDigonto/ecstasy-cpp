set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw"
    GIT_TAG 3.3.8
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glfw3webgpu
  GIT_REPOSITORY "https://github.com/eliemichel/glfw3webgpu.git"
  GIT_TAG        main
)
FetchContent_MakeAvailable(glfw3webgpu)

FetchContent_Declare(
    stb
    GIT_REPOSITORY "https://github.com/nothings/stb"
    GIT_TAG 5736b15f7ea0ffb08dd38af21067c314d6a3aae9
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(stb)

set(WEBGPU_BACKEND "wgpu")
FetchContent_Declare(
  webgpu
  GIT_REPOSITORY "https://github.com/eliemichel/WebGPU-distribution"
  GIT_TAG        main
)
FetchContent_MakeAvailable(webgpu)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
    GIT_TAG v1.89.5
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    implot
    GIT_REPOSITORY "https://github.com/epezent/implot"
    GIT_TAG v0.14
    GIT_PROGRESS TRUE
)
FetchContent_Populate(implot)

add_subdirectory(src)
#add_subdirectory(third_party)

add_library(${ECSTASY_LIB} STATIC ${ECSTASY_SOURCES})

target_include_directories( ${ECSTASY_LIB} PRIVATE ${stb_SOURCE_DIR})
#target_include_directories( ${ECSTASY_LIB} PRIVATE ${EnTT_SOURCE_DIR}/src)
target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${webgpu_SOURCE_DIR}/include
        ${webgpu_SOURCE_DIR}/include/webgpu
        )
target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

target_link_libraries(${ECSTASY_LIB} PRIVATE fmt::fmt glfw Eigen3::Eigen webgpu glfw3webgpu EnTT::EnTT)
