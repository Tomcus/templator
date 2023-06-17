#include "templator/instructions.h"

void insert_text_instruction_init(Instruction* instruction, char* data, size_t len) {
    instruction->type = INSERT_TEXT;
    insert_text_data_init(&instruction->insertTextData, data, len);
}

void insert_variable_instruction_init(Instruction* instruction, size_t nameIndex) {
    instruction->type = INSERT_VARIABLE_VALUE;
    insert_variable_data_init(&instruction->insertVariableData, nameIndex);
}

void instruction_free(Instruction* instruction) {
    switch(instruction->type) {
        case INSERT_TEXT:
            insert_text_data_free(&instruction->insertTextData);
            break;
        case INSERT_VARIABLE_VALUE:
            insert_variable_data_free(&instruction->insertVariableData);
            break;
    }
}
