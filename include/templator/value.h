#ifndef TEMPLATOR_VALUE_H
#define TEMPLATOR_VALUE_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TEMPLATOR_VALUE_TYPE_INT = 0,
    TEMPLATOR_VALUE_TYPE_UINT = 1,
    TEMPLATOR_VALUE_TYPE_CSTR_REF = 2,
    TEMPLATOR_VALUE_TYPE_CSTR_OWN = 3
} TEMPLATOR_VALUE_TYPE;

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
    TEMPLATOR_VALUE_TYPE type;
} TemplatorValue;


#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_VALUE_H
