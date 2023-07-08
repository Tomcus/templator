#include "templator/variable_comparisons.h"
#include "templator/error.h"

#include <string.h>

int variables_cmp_int_int(Variable* a, Variable* b) {
    intmax_t diff = a->i - b->i;
    if (diff == 0) {
        return TEMPLATOR_COMPARISON_RESULT_EQ;
    } else if (diff < 0) {
        return TEMPLATOR_COMPARISON_RESULT_LT;
    } else {
        return TEMPLATOR_COMPARISON_RESULT_GT;
    }
}

int variables_cmp_str_str(Variable* a, Variable* b) {
    int res = strncmp(a->s.data, b->s.data, (a->s.len > b->s.len) ? a->s.len: b->s.len);
    if (res == 0) {
        return TEMPLATOR_COMPARISON_RESULT_EQ;
    } else if (res < 0) {
        return TEMPLATOR_COMPARISON_RESULT_LT;
    } else {
        return TEMPLATOR_COMPARISON_RESULT_GT;
    }
}

int unknown_cmp(Variable*, Variable*) {
    return TEMPLATOR_UNSUPORTED_COMPARISON;
}
