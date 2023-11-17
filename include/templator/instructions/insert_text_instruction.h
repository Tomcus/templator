#ifndef TEMPLATOR_INSERT_TEXT_INSTRUCTION_H
#define TEMPLATOR_INSERT_TEXT_INSTRUCTION_H

#include <stdlib.h>

typedef struct {
    char* data;
    size_t len;
} TemplatorInsertTextData;

void templator_insert_text_data_init(TemplatorInsertTextData* itd, char* data, size_t len);
void templator_insert_text_data_free(TemplatorInsertTextData* itd);

#endif // TEMPLATOR_INSERT_TEXT_INSTRUCTION_H
