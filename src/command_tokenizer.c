#include "templator/command_tokenizer.h"
#include "templator/error.h"

#include <ctype.h>
#include <string.h>

int templator_is_operator(int character) {
    switch(character) {
        case '+':
        case '-':
        case '/':
        case '%':
        case '>':
        case '<':
        case '=':
        case '&':
        case '|':
        case '!':
            return true;
        default:
            return false;
    }
}

Token templator_parser_next_token(Parser* parser) {
    Token res;
    res.data = NULL;
    res.len = 0;
    res.type = NONE;

    parser_skip_while(parser, isspace);
    if (parser->len == 0) {
        return res; 
    }

    if (isdigit(parser->data[0])) {
        Parser parsed = parser_read_while_char(parser, isdigit, true);
        res.data = parsed.data;
        res.len = parsed.len;
        res.type = NUMB;
        return res;
    }

    if (isalpha(parser->data[0])) {
        Parser parsed = parser_read_while_char(parser, isalpha, true);
        res.data = parsed.data;
        res.len = parsed.len;
        res.type = token_get_word_type(parsed);
        return res;
    }

    if (templator_is_operator(parser->data[0])) {
        Parser parsed = parser_read_while_char(parser, templator_is_operator, true);
        res.data = parsed.data;
        res.len = parsed.len;
        res.type = OPERATOR;
        return res;
    }

    res.data = parser->data;
    res.len = 1;
    res.type = CHAR;

    if (parser->data[0] == '(') {
        res.type = PAREN_OPEN;
    }

    if (parser->data[0] == ')') {
        res.type = PAREN_CLOSE;
    }

    parser_skip(parser, 1);
    return res;
}

Token templator_parser_peek_token(Parser parser) {
    return templator_parser_next_token(&parser);
}

#define MIN(a,b) (a > b) ? b : a

TOKEN_TYPE token_get_word_type(Parser parser) {
    if (strncmp("if", parser.data, MIN(parser.len, 2)) == 0) {
        return KEYWORD_IF;
    }
    if (strncmp("endif", parser.data, MIN(parser.len, 5)) == 0) {
        return KEYWORD_ENDIF;
    }

    return WORD;
}
