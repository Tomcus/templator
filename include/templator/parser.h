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
} Parser;

void parser_init(Parser* parser, char* data, size_t len);

Parser parser_read_until_str(Parser* parser, validateStr validator, bool skipEscaped);
Parser parser_read_while_str(Parser* parser, validateStr validator, bool skipEscaped);
Parser parser_read_until_char(Parser* parser, validateChar validator, bool skipEscaped);
Parser parser_read_while_char(Parser* parser, validateChar validator, bool skipEscaped);

void parser_skip(Parser* parser, size_t count);
void parser_skip_from_end(Parser* parser, size_t count);
void parser_skip_while(Parser* parser, validateChar validator);
void parser_skip_while_from_end(Parser* parser, validateChar validator);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_PARSER_H
