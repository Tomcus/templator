#ifndef TEMPLATOR_PARSER_H
#define TEMPLATOR_PARSER_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int(*validateChar)(int);
typedef int(*validateStr)(char*, size_t);

typedef struct {
    char* data;
    size_t len;
} TemplatorParser;

void templator_parser_init(TemplatorParser* parser, char* data, size_t len);

TemplatorParser templator_parser_read_until_str(TemplatorParser* parser, validateStr validator, bool skipEscaped);
TemplatorParser templator_parser_read_while_str(TemplatorParser* parser, validateStr validator, bool skipEscaped);
TemplatorParser templator_parser_read_until_char(TemplatorParser* parser, validateChar validator, bool skipEscaped);
TemplatorParser templator_parser_read_while_char(TemplatorParser* parser, validateChar validator, bool skipEscaped);

void templator_parser_skip(TemplatorParser* parser, size_t count);
void templator_parser_skip_from_end(TemplatorParser* parser, size_t count);
void templator_parser_skip_while(TemplatorParser* parser, validateChar validator);
void templator_parser_skip_while_from_end(TemplatorParser* parser, validateChar validator);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_PARSER_H
