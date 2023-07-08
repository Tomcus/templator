#include "templator/instructions.h"
#include "templator/template.h"

void templator_insert_text_instruction_init(TemplatorInstruction* instruction, char* data, size_t len) {
    instruction->type = TEMPLATOR_INSTRUCTION_TYPE_INSERT_TEXT;
    insert_text_data_init(&instruction->insertTextData, data, len);
}

void templator_insert_variable_instruction_init(TemplatorInstruction* instruction, size_t nameIndex) {
    instruction->type = TEMPLATOR_INSTRUCTION_TYPE_INSERT_VARIABLE_VALUE;
    insert_variable_data_init(&instruction->insertVariableData, nameIndex);
}

void templator_insert_conditional_subtemplate(TemplatorInstruction* instruction, Template subTemplate, ComparisonChain comp) {
    instruction->type = TEMPLATOR_INSTRUCTION_TYPE_INSERT_CONDITIONAL_SUBTEMPLATE;
    conditional_insert_text_data_init(&instruction->conditionalInsertTextData, subTemplate, comp);
}

void templator_instruction_free(TemplatorInstruction* instruction) {
    switch(instruction->type) {
        case TEMPLATOR_INSTRUCTION_TYPE_INSERT_TEXT:
            insert_text_data_free(&instruction->insertTextData);
            break;
        case TEMPLATOR_INSTRUCTION_TYPE_INSERT_VARIABLE_VALUE:
            insert_variable_data_free(&instruction->insertVariableData);
            break;
        case TEMPLATOR_INSTRUCTION_TYPE_INSERT_CONDITIONAL_SUBTEMPLATE:
            conditional_insert_text_data_free(&instruction->conditionalInsertTextData);
            break;
        case TEMPLATOR_INSTRUCTION_TYPE_NOOP:
            break;
    }
}
