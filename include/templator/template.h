#ifndef TEMPLATOR_TEMPLATE_H
#define TEMPLATOR_TEMPLATE_H

#include "instructions.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TTemplatorTemplate {
    TemplatorInstruction* instructions;
    size_t instructionsCnt;
    size_t instructionsCap;
    char** variables;
    size_t variablesCnt;
    size_t variablesCap;
} TemplatorTemplate;

int templator_template_parse(TemplatorTemplate* templ, TemplatorParser* parser);
int templator_template_parse_instruction(TemplatorTemplate* temp, TemplatorParser commandTemplatorParser, TemplatorParser* afterCommandTemplatorParser);
void templator_template_free(TemplatorTemplate* templ);

TemplatorInstruction* templator_template_add_instruction(TemplatorTemplate* templ);
size_t templator_template_try_insert_variable(TemplatorTemplate* templ, char* data, size_t len);

int templator_template_is_opening_bracket(char* data, size_t len);
int templator_template_is_closing_bracket(char* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_TEMPLATE_H
