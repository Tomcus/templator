#include "templator/variables.h"

#include <string.h>

#define VARIABLES_INITIAL_SIZE 10

void templator_variables_init(TemplatorVariables* variables) {
    variables->data = malloc(sizeof(TemplatorVariable) * VARIABLES_INITIAL_SIZE);
    variables->cap = VARIABLES_INITIAL_SIZE;
    variables->len = 0;
}

void templator_variables_free(TemplatorVariables* variables) {
    free(variables->data);
}

#define VARIABLE_FREE(var) \
    if (var->type == TEMPLATOR_VARIABLE_TYPE_CSTR_OWN && var->s.data != NULL) { \
        free(var->s.data); \
    } \

void variables_resize(TemplatorVariables* variables) {
    size_t newSize = variables->cap * 2;
    variables->data = realloc(variables->data, sizeof(TemplatorVariable) * newSize);
    variables->cap = newSize;
}

#define CHECK_RESIZE_VARIABLES              \
    if (variables->len == variables->cap) { \
        variables_resize(variables);        \
    }

void templator_variables_set_int_variable(TemplatorVariables* variables, const char* name, intmax_t value) {
    TemplatorVariable* var = templator_variables_get_variable(variables, name);
    if (var == NULL) {
        CHECK_RESIZE_VARIABLES;
        var = &variables->data[variables->len];
        var->name = name;
        var->type = TEMPLATOR_VARIABLE_TYPE_INT;
        var->i = value;
        variables->len++;
    } else {
        VARIABLE_FREE(var);
        var->type = TEMPLATOR_VARIABLE_TYPE_INT;
        var->i = value;
    }
}

void templator_variables_set_uint_variable(TemplatorVariables* variables, const char* name, uintmax_t value) {
    TemplatorVariable* var = templator_variables_get_variable(variables, name);
    if (var == NULL) {
        CHECK_RESIZE_VARIABLES;
        var = &variables->data[variables->len];
        var->name = name;
        var->type = TEMPLATOR_VARIABLE_TYPE_UINT;
        var->u = value;
        variables->len++;
    } else {
        VARIABLE_FREE(var);
        var->type = TEMPLATOR_VARIABLE_TYPE_UINT;
        var->u = value;
    }
}

#define STR_ASSIGN(dst, src, length) \
    size_t strLen = length; \
    if (strLen == TEMPLATOR_STR_NUL_TERMINATED) { \
        strLen = strlen(src); \
    } \
    STR_CPY(dst, src, strLen); \
    dst.len = strLen;

#define STR_CPY(dst, src, len) \
    dst.data = src;

void templator_variables_set_str_variable(TemplatorVariables* variables, const char* name, char* value, size_t valueLen) {
    TemplatorVariable* var = templator_variables_get_variable(variables, name);
    if (var == NULL) {
        CHECK_RESIZE_VARIABLES;
        var = &variables->data[variables->len];
        var->name = name;
        var->type = TEMPLATOR_VARIABLE_TYPE_CSTR_REF;
        STR_ASSIGN(var->s, value, valueLen);
        variables->len++;
    } else {
        VARIABLE_FREE(var);
        var->type = TEMPLATOR_VARIABLE_TYPE_CSTR_REF;
        STR_ASSIGN(var->s, value, valueLen);
    }
}

#undef STR_CPY

#define STR_CPY(dst, src, len) \
    dst.data = malloc(len + 1); \
    strncpy(dst.data, src, len); \
    dst.data[len] = 0;

void templator_variables_set_cpy_str_variable(TemplatorVariables* variables, const char* name, char* value, size_t valueLen) {
    TemplatorVariable* var = templator_variables_get_variable(variables, name);
    if (var == NULL) {
        CHECK_RESIZE_VARIABLES;
        var = &variables->data[variables->len];
        var->name = name;
        var->type = TEMPLATOR_VARIABLE_TYPE_CSTR_OWN;
        STR_ASSIGN(var->s, value, valueLen);
        variables->len++;
    } else {
        VARIABLE_FREE(var);
        var->type = TEMPLATOR_VARIABLE_TYPE_CSTR_OWN;
        STR_ASSIGN(var->s, value, valueLen);
    }
}

void templator_variables_clear_variables(TemplatorVariables* variables) {
    for (size_t i = 0; i < variables->len; ++i) {
        TemplatorVariable* var = &variables->data[i];
        VARIABLE_FREE(var);
    }
    variables->len = 0;
}

void templator_variables_remove_variable(TemplatorVariables* variables, const char* name) {
    size_t i = 0;
    for (; i < variables->len; ++i) {
        TemplatorVariable* var = &variables->data[i];
        if (strcmp(name, var->name) == 0) {
            VARIABLE_FREE(var);
            variables->len -= 1;
            break;
        }
    }
    for(; i < variables->len; ++i) {
        variables->data[i] = variables->data[i + 1];
    }
}

TemplatorVariable* templator_variables_get_variable(TemplatorVariables* variables, const char* name) {
    for (size_t i = 0; i < variables->len; ++i) {
        TemplatorVariable* var = &variables->data[i];
        if (strcmp(var->name, name) == 0) {
            return var;
        }
    }
    return NULL;
}
