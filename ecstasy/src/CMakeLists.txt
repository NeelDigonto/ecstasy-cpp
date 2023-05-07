set(SOURCES ecstasy.cpp)
add_subdirectory(audio)
add_subdirectory(renderer)
add_library(${ECSTASY_LIB} STATIC ${SOURCES} ${AUDIO_SOURCES} ${RENDERER_SOURCES})

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

target_link_libraries(${ECSTASY_LIB} PRIVATE fmt::fmt glfw Eigen3::Eigen webgpu glfw3webgpu)

# sdl2
#FetchContent_Declare(
#    SDL2
#    GIT_REPOSITORY  "https://github.com/libsdl-org/SDL"
#    GIT_TAG         release-2.26.5
#    GIT_PROGRESS    TRUE
#)
#FetchContent_MakeAvailable(SDL2)
#MESSAGE("SDL2_LIBRARIES ${SDL2_LIBRARIES}")
# SDL2main SDL2-static 