#include "templator/instructions/insert_variable_instruction.h"

void templator_insert_variable_data_init(TemplatorInsertVariableData* itd, size_t nameIndex) {
    itd->nameIndex = nameIndex;
}

void templator_insert_variable_data_free(TemplatorInsertVariableData* itd) {
    (void)itd;
    // NOOP
}
