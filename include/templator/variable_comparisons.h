#ifndef TEMPLATOR_VARIABLE_COMPARISONS_H
#define TEMPLATOR_VARIABLE_COMPARISONS_H

#include "logical_operations.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int(* TemplatorCmpVariables)(TemplatorVariable*, TemplatorVariable*);

int templator_cmp_int_int_variables(TemplatorVariable* a, TemplatorVariable* b);
int templator_cmp_str_str_variables(TemplatorVariable* a, TemplatorVariable* b);
int templator_unknown_cmp(TemplatorVariable* a, TemplatorVariable* b);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_VARIABLE_COMPARISONS_H
