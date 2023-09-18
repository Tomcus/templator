cmake_minimum_required(VERSION 3.20)

project(templator)

find_package(Catch2 REQUIRED)

add_library(templator STATIC
    src/avl_tree.c
    src/command_tokenizer.c
    src/error.c
    src/instructions.c
    src/instructions/insert_conditional_subtemplate.c
    src/instructions/insert_text_instruction.c
    src/instructions/insert_variable_instruction.c
    src/logical_operations.c
    src/parser.c
    src/template.c
    src/templator.c
    src/variable_comparisons.c
    src/variables.c
)

target_include_directories(templator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_property(TARGET templator PROPERTY C_STANDARD 23)

target_compile_options(templator PUBLIC
    -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror
    -ggdb3
)

if (DEFINED TEMPLATOR_SANITIZER)
    target_compile_options(templator PUBLIC
        -fsanitize=${TEMPLATOR_SANITIZER}
    )

    target_link_libraries(templator PUBLIC
        asan
    )
endif()

add_subdirectory(test)

