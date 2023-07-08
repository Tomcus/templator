#ifndef TEMPLATOR_CONDITIONAL_INSERT_TEXT_INSTRUCTION_H
#define TEMPLATOR_CONDITIONAL_INSERT_TEXT_INSTRUCTION_H

#include "templator/logical_operations.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    TemplatorComparisonChain chain;
    struct TTemplatorTemplate* templ;
} TemplatorInsertConditionalSubtemplateData;

void templator_insert_conditional_subtemplate_data_init(TemplatorInsertConditionalSubtemplateData* citd, struct TTemplatorTemplate innerTemplatorTemplate, TemplatorComparisonChain chain);
void templator_insert_conditional_subtemplate_data_free(TemplatorInsertConditionalSubtemplateData* citd);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_CONDITIONAL_INSERT_TEXT_INSTRUCTION_H

