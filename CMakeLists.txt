cmake_minimum_required(VERSION 3.22)

project(SSHTunnelManager VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_VERSION_MAJOR 6)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Widgets Network)

qt_standard_project_setup()

qt_policy(SET QTP0001 NEW)

if(MSVC)
    add_compile_options("/utf-8")
endif()

include(CTest)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/code)
