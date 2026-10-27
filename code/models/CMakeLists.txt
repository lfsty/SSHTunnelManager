set(MODULE_NAME models)

qt_add_library(${MODULE_NAME} STATIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/ForwardData.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/LogManager.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/SshOutput.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/Tunnel.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/TunnelListModel.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/TunnelManager.h
    ${CMAKE_CURRENT_LIST_DIR}/src/ForwardData.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/LogManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SshOutput.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Tunnel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TunnelListModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TunnelManager.cpp
)

target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

target_link_libraries(${MODULE_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
)
