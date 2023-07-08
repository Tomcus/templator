#ifndef TEMPLATOR_VALUE_H
#define TEMPLATOR_VALUE_H

#include <stdlib.h>
#include <stdint.h>

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


#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_VALUE_H
