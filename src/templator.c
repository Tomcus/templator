#include "templator/templator.h"
#include "templator/error.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#define TEMPLATOR_INITIAL_TEMPLATE_CAPACITY 10
#define TEMPLATOR_BUFFER_SIZE 1024

void templator_init(Templator* templator) {
    templator->templates = malloc(sizeof(TemplatorNameAndTemplatePair) * TEMPLATOR_INITIAL_TEMPLATE_CAPACITY);
    templator->templatesCap = TEMPLATOR_INITIAL_TEMPLATE_CAPACITY;
    templator->templatesCnt = 0;
}

void templator_free(Templator* templator) {
    for (size_t i = 0; i < templator->templatesCnt; ++i) {
        templator_template_free(&templator->templates[0].templ);
    }
    free(templator->templates);
}

int templator_add_named_template(Templator* templator, const char* name, char* data, size_t len) {
    if (templator_get_template_by_name(templator, name) != NULL) {
        return 0;
    }
    
    if (templator->templatesCnt == templator->templatesCap) {
        templator->templatesCap *= 2;
        templator->templates = realloc(templator->templates, templator->templatesCap * sizeof(TemplatorNameAndTemplatePair));
    }

    TemplatorNameAndTemplatePair* nameAndTemplate = &templator->templates[templator->templatesCnt];
    
    nameAndTemplate->name = name;
    TemplatorParser parser;
    templator_parser_init(&parser, data, len);
    int res = templator_template_parse(&nameAndTemplate->templ, &parser);
    if (res < 0) {
        return res;
    }

    templator->templatesCnt ++;

    return 1;
}

TemplatorTemplate* templator_get_template_by_name(const Templator* templator, const char* name) {
    for (size_t i = 0; i < templator->templatesCnt; ++i) {
        if (strcmp(templator->templates[i].name, name) == 0) {
            return &templator->templates[i].templ;
        }
    }
    return NULL;
}

int templator_run_named(const Templator* templator, const char* name, TemplatorVariables* variables, void* data, TemplatorAppendStrFunction appendFunction) {
    TemplatorTemplate* template = templator_get_template_by_name(templator, name);
    
    if (template != NULL) {
        return templator_run(templator, template, variables, data, appendFunction);
    }
    return -1;
}

int templator_run_external(const Templator* templator, char* templateData, size_t templateLen, TemplatorVariables* variables, void* data, TemplatorAppendStrFunction appendFunction) {
    TemplatorTemplate template;
    TemplatorParser parser;
    templator_parser_init(&parser, templateData, templateLen);
    int res = templator_template_parse(&template, &parser);

    if (res < 0) {
        templator_template_free(&template);
        return res;    
    }

    res = templator_run(templator, &template, variables, data, appendFunction);

    templator_template_free(&template);
    return res;
}

int templator_run(const Templator* templator, TemplatorTemplate* template, TemplatorVariables* variables, void* data, TemplatorAppendStrFunction appendFunction) {
    char buffer[TEMPLATOR_BUFFER_SIZE];
    for (size_t i = 0; i < template->instructionsCnt; ++i) {
        buffer[0] = 0;
        TemplatorInstruction* ins = &template->instructions[i];
        switch (ins->type) {
            case TEMPLATOR_INSTRUCTION_TYPE_INSERT_TEXT:
                appendFunction(data, ins->insertTextData.data, ins->insertTextData.len);
            break;
            case TEMPLATOR_INSTRUCTION_TYPE_INSERT_VARIABLE_VALUE: {
                size_t variableNameIndex = ins->insertVariableData.nameIndex;
                char* variableName = template->variables[variableNameIndex];
                TemplatorVariable* var = templator_variables_get_variable(variables, variableName);

                if (var == NULL) {
                    return TEMPLATOR_VARIABLE_NOT_SET;
                }

                switch (var->type) {
                    case TEMPLATOR_VARIABLE_TYPE_CSTR_REF:
                    case TEMPLATOR_VARIABLE_TYPE_CSTR_OWN:
                        appendFunction(data, var->s.data, var->s.len);
                    break;
                    case TEMPLATOR_VARIABLE_TYPE_INT: {
                        int len = snprintf(buffer, sizeof(var->i), "%"PRIdMAX, var->i);
                        appendFunction(data, buffer, (size_t)len);
                    }
                    break;
                    case TEMPLATOR_VARIABLE_TYPE_UINT: {
                        int len = snprintf(buffer, sizeof(var->i), "%"PRIuMAX, var->u);
                        appendFunction(data, buffer, (size_t)len);
                    }
                    break;
                }
            }
            break;
            case TEMPLATOR_INSTRUCTION_TYPE_INSERT_CONDITIONAL_SUBTEMPLATE: {
                int res = templator_comparison_chain_eval(&ins->insertConditionalSubtemplateData.chain, template, variables);
                if (res < 0) {
                    return res;
                }
                if (res) {
                    res = templator_run(templator, ins->insertConditionalSubtemplateData.templ, variables, data, appendFunction);
                    if (res < 0) {
                        return res;
                    }
                }
            }
            break;
            case TEMPLATOR_INSTRUCTION_TYPE_NOOP:
            break;
        }
    }
    return 0;
}
