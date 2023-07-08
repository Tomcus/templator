#ifndef TEMPLATOR_CONDITIONAL_INSERT_TEXT_INSTRUCTION_H
#define TEMPLATOR_CONDITIONAL_INSERT_TEXT_INSTRUCTION_H

#include "templator/logical_operations.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ComparisonChain chain;
    struct TTemplate* templ;
} ConditionalInsertTextData;

void conditional_insert_text_data_init(ConditionalInsertTextData* citd, struct TTemplate innerTemplate, ComparisonChain chain);
void conditional_insert_text_data_free(ConditionalInsertTextData* citd);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_CONDITIONAL_INSERT_TEXT_INSTRUCTION_H

