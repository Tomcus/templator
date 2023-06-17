#ifndef TEMPLATOR_INSERT_TEXT_INSTRUCTION_H
#define TEMPLATOR_INSERT_TEXT_INSTRUCTION_H

#include <stdlib.h>

typedef struct {
    char* data;
    size_t len;
} InsertTextData;

void insert_text_data_init(InsertTextData* itd, char* data, size_t len);
void insert_text_data_free(InsertTextData* itd);

#endif//TEMPLATOR_INSERT_TEXT_INSTRUCTION_H
