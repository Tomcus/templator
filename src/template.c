#include "templator/template.h"
#include "templator/parser.h"
#include "templator/error.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_VARIABLES_CAPACITY 10
#define INITIAL_INSTRUCTIONS_CAPACITY 20

int template_is_opening_bracket(char* data, size_t len) {
    return strncmp(data, "{%", (len >= 2) ? 2 : len) == 0;
}

int template_is_closing_bracket(char* data, size_t len) {
    return strncmp(data, "%}", (len >= 2) ? 2 : len) == 0;
}

int template_parse(Template* template, char* data, size_t len) {
    template->instructions = malloc(INITIAL_INSTRUCTIONS_CAPACITY * sizeof(Instruction));
    template->instructionsCnt = 0;
    template->instructionsCap = INITIAL_INSTRUCTIONS_CAPACITY;
    template->variables = malloc(INITIAL_VARIABLES_CAPACITY * sizeof(char*));
    template->variablesCnt = 0;
    template->variablesCap = INITIAL_VARIABLES_CAPACITY;

    Parser parser;
    parser_init(&parser, data, len);

    Parser parsed = parser_read_until_str(&parser, template_is_opening_bracket, true); 
    while (parsed.data != NULL) {
        if (parsed.len > 0) {
            Instruction* ins = template_add_instruction(template);
            insert_text_instruction_init(ins, parsed.data, parsed.len);
        }
        parser_skip(&parser, 2);
        parsed = parser_read_until_str(&parser, template_is_closing_bracket, true);
        if (parsed.data == NULL) {
            return TEMPLATE_INCOMPLETE_INSTRUCTION_BRACKETS;
        }
        parser_skip(&parser, 2);
        int res = template_parse_instruction(template, parsed);
        if (res < 0) {
            return res;
        }
        parsed = parser_read_until_str(&parser, template_is_opening_bracket, true);
    }

    if (parser.len > 0) {
        Instruction* ins = template_add_instruction(template);
        insert_text_instruction_init(ins, parser.data, parser.len);
    }

    return 0;
}

int template_parse_instruction(Template* template, Parser parser) {
    parser_skip_while(&parser, isspace);
    parser_skip_while_from_end(&parser, isspace);
    if (parser.len > 0) {
        size_t index = template_try_insert_variable(template, parser.data, parser.len);
        Instruction* ins = template_add_instruction(template);
        insert_variable_instruction_init(ins, index);
        return 0;
    }

    return TEMPLATE_NO_INSTRUCTION_IN_BRACKETS;
}

void template_free(Template* template) {
    for (size_t i = 0; i < template->instructionsCnt; ++i) {
        instruction_free(&template->instructions[i]);
    }
    for (size_t i = 0; i < template->variablesCnt; ++i) {
        free(template->variables);
    }
}

Instruction* template_add_instruction(Template* template) {
    if (template->instructionsCnt == template->instructionsCap) {
        template->instructionsCap *= 2;
        template->instructions = realloc(template->instructions, template->instructionsCap * sizeof(Instruction));
    }
    return &template->instructions[template->instructionsCnt++];
}

size_t template_try_insert_variable(Template* template, char* data, size_t len) {
    for (size_t i = 0; i < template->variablesCnt; ++i) {
        if (strncmp(template->variables[i], data, len) == 0) {
            return i;
        }
    }
    if (template->variablesCnt == template->variablesCap) {
        template->variablesCap *= 2;
        template->variables = realloc(template->variables, sizeof(char*) * template->variablesCap);
    }
    template->variables[template->variablesCnt] = malloc(len + 1);
    strncpy(template->variables[template->variablesCnt], data, len);

    template->variables[template->variablesCnt][len] = 0;
    return template->variablesCnt++;
}
