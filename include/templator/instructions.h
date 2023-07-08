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
    INSERT_TEXT,
    INSERT_VARIABLE_VALUE,
    CONDITIONAL_TEXT_INSERT,

    NOOP,
} INSTRUCTION_TYPE;

typedef struct {
    union {
        InsertTextData insertTextData;
        InsertVariableData insertVariableData;
        ConditionalInsertTextData conditionalInsertTextData;
    };
    INSTRUCTION_TYPE type;
} Instruction;

void insert_text_instruction_init(Instruction* instruction, char* data, size_t len);
void insert_variable_instruction_init(Instruction* instruction, size_t nameIndex);
void conditional_insert_text_instruction_init(Instruction* Instruction, struct TTemplate subTemplate, ComparisonChain comparison);

void instruction_free(Instruction* instruction);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_INSTRUCTIONS_H
