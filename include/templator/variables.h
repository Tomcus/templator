#ifndef TEMPLATOR_VARIABLES_H
#define TEMPLATOR_VARIABLES_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TEMPLATOR_VARIABLE_TYPE_INT = 0,
    TEMPLATOR_VARIABLE_TYPE_UINT = 1,
    TEMPLATOR_VARIABLE_TYPE_CSTR_REF = 2,
    TEMPLATOR_VARIABLE_TYPE_CSTR_OWN = 3
} TEMPLATOR_VARIABLE_TYPE;

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
    TEMPLATOR_VARIABLE_TYPE type;
} TemplatorVariable;

typedef struct {
    TemplatorVariable* data;
    size_t cap;
    size_t len;
} TemplatorVariables; 

void templator_variables_init(TemplatorVariables* variables);
void templator_variables_free(TemplatorVariables* variables);

void templator_variables_set_str_variable(TemplatorVariables* variables, const char* name, char* value, size_t valueLen);
void templator_variables_set_cpy_str_variable(TemplatorVariables* variables, const char* name, char* value, size_t valueLen);
void templator_variables_set_uint_variable(TemplatorVariables* variables, const char* name, uintmax_t value);
void templator_variables_set_int_variable(TemplatorVariables* variables, const char* name, intmax_t value);

void templator_variables_clear_variables(TemplatorVariables* variables);
void templator_variables_remove_variable(TemplatorVariables* variables, const char* name);

TemplatorVariable* templator_variables_get_variable(TemplatorVariables* variables, const char* name);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_VARIABLES_H
