set(SOURCES ecstasy.cpp)
add_subdirectory(audio)
add_subdirectory(renderer)
add_library(${ECSTASY_LIB} STATIC ${SOURCES} ${AUDIO_SOURCES} ${RENDERER_SOURCES})

# set(WEBGPU_BACKEND "dawn")
# 
# FetchContent_Declare(
#   webgpu
#   GIT_REPOSITORY "https://github.com/eliemichel/WebGPU-distribution"
#   GIT_TAG        "wgpu"
#   GIT_PROGRESS TRUE
# )
# FetchContent_MakeAvailable(webgpu)

target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${webgpu_SOURCE_DIR}/include/
        ${webgpu_SOURCE_DIR}/include/webgpu
        )

target_include_directories(
    ${ECSTASY_LIB} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${SDL2_SOURCE_DIR}/include
    )
#MESSAGE("SDL2_LIBRARIES ${SDL2_LIBRARIES}")
target_link_libraries(${ECSTASY_LIB} PRIVATE fmt::fmt glfw Eigen3::Eigen webgpu glfw3webgpu)

# SDL2main SDL2-static 