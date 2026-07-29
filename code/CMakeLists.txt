set(CMAKE_AUTORCC ON)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/models)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/view)

set(EXE_NAME ${PROJECT_NAME})

add_executable(${EXE_NAME} 
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/resources/resource.qrc
)

set_target_properties(${EXE_NAME} PROPERTIES
    WIN32_EXECUTABLE $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Release>>:TRUE>
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

target_link_libraries(${EXE_NAME} PRIVATE
    models
    view
)
