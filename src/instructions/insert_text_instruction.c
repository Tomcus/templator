#include "templator/instructions/insert_text_instruction.h"

#include <string.h>

void templator_insert_text_data_init(TemplatorInsertTextData* itd, char* data, size_t len) {
    itd->data = malloc(len + 1);
    strncpy(itd->data, data, len);
    itd->data[len] = 0;
    itd->len = len;
}

void templator_insert_text_data_free(TemplatorInsertTextData* itd) {
    free(itd->data);
}
