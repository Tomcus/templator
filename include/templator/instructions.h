#ifndef TEMPLATOR_INSTRUCTIONS_H
#define TEMPLATOR_INSTRUCTIONS_H

#include "instructions/insert_text_instruction.h"
#include "instructions/insert_variable_instruction.h"
#include "instructions/conditional_insert_text_instruction.h"

#ifdef __cplusplus
extern "C" {
#endif

struct TTemplate;

typedef enum {
    TEMPLATOR_INSTRUCTION_TYPE_INSERT_TEXT,
    TEMPLATOR_INSTRUCTION_TYPE_INSERT_VARIABLE_VALUE,
    TEMPLATOR_INSTRUCTION_TYPE_INSERT_CONDITIONAL_SUBTEMPLATE,

    TEMPLATOR_INSTRUCTION_TYPE_NOOP,
} TEMPLATOR_INSTRUNCTION_TYPE;

typedef struct {
    union {
        InsertTextData insertTextData;
        InsertVariableData insertVariableData;
        ConditionalInsertTextData conditionalInsertTextData;
    };
    TEMPLATOR_INSTRUNCTION_TYPE type;
} TemplatorInstruction;

void templator_insert_text_instruction_init(TemplatorInstruction* instruction, char* data, size_t len);
void templator_insert_variable_instruction_init(TemplatorInstruction* instruction, size_t nameIndex);
void templator_insert_conditional_subtemplate(TemplatorInstruction* TemplatorInstruction, struct TTemplate subTemplate, TemplatorComparisonChain comparison);

void templator_instruction_free(TemplatorInstruction* instruction);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_INSTRUCTIONS_H
