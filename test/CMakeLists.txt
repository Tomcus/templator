enable_testing()

file(GLOB TEST_FILES test_*.cpp)

add_executable(unit_tests
    ${TEST_FILES}
)

target_link_libraries(unit_tests PUBLIC
    templator
    Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(unit_tests)
