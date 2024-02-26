#include "templator/variable_comparisons.h"
#include "templator/error.h"

#include <string.h>

int templator_cmp_int_int_values(TemplatorValue* lhs, TemplatorValue* rhs) {
    intmax_t diff = lhs->i - rhs->i;
    if (diff == 0) {
        return TEMPLATOR_COMPARISON_RESULT_EQ;
    }
    if (diff < 0) {
        return TEMPLATOR_COMPARISON_RESULT_LT;
    }
    return TEMPLATOR_COMPARISON_RESULT_GT;
}

int templator_cmp_str_str_values(TemplatorValue* lhs, TemplatorValue* rhs) {
    int res = strncmp(lhs->s.data, rhs->s.data, (lhs->s.len > rhs->s.len) ? lhs->s.len : rhs->s.len);
    if (res == 0) {
        return TEMPLATOR_COMPARISON_RESULT_EQ;
    if (res < 0) {
        return TEMPLATOR_COMPARISON_RESULT_LT;
    }
    return TEMPLATOR_COMPARISON_RESULT_GT;
}

int templator_unknown_cmp(TemplatorValue* /*lhs*/, TemplatorValue* /*rhs*/) {
    return TEMPLATOR_UNSUPORTED_COMPARISON;
}
