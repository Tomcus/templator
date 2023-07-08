#ifndef TEMPLATOR_VARIABLE_COMPARISONS_H
#define TEMPLATOR_VARIABLE_COMPARISONS_H

#include "logical_operations.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int(* TemplatorCmpVariables)(Variable*, Variable*);

int templator_cmp_int_int_variables(Variable* a, Variable* b);
int templator_cmp_str_str_variables(Variable* a, Variable* b);
int templator_unknown_cmp(Variable* a, Variable* b);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_VARIABLE_COMPARISONS_H
