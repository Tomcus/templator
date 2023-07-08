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
    LOCAL_VARIABLE,
    VARIABLE_REFERENCE,
    COMPARISON_CHAIN
} COMPARISON_TYPE;

typedef enum {
    EQ = 0,
    LT = -1,
    GT = 1
} COMPARISON_RESULT;

typedef enum {
    CMP_EQ,
    CMP_NE,
    CMP_GT,
    CMP_GE,
    CMP_LT,
    CMP_LE,
    CMP_VARIABLE_EVAL,
    CMP_CHAIN_EVAL
} COMPARISON_OPERATOR;

typedef enum {
    TEMPLATOR_CMP_AND,
    TEMPLATOR_CMP_OR
} CHAIN_OPERATOR;

struct TComparisonChain;

typedef union {
    size_t variableIndex;
    Variable local;
    struct TComparisonChain* chain;
} ComparisonData;

typedef struct {
    ComparisonData lhs;
    COMPARISON_TYPE lhsType;
    ComparisonData rhs;
    COMPARISON_TYPE rhsType;
    COMPARISON_OPERATOR op;
} Comparison;

typedef struct TComparisonChain {
    Comparison cmp;
    CHAIN_OPERATOR op;
    struct TComparisonChain* next;
} ComparisonChain;

bool is_variable_truthy(Variable* var);

bool is_comparison_result_ok(COMPARISON_OPERATOR op, COMPARISON_RESULT res);
int variables_compare(Variable* a, Variable* b);

int comparison_chain_parse(ComparisonChain* compChain, Parser* parser, struct TTemplate* templ);
int templator_comparison_parse(Comparison* comparison, Parser* parser, struct TTemplate* templ);
int comparison_chain_validate_operator(char* data, size_t len);
int comparison_chain_eval(ComparisonChain* compChain, struct TTemplate* templ, Variables* variables);
int templator_comparison_operator_validate(char* data, size_t len);
Variable* templator_comparison_get_lhs(Comparison* comparison, struct TTemplate* templ, Variables* variables);
Variable* templator_comparison_get_rhs(Comparison* comparison, struct TTemplate* templ, Variables* variables);

int templator_comparison_eval(Comparison* comparison, struct TTemplate* templ, Variables* variables);
void comparison_chain_free(ComparisonChain* compChain);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_LOGICAL_OPERATIONS
