#ifndef TEMPLATOR_INSTRUCTIONS_H
#define TEMPLATOR_INSTRUCTIONS_H

#include "instructions/insert_text_instruction.h"
#include "instructions/insert_variable_instruction.h"

typedef enum {
    INSERT_TEXT,
    INSERT_VARIABLE_VALUE
} INSTRUCTION_TYPE;

typedef struct {
    union {
        InsertTextData insertTextData;
        InsertVariableData insertVariableData;
    };
    INSTRUCTION_TYPE type;
} Instruction;

void insert_text_instruction_init(Instruction* instruction, char* data, size_t len);
void insert_variable_instruction_init(Instruction* instruction, size_t nameIndex);

void instruction_free(Instruction* instruction);

#endif//TEMPLATOR_INSTRUCTIONS_H
