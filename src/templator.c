#include "templator/templator.h"
#include "templator/error.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#define TEMPLATOR_INITIAL_TEMPLATE_CAPACITY 10

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

bool templator_add_named_template(Templator* templator, const char* name, char* data, size_t len) {
    if (templator_get_template_by_name(templator, name) != NULL) {
        return false;
    }
    
    if (templator->templatesCnt == templator->templatesCap) {
        templator->templatesCap *= 2;
        templator->templates = realloc(templator->templates, templator->templatesCap * sizeof(NameAndTemplatePair));
    }

    NameAndTemplatePair* nameAndTemplate = &templator->templates[templator->templatesCnt];
    
    nameAndTemplate->name = name;
    template_parse(&nameAndTemplate->templ, data, len);

    templator->templatesCnt ++;

    return true;
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
    template_parse(&template, templateData, templateLen);

    return templator_run(templator, &template, variables, data, appendFunction);
}

int templator_run(const Templator* templator, Template* template, Variables* variables, void* data, AppendFunction appendFunction) {
    (void) templator;
    for (size_t i = 0; i < template->instructionsCnt; ++i) {
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
                        char buffer[sizeof(var->i)];
                        int len = snprintf(buffer, sizeof(var->i), "%"PRIdMAX, var->i);
                        appendFunction(data, buffer, (size_t)len);
                    }
                    break;
                    case VARIABLE_UINT: {
                        char buffer[sizeof(var->i)];
                        int len = snprintf(buffer, sizeof(var->i), "%"PRIuMAX, var->u);
                        appendFunction(data, buffer, (size_t)len);
                    }
                    break;
                }
            }
            break;
        }
    }
    return 0;
}
