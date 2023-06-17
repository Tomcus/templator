#include "templator/instructions/insert_variable_instruction.h"

void insert_variable_data_init(InsertVariableData* itd, size_t nameIndex) {
    itd->nameIndex = nameIndex;
}

void insert_variable_data_free(InsertVariableData* itd) {
    (void)itd;
    // NOOP
}
