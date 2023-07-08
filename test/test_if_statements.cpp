#include <catch2/catch_test_macros.hpp>

#include "templator/templator.h"
#include "templator/definitions.h"

#include <string>
#include <vector>

void append_str(void* data, const char* src, size_t len) {
    std::string* str = (std::string*) data;
    str->append(src, len);
}

TEST_CASE("Test if statements", "[templating]") {
    const std::string templateStr = R"(var: {% var %},{% if (var == 1) %} var is one{% endif %})";

    Templator templator;
    std::string str{};
    templator_init(&templator);

    Variables vars;
    variables_init(&vars);

    variables_set_int_variable(&vars, "var", 0);

    int res = templator_run_external(&templator, (char*)templateStr.c_str(), templateStr.size(), &vars, (void*)&str, append_str);
    REQUIRE(res == 0);
    REQUIRE(str == R"(var: 0,)");
    str.clear();

    variables_set_int_variable(&vars, "var", 1);
    res = templator_run_external(&templator, (char*)templateStr.c_str(), templateStr.size(), &vars, (void*)&str, append_str);
    REQUIRE(res == 0);
    REQUIRE(str == R"(var: 1, var is one)");

    variables_free(&vars);
    templator_free(&templator);
}

TEST_CASE("Test invalid parsing", "[templating]") {
    std::vector<std::string> testData = {
        "{% if %}",
        "{% if ( %}}",
        "{% if (a > 0) %}",
        "{% if (a > 0 && a < 100 || a != 500) %}{%endif%}"
    };

    TemplatorTemplate templ;

    for (auto& str: testData) {
        TemplatorParser parser;
        parser.data = str.data();
        parser.len = str.size();
        int res = templator_template_parse(&templ, &parser);
        REQUIRE(res < 0);
        templator_template_free(&templ);
    }
}

TEST_CASE("Test multiple comparisons", "[templating]") {
    std::vector<std::pair<std::string, std::string>> testData = {
        std::pair{"{% if (val > 0 && val < 2) %}{% val %}{% endif %}", "1"},
        std::pair{"{% if (val < 0 || val >= 1) %}{% val %}{% endif %}", "1"},
        std::pair{"{% if (val < 0 || (val)) %}{% val %}{% endif %}", "1"},
        std::pair{"{% if (val < 0 || val) %}{% val %}{% endif %}", "1"},
        std::pair{"{% if (val && val != 0) %}{% val %}{% endif %}", "1"},
    };

    Templator templator;
    templator_init(&templator);

    Variables vars;
    variables_init(&vars);

    variables_set_int_variable(&vars, "val", 1);

    for (auto& [input, expected]: testData) {
        std::string str{};
        int res = templator_run_external(&templator, input.data(), input.size(), &vars, (void *)&str, append_str);

        REQUIRE(res == 0);
        REQUIRE(str == expected);
    }

    variables_free(&vars);
    templator_free(&templator);
}
