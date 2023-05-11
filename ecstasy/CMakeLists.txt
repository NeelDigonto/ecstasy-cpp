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

add_library(${ECSTASY_LIB} STATIC ${ECSTASY_SOURCES})

target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${webgpu_SOURCE_DIR}/include
        ${webgpu_SOURCE_DIR}/include/webgpu
        )
target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(${ECSTASY_LIB} PRIVATE fmt::fmt glfw Eigen3::Eigen webgpu glfw3webgpu EnTT::EnTT)
