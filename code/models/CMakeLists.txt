set(CMAKE_AUTOMOC ON)

set(MODULE_NAME models)

set(SOURCES_LIST
    ${CMAKE_CURRENT_LIST_DIR}/src/tunnel.cpp
)

set(HEADERS_LIST
    ${CMAKE_CURRENT_LIST_DIR}/inc/tunnel.h
)

add_library(${MODULE_NAME} STATIC ${SOURCES_LIST})

target_sources(${MODULE_NAME} PRIVATE ${HEADERS_LIST})

target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

target_link_libraries(${MODULE_NAME} PUBLIC 
    Qt${QT_VERSION_MAJOR}::Core
)