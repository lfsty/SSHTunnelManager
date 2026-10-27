set(EXE_NAME ${PROJECT_NAME})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/models)

set(QML_DIR ${CMAKE_CURRENT_LIST_DIR}/qml)

set(QML_FILES
    ${QML_DIR}/Main.qml
    ${QML_DIR}/Theme.qml
    ${QML_DIR}/AppButton.qml
    ${QML_DIR}/TunnelCard.qml
    ${QML_DIR}/TunnelSetupDialog.qml
    ${QML_DIR}/ForwardRow.qml
    ${QML_DIR}/WindowControlButton.qml
)

# 让 QML 文件位于模块根目录，而不是镜像源码目录层级
foreach(QML_FILE ${QML_FILES})
    get_filename_component(QML_FILE_NAME ${QML_FILE} NAME)
    set_source_files_properties(${QML_FILE} PROPERTIES QT_RESOURCE_ALIAS ${QML_FILE_NAME})
endforeach()

set_source_files_properties(
    ${QML_DIR}/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_add_executable(${EXE_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

qt_add_resources(${EXE_NAME} "app_resources"
    PREFIX "/"
    BASE ${CMAKE_CURRENT_LIST_DIR}/resources
    FILES ${CMAKE_CURRENT_LIST_DIR}/resources/img/icon.png
)

qt_add_qml_module(${EXE_NAME}
    URI SSHTunnelManager
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

set_target_properties(${EXE_NAME} PROPERTIES
    WIN32_EXECUTABLE $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Release>>:TRUE>
)

target_link_libraries(${EXE_NAME} PRIVATE
    models
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::Widgets
)

if(BUILD_TESTING)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
endif()
