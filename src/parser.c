#include "templator/parser.h"

void templator_parser_init(TemplatorParser* parser, char* data, size_t len) {
    parser->data = data;
    parser->len = len;
}

TemplatorParser templator_parser_read_while_str(TemplatorParser* parser, templatorValidateStr validator, bool skipEscaped) {
    TemplatorParser res = *parser;
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

        if (!validator(parser->data + res.len, parser->len - res.len)) {
            parser->data += res.len;
            parser->len -= res.len;
            return res;
        }

        ++res.len;
    }

    parser->data += res.len;
    parser->len -= res.len;
    return res;
}

TemplatorParser templator_parser_read_until_str(TemplatorParser* parser, templatorValidateStr validator, bool skipEscaped) {
    TemplatorParser res = *parser;
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

TemplatorParser templator_parser_read_while_char(TemplatorParser* parser, templatorValidateChar validator, bool skipEscaped) {
    TemplatorParser res = *parser;
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

        if (!validator(parser->data[res.len])) {
            parser->data += res.len;
            parser->len -= res.len;
            return res;
        }

        ++res.len;
    }

    parser->data += res.len;
    parser->len -= res.len;
    return res;
}

TemplatorParser templator_parser_read_until_char(TemplatorParser* parser, templatorValidateChar validator, bool skipEscaped) {
    TemplatorParser res = *parser;
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

void templator_parser_skip(TemplatorParser* parser, size_t count) {
    count = (count <= parser->len) ? count : parser->len;
    parser->data += count;
    parser->len -= count;
}

void templator_parser_skip_while(TemplatorParser* parser, templatorValidateChar validator) {
    while(parser->len > 0 && validator(parser->data[0])) {
        templator_parser_skip(parser, 1);
    }
}

void templator_parser_skip_from_end(TemplatorParser* parser, size_t count) {
    count = (count <= parser->len) ? count : parser->len;
    parser->len -= count;
}

void templator_parser_skip_while_from_end(TemplatorParser* parser, templatorValidateChar validator) {
    while(parser->len > 0 && validator(parser->data[parser->len - 1])) {
        templator_parser_skip_from_end(parser, 1);
    }
}
