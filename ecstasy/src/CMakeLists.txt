set(SOURCES ecstasy.cpp)
add_subdirectory(audio)
add_subdirectory(renderer)
add_library(${ECSTASY_LIB} STATIC ${SOURCES} ${AUDIO_SOURCES} ${RENDERER_SOURCES})
target_include_directories(${ECSTASY_LIB} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${SDL2_SOURCE_DIR}/include
    )
#MESSAGE("SDL2_LIBRARIES ${SDL2_LIBRARIES}")
target_link_libraries(${ECSTASY_LIB} PRIVATE fmt::fmt SDL2main SDL2-static glfw Eigen3::Eigen)