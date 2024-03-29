#include <catch2/catch_test_macros.hpp>

#include "templator/templator.h"
#include "templator/definitions.h"

#include <string>

void realloc_str_and_append(void* data, const char* src, size_t len) {
    std::string* str = (std::string*) data;
    str->append(src, len);
}

TEST_CASE("Test plain text templating", "[templating]") {
    const std::string templatStr = "toto je genericky text";

    Templator templator;
    std::string str{};
    templator_init(&templator);

    int res = templator_run_external(&templator, (char*)templatStr.c_str(), templatStr.size(), NULL, (void*)&str, realloc_str_and_append);
    REQUIRE(res == 0);
    REQUIRE(str == templatStr);

    templator_free(&templator);
}

TEST_CASE("Test variable insertion templating", "[templating]") {
    const std::string templatStr = "a: {%a%}, b: {% b %}, c: {%   c %}, d: {% d   %}, a: {%a %}, b: {% b%}, test";

    Templator templator;
    std::string str;
    templator_init(&templator);

    TemplatorVariables vars;
    templator_variables_init(&vars);

    templator_variables_set_int_variable(&vars, "a", -69);
    templator_variables_set_uint_variable(&vars, "b", 420);
    templator_variables_set_cpy_str_variable(&vars, "c", (char *)"coppied text", TEMPLATOR_STR_NUL_TERMINATED);
    templator_variables_set_str_variable(&vars, "d", (char *)"original text", 13);

    int res = templator_run_external(&templator, (char*)templatStr.c_str(), templatStr.size(), &vars, (void*)&str, realloc_str_and_append);
    REQUIRE(res == 0);
    REQUIRE(str == "a: -69, b: 420, c: coppied text, d: original text, a: -69, b: 420, test");
    templator_variables_free(&vars);
    templator_free(&templator);
}

