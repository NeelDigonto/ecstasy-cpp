set(SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/ecstasy.cpp 
        #${CMAKE_CURRENT_SOURCE_DIR}/scene/scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/controller/InputController.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/controller/EditorController.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geometry/plane.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geometry/box.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geometry/Geometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/material/Material.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/manager/RendererResourceManager.cpp
        )

add_subdirectory(audio)
add_subdirectory(renderer)
set(ECSTASY_SOURCES ${SOURCES} ${AUDIO_SOURCES} ${RENDERER_SOURCES} PARENT_SCOPE)

include_directories( 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        )


