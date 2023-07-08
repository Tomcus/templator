#include "templator/variable_comparisons.h"
#include "templator/error.h"

#include <string.h>

int templator_cmp_int_int_variables(TemplatorVariable* a, TemplatorVariable* b) {
    intmax_t diff = a->i - b->i;
    if (diff == 0) {
        return TEMPLATOR_COMPARISON_RESULT_EQ;
    } else if (diff < 0) {
        return TEMPLATOR_COMPARISON_RESULT_LT;
    } else {
        return TEMPLATOR_COMPARISON_RESULT_GT;
    }
}

int templator_cmp_str_str_variables(TemplatorVariable* a, TemplatorVariable* b) {
    int res = strncmp(a->s.data, b->s.data, (a->s.len > b->s.len) ? a->s.len: b->s.len);
    if (res == 0) {
        return TEMPLATOR_COMPARISON_RESULT_EQ;
    } else if (res < 0) {
        return TEMPLATOR_COMPARISON_RESULT_LT;
    } else {
        return TEMPLATOR_COMPARISON_RESULT_GT;
    }
}

int templator_unknown_cmp(TemplatorVariable*, TemplatorVariable*) {
    return TEMPLATOR_UNSUPORTED_COMPARISON;
}
