set(SOURCES ecstasy.cpp scene/scene.cpp)
add_subdirectory(audio)
add_subdirectory(renderer)
set("${ECSTASY_LIB}_SOURCES" ${SOURCES} ${AUDIO_SOURCES} ${RENDERER_SOURCES} PARENT_SCOPE)
add_library(${ECSTASY_LIB} STATIC ${SOURCES} ${AUDIO_SOURCES} ${RENDERER_SOURCES})

target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${webgpu_SOURCE_DIR}/include
        ${webgpu_SOURCE_DIR}/include/webgpu
        )

target_link_libraries(${ECSTASY_LIB} PRIVATE fmt::fmt glfw Eigen3::Eigen webgpu glfw3webgpu EnTT::EnTT)
