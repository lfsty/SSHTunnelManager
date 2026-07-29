set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(MODULE_NAME view)

set(SOURCES_LIST
    ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/tunnelview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/tunnelview.h
    ${CMAKE_CURRENT_LIST_DIR}/src/tunnelsetupdialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/tunnelsetupdialog.h
    ${CMAKE_CURRENT_LIST_DIR}/src/forwardsetup.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/forwardsetup.h
)

set(HEADERS_LIST
    ${CMAKE_CURRENT_LIST_DIR}/inc/mainwindow.h
)

set(UI_LIST
    ${CMAKE_CURRENT_LIST_DIR}/ui/mainwindow.ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/tunnelview.ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/tunnelsetupdialog.ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/forwardsetup.ui
)

add_library(${MODULE_NAME} STATIC
    ${SOURCES_LIST}
    ${HEADERS_LIST}
    ${UI_LIST}
)

target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

target_link_libraries(${MODULE_NAME} PUBLIC
    models

    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(${MODULE_NAME} PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)