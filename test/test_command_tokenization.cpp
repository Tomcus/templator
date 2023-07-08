#include <catch2/catch_test_macros.hpp>

#include "templator/parser.h"
#include "templator/command_tokenizer.h"

#include <string_view>
#include <vector>

TEST_CASE("Test parsing basic commands", "[parsing][tokenization]") {
    struct TestData {
        std::string_view command;
        std::vector<TOKEN_TYPE> expected_tokens;
    };

    std::vector<TestData> data = {
        TestData{"if (variable >= 123)", {KEYWORD_IF, PAREN_OPEN, WORD, OPERATOR, NUMB, PAREN_CLOSE, NONE}},
        TestData{"   endif ", {KEYWORD_ENDIF, NONE}},
        TestData{"hex(arg, var  , val)", {WORD, PAREN_OPEN, WORD, CHAR, WORD, CHAR, WORD, PAREN_CLOSE, NONE}},
        TestData{"a", {WORD, NONE}},
        TestData{"", {NONE}},
    };

    for (auto& [command, expected_tokens]: data) {
        Parser parser;
        parser.data = (char *)command.data();
        parser.len = command.size();
        for (const auto token: expected_tokens) {
            Token t = templator_parser_next_token(&parser);
            REQUIRE(t.type == token);
        }
    }
}
