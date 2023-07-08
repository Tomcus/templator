#ifndef TEMPLATOR_VARIABLE_COMPARISONS_H
#define TEMPLATOR_VARIABLE_COMPARISONS_H

#include "logical_operations.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int(* cmpFunction)(Variable*, Variable*);

int variables_cmp_int_int(Variable* a, Variable* b);
int variables_cmp_str_str(Variable* a, Variable* b);
int unknown_cmp(Variable* a, Variable* b);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_VARIABLE_COMPARISONS_H
