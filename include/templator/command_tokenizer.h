#ifndef TEMPLATOR_COMMAND_TOKENIZER_H
#define TEMPLATOR_COMMAND_TOKENIZER_H

#include <stdlib.h>

#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NONE,
    KEYWORD_IF,
    KEYWORD_ENDIF,
    WORD,
    NUMB,
    OPERATOR,
    PAREN_OPEN,
    PAREN_CLOSE,
    CHAR
} TOKEN_TYPE;

typedef struct {
    char* data;
    size_t len;
    TOKEN_TYPE type;
} Token;

int templator_is_operator(int character);
Token templator_parser_next_token(TemplatorParser* parser);
Token templator_parser_peek_token(TemplatorParser parser);
TOKEN_TYPE templator_get_word_type(TemplatorParser parser);

#ifdef __cplusplus
}
#endif

#endif // TEMPLATOR_COMMAND_TOKENIZER_H
