#ifndef TEMPLATOR_INSERT_VARIABLE_INSTRUCTION_H
#define TEMPLATOR_INSERT_VARIABLE_INSTRUCTION_H

#include <stdlib.h>

typedef struct {
    size_t nameIndex;
} InsertVariableData;

void insert_variable_data_init(InsertVariableData* itd, size_t nameIndex);
void insert_variable_data_free(InsertVariableData* itd);

#endif//TEMPLATOR_INSERT_VARIABLE_INSTRUCTION_H
