#ifndef TEMPLATOR_INSTRUCTIONS_H
#define TEMPLATOR_INSTRUCTIONS_H

#include "instructions/insert_text_instruction.h"
#include "instructions/insert_variable_instruction.h"
#include "instructions/insert_conditional_subtemplate.h"

#ifdef __cplusplus
extern "C" {
#endif

struct TTemplatorTemplate;

typedef enum {
    TEMPLATOR_INSTRUCTION_TYPE_INSERT_TEXT,
    TEMPLATOR_INSTRUCTION_TYPE_INSERT_VARIABLE_VALUE,
    TEMPLATOR_INSTRUCTION_TYPE_INSERT_CONDITIONAL_SUBTEMPLATE,

    TEMPLATOR_INSTRUCTION_TYPE_NOOP,
} TEMPLATOR_INSTRUNCTION_TYPE;

typedef struct {
    union {
        TemplatorInsertTextData insertTextData;
        TemplatorInsertVariableData insertVariableData;
        TemplatorInsertConditionalSubtemplateData insertConditionalSubtemplateData;
    };
    TEMPLATOR_INSTRUNCTION_TYPE type;
} TemplatorInstruction;

void templator_insert_text_instruction_init(TemplatorInstruction* instruction, char* data, size_t len);
void templator_insert_variable_instruction_init(TemplatorInstruction* instruction, size_t nameIndex);
void templator_insert_conditional_subtemplate(TemplatorInstruction* TemplatorInstruction, struct TTemplatorTemplate subTemplatorTemplate, TemplatorComparisonChain comparison);

void templator_instruction_free(TemplatorInstruction* instruction);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_INSTRUCTIONS_H
