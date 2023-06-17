#ifndef TEMPLATOR_VARIABLES_H
#define TEMPLATOR_VARIABLES_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "definitions.h"

typedef enum {
    VARIABLE_INT,
    VARIABLE_UINT,
    VARIABLE_CSTR_REF,
    VARIABLE_CSTR_OWN
} VARIABLE_TYPE;

typedef struct {
    const char* name;
    union {
        intmax_t i;
        uintmax_t u;
        struct {
            char* data;
            size_t len;
        } s;
    };
    VARIABLE_TYPE type;
} Variable;

typedef struct {
    Variable* data;
    size_t cap;
    size_t len;
} Variables; 

void variables_init(Variables* variables);
void variables_free(Variables* variables);

void variables_set_str_variable(Variables* variables, const char* name, char* value, size_t valueLen);
void variables_set_cpy_str_variable(Variables* variables, const char* name, char* value, size_t valueLen);
void variables_set_uint_variable(Variables* variables, const char* name, uintmax_t value);
void variables_set_int_variable(Variables* variables, const char* name, intmax_t value);

void variables_clear_variables(Variables* variables);
void variables_remove_variable(Variables* variables, const char* name);

Variable* variables_get_variable(Variables* variables, const char* name);

#endif//TEMPLATOR_VARIABLES_H
