#ifndef TEMPLATOR_INSERT_VARIABLE_INSTRUCTION_H
#define TEMPLATOR_INSERT_VARIABLE_INSTRUCTION_H

#include <stdlib.h>

typedef struct {
    size_t nameIndex;
} TemplatorInsertVariableData;

void templator_insert_variable_data_init(TemplatorInsertVariableData* itd, size_t nameIndex);
void templator_insert_variable_data_free(TemplatorInsertVariableData* itd);

#endif // TEMPLATOR_INSERT_VARIABLE_INSTRUCTION_H
