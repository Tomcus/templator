enable_testing()

add_executable(test_basics
    test_basics.c
)

target_link_libraries(test_basics PUBLIC templator)

add_test(test_basics ./test_basics)
