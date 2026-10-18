add_executable(libarchipelagopp-example example.cpp)
target_link_libraries(libarchipelagopp-example libarchipelagopp::libarchipelagopp)

add_custom_target(example_copy_deps
        COMMAND_EXPAND_LISTS
        VERBATIM
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:libarchipelagopp-example> ${CMAKE_CURRENT_BINARY_DIR}
)