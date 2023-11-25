add_subdirectory(src)
#add_subdirectory(third_party)

add_library(${ECSTASY_LIB} STATIC ${ECSTASY_SOURCES})
#target_compile_options(${ECSTASY_LIB} PRIVATE -Wall -Wextra -Werror)

#target_include_directories( ${ECSTASY_LIB} PRIVATE ${stb_SOURCE_DIR})
#target_include_directories( ${ECSTASY_LIB} PRIVATE ${EnTT_SOURCE_DIR}/src)
#target_include_directories(
#        ${ECSTASY_LIB} PRIVATE 
#        ${webgpu_SOURCE_DIR}/include
#        ${webgpu_SOURCE_DIR}/include/webgpu
#        )

target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
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
    ${stb_SOURCE_DIR}
    filament
    backend
    bluegl
    bluevk
    filabridge
    filaflat
    utils
    geometry
    smol-v
    vkshaders
    ibl
    matdbg 
    filamat
    utils
    shaders
    vkshaders
    )

