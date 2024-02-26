#ifndef TEMPLATOR_VARIABLE_COMPARISONS_H
#define TEMPLATOR_VARIABLE_COMPARISONS_H

#include "logical_operations.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*TemplatorCmpValues)(TemplatorValue*, TemplatorValue*);

int templator_cmp_int_int_values(TemplatorValue* lhs, TemplatorValue* rhs);
int templator_cmp_str_str_values(TemplatorValue* lhs, TemplatorValue* rhs);
int templator_unknown_cmp(TemplatorValue* lhs, TemplatorValue* rhs);

#ifdef __cplusplus
}
#endif

#endif // TEMPLATOR_VARIABLE_COMPARISONS_H
