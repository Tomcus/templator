#include "templator/instructions/insert_text_instruction.h"

#include <string.h>

void insert_text_data_init(InsertTextData* itd, char* data, size_t len) {
    itd->data = malloc(len + 1);
    strncpy(itd->data, data, len);
    itd->data[len] = 0;
    itd->len = len;
}

void insert_text_data_free(InsertTextData* itd) {
    free(itd->data);
}
