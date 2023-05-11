set(SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/ecstasy.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/scene/scene.cpp)
add_subdirectory(audio)
add_subdirectory(renderer)
set(ECSTASY_SOURCES ${SOURCES} ${AUDIO_SOURCES} ${RENDERER_SOURCES} PARENT_SCOPE)

include_directories( 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${webgpu_SOURCE_DIR}/include
        ${webgpu_SOURCE_DIR}/include/webgpu
        )


