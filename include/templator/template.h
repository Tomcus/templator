#ifndef TEMPLATOR_TEMPLATE_H
#define TEMPLATOR_TEMPLATE_H

#include "instructions.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TTemplate {
    TemplatorInstruction* instructions;
    size_t instructionsCnt;
    size_t instructionsCap;
    char** variables;
    size_t variablesCnt;
    size_t variablesCap;
} Template;

int template_parse(Template* templ, TemplatorParser* parser);
int template_parse_instruction(Template* temp, TemplatorParser commandTemplatorParser, TemplatorParser* afterCommandTemplatorParser);
void template_free(Template* templ);

TemplatorInstruction* template_add_instruction(Template* templ);
size_t template_try_insert_variable(Template* templ, char* data, size_t len);

int template_is_opening_bracket(char* data, size_t len);
int template_is_closing_bracket(char* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_TEMPLATE_H
