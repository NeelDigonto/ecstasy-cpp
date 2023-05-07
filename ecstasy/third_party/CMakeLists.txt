if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/temp/dawn.tar.gz)
   file(DOWNLOAD 
        "https://github.com/hexops/mach-gpu-dawn/releases/download/release-c46a59e/x86_64-windows-gnu_release-fast.tar.gz"
        ${CMAKE_CURRENT_SOURCE_DIR}/temp/dawn.tar.gz
        TLS_VERIFY ON)
file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_SOURCE_DIR}/temp/dawn.tar.gz DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/dawn)
endif()

target_include_directories(
        ${ECSTASY_LIB} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/dawn/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dawn/include/webgpu
        )
        
target_link_libraries(
        ${ECSTASY_LIB} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/dawn/dawn.lib
        )

