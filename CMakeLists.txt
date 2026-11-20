cmake_minimum_required(VERSION 3.16)

project(vhgguftool VERSION 0.2.0 LANGUAGES CXX)

# set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_BUILD_TYPE "MinSizeRel")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6: AUTOMOC обрабатывает классы с Q_OBJECT (MainWindow). Для .ui форм / .qrc
# ресурсов можно включить AUTOUIC / AUTORCC на таргете.

# Единая сборка приложения: консоль (CLI) и графический интерфейс (GUI)
# объединены в одном исполняемом файле, режим выбирается во время работы
# (GUI если не задана CLI команда, см. src/main.cpp и src/mods/cli/cli.cpp).
option(VHGGUITOOL_BUILD_CLI "Build vhgguftool application (console + Qt6 Widgets GUI)" ON)

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


# Ядро приложения: общая логика CLI и GUI без entry point.
set(VHGGUITOOL_CORE_SOURCES
    src/mods/misc/visual.cpp
    src/mods/gguf/vhgguf.cpp
    src/mods/gguf/vhggufmetainfo.cpp
    src/mods/gguf/vhgguftensor.cpp
    src/mods/texttools/texttools.cpp
    src/mods/scan/scan.cpp
    src/mods/cmds/cmd_show_signs_map.cpp
    src/mods/cmds/cmd_run_commands.cpp
)

add_library(vhgguftool_core STATIC ${VHGGUITOOL_CORE_SOURCES})

target_include_directories(vhgguftool_core PUBLIC
    src
    src/mods
    src/mods/gguf
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(vhgguftool_core
    PUBLIC
    vhlib_platform
    vhlib_args
    vhlib_fsys
)


# Единая сборка приложения: консоль (CLI) и графический интерфейс (GUI)
# объединены в одном исполняемом файле, режим выбирается во время работы
# (GUI если не задана CLI команда, см. src/main.cpp и src/mods/cli/cli.cpp).
if(VHGGUITOOL_BUILD_CLI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)

    qt_add_executable(${PROJECT_NAME}
        WIN32
        MACOSX_BUNDLE
        src/main.cpp
        src/mods/gui/wnd_main/mainwindow.cpp
        src/mods/gui/wdg_metadata/wdg_metadata.cpp
        src/mods/gui/wdg_tensors/wdg_tensors.cpp
        src/mods/gui/wdg_statusbar/wdg_statusbar.cpp
        src/mods/gui/wdg_tensoritem/wdg_tensoritem.cpp
        src/mods/gui/wdg_vlinesep/vlinesep.cpp
        src/mods/gui/wdg_hlinesep/hlinesep.cpp
        src/mods/gui/wdg_menu/wdg_menu.cpp
        src/mods/cli/cli.cpp
    )

    # moc для классов с Q_OBJECT (MainWindow).
    set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
    # set_target_properties(${PROJECT_NAME} PROPERTIES AUTOUIC ON AUTORCC ON)

    target_include_directories(${PROJECT_NAME} PRIVATE
        src
        src/mods
        src/mods/gguf
        src/mods/gui
        ${CMAKE_CURRENT_BINARY_DIR}/generated
    )

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        vhgguftool_core
        Qt6::Widgets
    )
endif()
