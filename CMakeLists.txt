cmake_minimum_required(VERSION 3.16)

project(vhgguftool VERSION 0.1.0 LANGUAGES CXX)

# set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_BUILD_TYPE "MinSizeRel")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version_vhgguftool.h"
)


# Подключаем папку с субмодулем
add_subdirectory(src/extmods/vhlibplatform)

# Подключаем папку с субмодулем
add_subdirectory(src/extmods/vhlibargs)

# Подключаем папку с субмодулем
add_subdirectory(src/extmods/vhlibfsys)

# Подключаем папку с субмодулем
# add_subdirectory(src/extmods/vhlibsvg)


#
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/visual.cpp
    src/cli.cpp
    src/mods/gguf/vhgguf.cpp
    src/mods/gguf/vhggufmetainfo.cpp
    src/mods/gguf/vhgguftensor.cpp
    src/mods/texttools/texttools.cpp

    src/cmd_show_signs_map.cpp
)

#
target_include_directories(${PROJECT_NAME} PUBLIC
    src
    src/mods
    src/mods/gguf
    ${CMAKE_CURRENT_BINARY_DIR}/generated
#    src/extmods
)

# Defines
# target_compile_definitions(${PROJECT_NAME} PRIVATE
#     # CFG_DISABLE_CRC_CACHE=1
#     VHPLATFORM_PC
# )

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    vhlib_platform
    vhlib_args
    vhlib_fsys
#    vhlib_svg
)
