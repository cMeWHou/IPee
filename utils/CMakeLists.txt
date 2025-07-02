# Need for generate compile_command.json file (used by clangd)
target_compile_definitions(${PROJECT_LIB} PUBLIC CMAKE_EXPORT_COMPILE_COMMANDS=1)
