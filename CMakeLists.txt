cmake_minimum_required(VERSION 3.5)
project(SSHTunnelManager LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_VERSION_MAJOR 6)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
else()
    add_compile_definitions(RELEASE)
endif()

if(MSVC)
    add_compile_options("/utf-8")
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/code)
