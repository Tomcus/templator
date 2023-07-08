#ifndef TEMPLATOR_VARIABLES_H
#define TEMPLATOR_VARIABLES_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "definitions.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    TemplatorValue* data;
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

TemplatorValue* templator_variables_get_variable(TemplatorVariables* variables, const char* name);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_VARIABLES_H
