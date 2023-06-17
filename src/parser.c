#include "templator/parser.h"

void parser_init(Parser* parser, char* data, size_t len) {
    parser->data = data;
    parser->len = len;
}

Parser parser_read_until_str(Parser* parser, validateStr validator, bool skipEscaped) {
    Parser res = *parser;
    bool escaped = false;
    res.len = 0;

    while (parser->len - res.len > 0) {
        if (skipEscaped && res.data[res.len] == '\\') {
            escaped = true;
            continue;
        }
        if (escaped) {
            escaped = false;
            continue;
        }

        if (validator(parser->data + res.len, parser->len - res.len)) {
            parser->data += res.len;
            parser->len -= res.len;
            return res;
        }

        ++res.len;
    }

    res.len = 0;
    res.data = NULL;
    return res;
}

Parser parser_read_until_char(Parser* parser, validateChar validator, bool skipEscaped) {
    Parser res = *parser;
    bool escaped = false;
    res.len = 0;

    while (parser->len - res.len > 0) {
        if (skipEscaped && res.data[res.len] == '\\') {
            escaped = true;
            continue;
        }
        if (escaped) {
            escaped = false;
            continue;
        }

        if (validator(parser->data[res.len])) {
            parser->data += res.len;
            parser->len -= res.len;
            return res;
        }

        ++res.len;
    }

    res.len = 0;
    res.data = NULL;
    return res;
}

void parser_skip(Parser* parser, size_t count) {
    count = (count <= parser->len) ? count : parser->len;
    parser->data += count;
    parser->len -= count;
}

void parser_skip_while(Parser* parser, validateChar validator) {
    while(parser->len > 0 && validator(parser->data[0])) {
        parser_skip(parser, 1);
    }
}

void parser_skip_from_end(Parser* parser, size_t count) {
    count = (count <= parser->len) ? count : parser->len;
    parser->len -= count;
}

void parser_skip_while_from_end(Parser* parser, validateChar validator) {
    while(parser->len > 0 && validator(parser->data[parser->len - 1])) {
        parser_skip_from_end(parser, 1);
    }
}
