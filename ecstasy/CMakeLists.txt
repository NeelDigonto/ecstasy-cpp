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


add_subdirectory(src)
#add_subdirectory(third_party)

