find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

add_executable(tst_tunnel
    ${CMAKE_CURRENT_LIST_DIR}/tst_tunnel.cpp
)

target_link_libraries(tst_tunnel PRIVATE
    models
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
)

add_test(NAME tst_tunnel COMMAND tst_tunnel)

# 本地直接运行 ctest 时也能找到 Qt 运行时 DLL
set_tests_properties(tst_tunnel PROPERTIES
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt${QT_VERSION_MAJOR}::Core>"
)
