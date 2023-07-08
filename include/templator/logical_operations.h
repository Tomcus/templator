#ifndef TEMPLATOR_LOGICAL_OPERATIONS
#define TEMPLATOR_LOGICAL_OPERATIONS

#include "variables.h"
#include "parser.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct TTemplate;

typedef enum {
    TEMPLATOR_SIDE_COMPARISON_TYPE_LOCAL_VARIABLE,
    TEMPLATOR_SIDE_COMPARISON_TYPE_VARIABLE_REFERENCE,
    TEMPLATOR_SIDE_COMPARISON_TYPE_COMPARISON_CHAIN
} TEMPLATOR_SIDE_COMPARISON_TYPE;

typedef enum {
    TEMPLATOR_COMPARISON_RESULT_EQ = 0,
    TEMPLATOR_COMPARISON_RESULT_LT = -1,
    TEMPLATOR_COMPARISON_RESULT_GT = 1
} TEMPLATOR_COMPARISON_RESULT;

typedef enum {
    TEMPLATOR_COMPARISON_OPERATOR_EQ,
    TEMPLATOR_COMPARISON_OPERATOR_NE,
    TEMPLATOR_COMPARISON_OPERATOR_GT,
    TEMPLATOR_COMPARISON_OPERATOR_GE,
    TEMPLATOR_COMPARISON_OPERATOR_LT,
    TEMPLATOR_COMPARISON_OPERATOR_LE,
    TEMPLATOR_COMPARISON_OPERATOR_VARIABLE_EVAL,
    TEMPLATOR_COMPARISON_OPERATOR_CHAIN_EVAL,
} TEMPLATOR_COMPARISON_OPERATOR;

typedef enum {
    TEMPLATOR_CHAIN_OPERATOR_AND,
    TEMPLATOR_CHAIN_OPERATOR_OR,
} TEMPLATOR_CHAIN_OPERATOR;

struct TTemplatorComparisonChain;

typedef union {
    size_t variableIndex;
    Variable local;
    struct TTemplatorComparisonChain* chain;
} TemplatorComparisonData;

typedef struct {
    TemplatorComparisonData lhs;
    TEMPLATOR_SIDE_COMPARISON_TYPE lhsType;
    TemplatorComparisonData rhs;
    TEMPLATOR_SIDE_COMPARISON_TYPE rhsType;
    TEMPLATOR_COMPARISON_OPERATOR op;
} TemplatorComparison;

typedef struct TTemplatorComparisonChain {
    TemplatorComparison cmp;
    TEMPLATOR_CHAIN_OPERATOR op;
    struct TTemplatorComparisonChain* next;
} TemplatorComparisonChain;

bool templator_is_variable_truthy(Variable* var);

bool templator_is_comparison_result_ok(TEMPLATOR_COMPARISON_OPERATOR op, TEMPLATOR_COMPARISON_RESULT res);
int templator_compare_variables(Variable* a, Variable* b);

int templator_comparison_chain_parse(TemplatorComparisonChain* compChain, TemplatorParser* parser, struct TTemplate* templ);
int templator_comparison_parse(TemplatorComparison* comparison, TemplatorParser* parser, struct TTemplate* templ);
int templator_comparison_chain_validate_operator(char* data, size_t len);
int templator_comparison_chain_eval(TemplatorComparisonChain* compChain, struct TTemplate* templ, Variables* variables);
int templator_comparison_operator_validate(char* data, size_t len);
Variable* templator_comparison_get_lhs(TemplatorComparison* comparison, struct TTemplate* templ, Variables* variables);
Variable* templator_comparison_get_rhs(TemplatorComparison* comparison, struct TTemplate* templ, Variables* variables);

int templator_comparison_eval(TemplatorComparison* comparison, struct TTemplate* templ, Variables* variables);
void templator_comparison_chain_free(TemplatorComparisonChain* compChain);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_LOGICAL_OPERATIONS
