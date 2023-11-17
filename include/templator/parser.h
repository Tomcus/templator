#ifndef TEMPLATOR_PARSER_H
#define TEMPLATOR_PARSER_H

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*templatorValidateChar)(int);
typedef int (*templatorValidateStr)(char*, size_t);

typedef struct {
    char* data;
    size_t len;
} TemplatorParser;

void templator_parser_init(TemplatorParser* parser, char* data, size_t len);

TemplatorParser templator_parser_read_until_str(TemplatorParser* parser, templatorValidateStr validator, bool skipEscaped);
TemplatorParser templator_parser_read_while_str(TemplatorParser* parser, templatorValidateStr validator, bool skipEscaped);
TemplatorParser templator_parser_read_until_char(TemplatorParser* parser, templatorValidateChar validator, bool skipEscaped);
TemplatorParser templator_parser_read_while_char(TemplatorParser* parser, templatorValidateChar validator, bool skipEscaped);

void templator_parser_skip(TemplatorParser* parser, size_t count);
void templator_parser_skip_from_end(TemplatorParser* parser, size_t count);
void templator_parser_skip_while(TemplatorParser* parser, templatorValidateChar validator);
void templator_parser_skip_while_from_end(TemplatorParser* parser, templatorValidateChar validator);

#ifdef __cplusplus
}
#endif

#endif // TEMPLATOR_PARSER_H
