add_subdirectory(src)
#add_subdirectory(third_party)

add_library(${ECSTASY_LIB} STATIC 
    ${ECSTASY_SOURCES}
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)


#target_compile_options(${ECSTASY_LIB} PRIVATE -Wall -Wextra -Werror)

#target_include_directories( ${ECSTASY_LIB} PRIVATE ${stb_SOURCE_DIR})
#target_include_directories( ${ECSTASY_LIB} PRIVATE ${EnTT_SOURCE_DIR}/src)

target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        
        ${stb_SOURCE_DIR}
        ${tinyexr_SOURCE_DIR}
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${implot_SOURCE_DIR}
        )

if(MSVC)
    target_link_libraries(${ECSTASY_LIB} PRIVATE opengl32)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_include_directories(${ECSTASY_LIB} PRIVATE "C:\\Users\\fathomless\\lib\\filament-v1.46.0-windows\\include")
        target_link_directories(${ECSTASY_LIB} INTERFACE "C:\\Users\\fathomless\\lib\\filament-v1.46.0-windows\\lib\\x86_64\\mdd")
    else()
        target_include_directories(${ECSTASY_LIB} PRIVATE "C:\\Users\\fathomless\\lib\\filament-v1.46.0-windows\\include")
        target_link_directories(${ECSTASY_LIB} INTERFACE "C:\\Users\\fathomless\\lib\\filament-v1.46.0-windows\\lib\\x86_64\\md")
    endif()

else()
    target_link_libraries(${ECSTASY_LIB} PRIVATE c++)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_include_directories(${ECSTASY_LIB} PRIVATE /home/fathomless/lib/filament/out/debug/filament/include)
        target_link_directories(${ECSTASY_LIB} INTERFACE /home/fathomless/lib/filament/out/debug/filament/lib/x86_64)
    else()
        target_include_directories(${ECSTASY_LIB} PRIVATE /home/fathomless/lib/filament/out/release/filament/include)
        target_link_directories(${ECSTASY_LIB} INTERFACE /home/fathomless/lib/filament/out/release/filament/lib/x86_64)
    endif()
endif()

target_link_libraries(${ECSTASY_LIB} PRIVATE 
    fmt::fmt 
    Eigen3::Eigen 
    EnTT::EnTT 
    glfw          
    #${stb_SOURCE_DIR}
    #OpenEXR::OpenEXR
    camutils
    # filagui
    filamat
    filament-iblprefilter
    filament
    backend
    bluegl
    bluevk
    filabridge
    filaflat
    geometry
    smol-v
    vkshaders
    matdbg 
    utils
    shaders
    image
    ibl
    )

set_target_properties(${ECSTASY_LIB} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
