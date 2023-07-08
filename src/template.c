#include "templator/command_tokenizer.h"
#include "templator/error.h"
#include "templator/parser.h"
#include "templator/template.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_VARIABLES_CAPACITY 10
#define INITIAL_INSTRUCTIONS_CAPACITY 20

int templator_template_is_opening_bracket(char* data, size_t len) {
    return strncmp(data, "{%", (len >= 2) ? 2 : len) == 0;
}

int templator_template_is_closing_bracket(char* data, size_t len) {
    return strncmp(data, "%}", (len >= 2) ? 2 : len) == 0;
}

int templator_template_parse(TemplatorTemplate* template, TemplatorParser* parser) {
    template->instructions = malloc(INITIAL_INSTRUCTIONS_CAPACITY * sizeof(TemplatorInstruction));
    template->instructionsCnt = 0;
    template->instructionsCap = INITIAL_INSTRUCTIONS_CAPACITY;
    template->variables = malloc(INITIAL_VARIABLES_CAPACITY * sizeof(char*));
    template->variablesCnt = 0;
    template->variablesCap = INITIAL_VARIABLES_CAPACITY;

    TemplatorParser parsed = templator_parser_read_until_str(parser, templator_template_is_opening_bracket, true); 
    while (parsed.data != NULL) {
        if (parsed.len > 0) {
            TemplatorInstruction* ins = templator_template_add_instruction(template);
            templator_insert_text_instruction_init(ins, parsed.data, parsed.len);
        }
        templator_parser_skip(parser, 2);
        parsed = templator_parser_read_until_str(parser, templator_template_is_closing_bracket, true);
        if (parsed.data == NULL) {
            return TEMPLATOR_INCOMPLETE_INSTRUCTION_BRACKETS;
        }
        templator_parser_skip(parser, 2);
        int res = templator_template_parse_instruction(template, parsed, parser);
        if (res < 0) {
            return res;
        }
        parsed = templator_parser_read_until_str(parser, templator_template_is_opening_bracket, true);
    }

    if (parser->len > 0) {
        TemplatorInstruction* ins = templator_template_add_instruction(template);
        templator_insert_text_instruction_init(ins, parser->data, parser->len);
    }

    return 0;
}

int templator_template_parse_instruction(TemplatorTemplate* template, TemplatorParser commandTemplatorParser, TemplatorParser* afterCommandTemplatorParser) {
    Token tok = templator_parser_next_token(&commandTemplatorParser);
    if (tok.type == NONE) {
        return TEMPLATOR_NO_INSTRUCTION_IN_BRACKETS;
    }
    Token nextToken = templator_parser_next_token(&commandTemplatorParser);
    if (tok.type == KEYWORD_ENDIF && nextToken.type == NONE) {
        return TEMPLATOR_PARSING_ENDED_WITH_ENDIF;
    }


    TemplatorInstruction* instr = templator_template_add_instruction(template);
    instr->type = TEMPLATOR_INSTRUCTION_TYPE_NOOP;
    switch (tok.type) {
        case WORD:
            if (nextToken.type != NONE) {
                return TEMPLATOR_UNABLE_TO_PARSE_INSTRUCTION; 
            }
            templator_insert_variable_instruction_init(instr, templator_template_try_insert_variable(template, tok.data, tok.len));
            return 0;
        case KEYWORD_IF:
            if (nextToken.type != PAREN_OPEN) {
                return TEMPLATOR_UNABLE_TO_PARSE_INSTRUCTION;
            }
            TemplatorComparisonChain cc;
            int res = templator_comparison_chain_parse(&cc, &commandTemplatorParser, template);
            if (res < 0) {
                return res;
            }
            TemplatorTemplate subTemplatorTemplate;
            res = templator_template_parse(&subTemplatorTemplate, afterCommandTemplatorParser);
            if (res != TEMPLATOR_PARSING_ENDED_WITH_ENDIF) {
                if (res == 0) {
                    return TEMPLATOR_PARSING_DIDNT_END_WITH_ENDIF;
                } else {
                    return res;
                }
            }
            templator_insert_conditional_subtemplate(instr, subTemplatorTemplate, cc);
            return 0;
        default:
            return TEMPLATOR_UNABLE_TO_PARSE_INSTRUCTION;
    }

}

void templator_template_free(TemplatorTemplate* template) {
    for (size_t i = 0; i < template->instructionsCnt; ++i) {
        templator_instruction_free(&template->instructions[i]);
    }
    free(template->instructions);
    template->instructions = NULL;
    for (size_t i = 0; i < template->variablesCnt; ++i) {
        free(template->variables[i]);
    }
    free(template->variables);
    template->variables = NULL;
}

TemplatorInstruction* templator_template_add_instruction(TemplatorTemplate* template) {
    if (template->instructionsCnt == template->instructionsCap) {
        template->instructionsCap *= 2;
        template->instructions = realloc(template->instructions, template->instructionsCap * sizeof(TemplatorInstruction));
    }
    return &template->instructions[template->instructionsCnt++];
}

size_t templator_template_try_insert_variable(TemplatorTemplate* template, char* data, size_t len) {
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
