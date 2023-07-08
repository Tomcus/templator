#include "templator/templator.h"
#include "templator/error.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#define TEMPLATOR_INITIAL_TEMPLATE_CAPACITY 10
#define TEMPLATOR_BUFFER_SIZE 1024

void templator_init(Templator* templator) {
    templator->templates = malloc(sizeof(NameAndTemplatePair) * TEMPLATOR_INITIAL_TEMPLATE_CAPACITY);
    templator->templatesCap = TEMPLATOR_INITIAL_TEMPLATE_CAPACITY;
    templator->templatesCnt = 0;
}

void templator_free(Templator* templator) {
    for (size_t i = 0; i < templator->templatesCnt; ++i) {
        template_free(&templator->templates[0].templ);
    }
    free(templator->templates);
}

int templator_add_named_template(Templator* templator, const char* name, char* data, size_t len) {
    if (templator_get_template_by_name(templator, name) != NULL) {
        return 0;
    }
    
    if (templator->templatesCnt == templator->templatesCap) {
        templator->templatesCap *= 2;
        templator->templates = realloc(templator->templates, templator->templatesCap * sizeof(NameAndTemplatePair));
    }

    NameAndTemplatePair* nameAndTemplate = &templator->templates[templator->templatesCnt];
    
    nameAndTemplate->name = name;
    Parser parser;
    parser_init(&parser, data, len);
    int res = template_parse(&nameAndTemplate->templ, &parser);
    if (res < 0) {
        return res;
    }

    templator->templatesCnt ++;

    return 1;
}

Template* templator_get_template_by_name(const Templator* templator, const char* name) {
    for (size_t i = 0; i < templator->templatesCnt; ++i) {
        if (strcmp(templator->templates[i].name, name) == 0) {
            return &templator->templates[i].templ;
        }
    }
    return NULL;
}

int templator_run_named(const Templator* templator, const char* name, Variables* variables, void* data, AppendFunction appendFunction) {
    Template* template = templator_get_template_by_name(templator, name);
    
    if (template != NULL) {
        return templator_run(templator, template, variables, data, appendFunction);
    }
    return -1;
}

int templator_run_external(const Templator* templator, char* templateData, size_t templateLen, Variables* variables, void* data, AppendFunction appendFunction) {
    Template template;
    Parser parser;
    parser_init(&parser, templateData, templateLen);
    int res = template_parse(&template, &parser);

    if (res < 0) {
        template_free(&template);
        return res;    
    }

    res = templator_run(templator, &template, variables, data, appendFunction);

    template_free(&template);
    return res;
}

int templator_run(const Templator* templator, Template* template, Variables* variables, void* data, AppendFunction appendFunction) {
    char buffer[TEMPLATOR_BUFFER_SIZE];
    for (size_t i = 0; i < template->instructionsCnt; ++i) {
        buffer[0] = 0;
        Instruction* ins = &template->instructions[i];
        switch (ins->type) {
            case INSERT_TEXT:
                appendFunction(data, ins->insertTextData.data, ins->insertTextData.len);
            break;
            case INSERT_VARIABLE_VALUE: {
                size_t variableNameIndex = ins->insertVariableData.nameIndex;
                char* variableName = template->variables[variableNameIndex];
                Variable* var = variables_get_variable(variables, variableName);

                if (var == NULL) {
                    return TEMPLATE_VARIABLE_NOT_SET;
                }

                switch (var->type) {
                    case VARIABLE_CSTR_REF:
                    case VARIABLE_CSTR_OWN:
                        appendFunction(data, var->s.data, var->s.len);
                    break;
                    case VARIABLE_INT: {
                        int len = snprintf(buffer, sizeof(var->i), "%"PRIdMAX, var->i);
                        appendFunction(data, buffer, (size_t)len);
                    }
                    break;
                    case VARIABLE_UINT: {
                        int len = snprintf(buffer, sizeof(var->i), "%"PRIuMAX, var->u);
                        appendFunction(data, buffer, (size_t)len);
                    }
                    break;
                }
            }
            break;
            case CONDITIONAL_TEXT_INSERT: {
                int res = comparison_chain_eval(&ins->conditionalInsertTextData.chain, template, variables);
                if (res < 0) {
                    return res;
                }
                if (res) {
                    res = templator_run(templator, ins->conditionalInsertTextData.templ, variables, data, appendFunction);
                    if (res < 0) {
                        return res;
                    }
                }
            }
            break;
            case NOOP:
            break;
        }
    }
    return 0;
}
