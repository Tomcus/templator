#include "templator/command_tokenizer.h"
#include "templator/error.h"
#include "templator/logical_operations.h"
#include "templator/template.h"
#include "templator/variable_comparisons.h"

#include <inttypes.h>
#include <string.h>
#include <stdio.h>

bool templator_is_value_truthy(TemplatorValue* var) {
    if (var == NULL) {
        return false;
    }

    switch(var->type) {
        case TEMPLATOR_VALUE_TYPE_INT:
            return var->i != 0; 
        case TEMPLATOR_VALUE_TYPE_UINT:
            return var->u > 0;
        case TEMPLATOR_VALUE_TYPE_CSTR_OWN:
        case TEMPLATOR_VALUE_TYPE_CSTR_REF:
            return var->s.len > 0;
    }
    return false;
}

bool templator_is_comparison_result_ok(TEMPLATOR_COMPARISON_OPERATOR op, TEMPLATOR_COMPARISON_RESULT res) {
    switch(res) {
        case TEMPLATOR_COMPARISON_RESULT_EQ:
            return op == TEMPLATOR_COMPARISON_OPERATOR_EQ || op == TEMPLATOR_COMPARISON_OPERATOR_GE || op == TEMPLATOR_COMPARISON_OPERATOR_LE;
        case TEMPLATOR_COMPARISON_RESULT_GT:
            return op == TEMPLATOR_COMPARISON_OPERATOR_NE || op == TEMPLATOR_COMPARISON_OPERATOR_GE || op == TEMPLATOR_COMPARISON_OPERATOR_GT;
        case TEMPLATOR_COMPARISON_RESULT_LT:
            return op == TEMPLATOR_COMPARISON_OPERATOR_NE || op == TEMPLATOR_COMPARISON_OPERATOR_LE || op == TEMPLATOR_COMPARISON_OPERATOR_LT;
    }
    return false;
}

const TemplatorCmpValues VAR_CMPS[4][4] = {
    {templator_cmp_int_int_values, templator_cmp_int_int_values, templator_unknown_cmp, templator_unknown_cmp},
    {templator_cmp_int_int_values, templator_cmp_int_int_values, templator_unknown_cmp, templator_unknown_cmp},
    {templator_unknown_cmp, templator_unknown_cmp, templator_cmp_str_str_values, templator_cmp_str_str_values},
    {templator_unknown_cmp, templator_unknown_cmp, templator_cmp_str_str_values, templator_cmp_str_str_values},
}; 


int templator_compare_values(TemplatorValue* a, TemplatorValue* b) {
    TemplatorCmpValues func = VAR_CMPS[a->type][b->type];
    return func(a, b);
}

int templator_comparison_parse(TemplatorComparison* comparison, TemplatorParser* parser, TemplatorTemplate* templ) {
    Token lhsToken = templator_parser_next_token(parser);
    switch(lhsToken.type) {
        case PAREN_OPEN: {
            comparison->lhs.chain = malloc(sizeof(TemplatorComparisonChain));
            comparison->lhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_COMPARISON_CHAIN;
            int res = templator_comparison_chain_parse(comparison->lhs.chain, parser, templ);
            if (res != 0) {
                return res;
            }
            comparison->op = TEMPLATOR_COMPARISON_OPERATOR_CHAIN_EVAL;
            return 0;
        }
        case WORD: {
            comparison->lhs.variableIndex = templator_template_try_insert_variable(templ, lhsToken.data, lhsToken.len);
            comparison->lhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_VARIABLE_REFERENCE;
        }
        break;
        case NUMB: {
            char buffer[8*sizeof(intmax_t)];
            strncpy(buffer, lhsToken.data, lhsToken.len);
            buffer[lhsToken.len] = 0;
            intmax_t val;
            sscanf(buffer, "%"SCNdMAX, &val);
            comparison->lhs.local.type = TEMPLATOR_VALUE_TYPE_INT;
            comparison->lhs.local.i = val;
            comparison->lhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_LOCAL_VALUE;
        }
        break;
        default:
            return TEMPLATOR_UNEXPECTED_TOKEN;
    }

    Token opToken = templator_parser_peek_token(*parser);
    switch (opToken.type) {
        case PAREN_CLOSE:
            comparison->op = TEMPLATOR_COMPARISON_OPERATOR_VARIABLE_EVAL;
            return 0;
        case OPERATOR: {
            int res = templator_comparison_chain_validate_operator(opToken.data, opToken.len);
            if (res < 0) {
                if (templator_comparison_operator_validate(opToken.data, opToken.len) >= 0) {
                    comparison->op = TEMPLATOR_COMPARISON_OPERATOR_VARIABLE_EVAL;
                    return 0;
                }
                return res;
            }
            templator_parser_next_token(parser);
            comparison->op = (TEMPLATOR_COMPARISON_OPERATOR)res;
        }
        break;
        default:
            return TEMPLATOR_UNEXPECTED_TOKEN;
    }

    Token rhsToken = templator_parser_next_token(parser);
    switch (rhsToken.type) {
        case WORD: {
            comparison->rhs.variableIndex = templator_template_try_insert_variable(templ, rhsToken.data, rhsToken.len);
            comparison->rhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_VARIABLE_REFERENCE;
        }
        break;
        case NUMB: {
            char buffer[8*sizeof(intmax_t)];
            strncpy(buffer, rhsToken.data, rhsToken.len);
            buffer[rhsToken.len] = 0;
            intmax_t val;
            sscanf(buffer, "%"SCNdMAX, &val);
            comparison->rhs.local.type = TEMPLATOR_VALUE_TYPE_INT;
            comparison->rhs.local.i = val;
            comparison->rhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_LOCAL_VALUE;
        }
        break;
        default:
            return TEMPLATOR_UNEXPECTED_TOKEN;
    }
    return 0;
}

int templator_comparison_chain_parse(TemplatorComparisonChain* compChain, TemplatorParser* parser, TemplatorTemplate* templ) {
    compChain->next = NULL;
    TemplatorComparisonChain* curr = compChain;

    while (true) {
        int res = templator_comparison_parse(&curr->cmp, parser, templ);
        bool killLoop = false;

        if (res < 0) {
            return res;
        }

        Token opToken = templator_parser_next_token(parser);
        switch (opToken.type) {
            case PAREN_CLOSE:
                killLoop = true;
                break;
            case OPERATOR: {
                int res = templator_comparison_operator_validate(opToken.data, opToken.len);
                if (res < 0) {
                    return res;
                }

                if (curr == compChain) {
                    compChain->op = (TEMPLATOR_CHAIN_OPERATOR)res;
                } else if (compChain->op != (TEMPLATOR_CHAIN_OPERATOR)res) {
                    return TEMPLATOR_CHAIN_OPERATOR_CHANGED;
                } else {
                    curr->op = (TEMPLATOR_CHAIN_OPERATOR)res;
                }

                curr->next = malloc(sizeof(TemplatorComparisonChain));
                curr = curr->next;
                curr->next = NULL;
            }
            break;
            default:
                return TEMPLATOR_UNEXPECTED_TOKEN;
        }

        if (killLoop) {
            break;
        }
    }
    return 0;
}

int templator_comparison_chain_eval(TemplatorComparisonChain* compChain, TemplatorTemplate* templ, TemplatorVariables* variables) {
    if (compChain->next == NULL) {
        return templator_comparison_eval(&compChain->cmp, templ, variables);
    }
    const TEMPLATOR_CHAIN_OPERATOR op = compChain->op;
    TemplatorComparisonChain* curr = compChain;
    int res;

    while (curr != NULL) {
        res = templator_comparison_eval(&curr->cmp, templ, variables);
        if (res < 0) {
            return res;
        }

        if (op == TEMPLATOR_CHAIN_OPERATOR_OR && res) {
            return 1;
        } else if (op == TEMPLATOR_CHAIN_OPERATOR_AND && !res) {
            return 0;
        }

        curr = curr->next;
    }

    switch (op) {
        case TEMPLATOR_CHAIN_OPERATOR_AND:
            return 1;
        case TEMPLATOR_CHAIN_OPERATOR_OR:
            return 0;
    }
    return 0;
}

int templator_comparison_eval(TemplatorComparison* comparison, TemplatorTemplate* templ, TemplatorVariables* variables) {
    TemplatorValue* lhs;
    TemplatorValue* rhs;
    switch (comparison->op) {
        case TEMPLATOR_COMPARISON_OPERATOR_CHAIN_EVAL:
            return templator_comparison_chain_eval(comparison->lhs.chain, templ, variables);
        case TEMPLATOR_COMPARISON_OPERATOR_VARIABLE_EVAL:
            lhs = templator_comparison_get_lhs(comparison, templ, variables);
            if (lhs == NULL) {
                return TEMPLATOR_VARIABLE_NOT_SET;
            }
            return templator_is_value_truthy(lhs);
        default:
            lhs = templator_comparison_get_lhs(comparison, templ, variables);
            if (lhs == NULL) {
                return TEMPLATOR_VARIABLE_NOT_SET;
            }
            rhs = templator_comparison_get_rhs(comparison, templ, variables);
            if (rhs == NULL) {
                return TEMPLATOR_VARIABLE_NOT_SET;
            }

            int res = templator_compare_values(lhs, rhs);
            if (res >= -1 && res <= 1) {
                return templator_is_comparison_result_ok(comparison->op, res);
            }
            return res;
    }
}

void templator_comparison_chain_free(TemplatorComparisonChain* compChain) {
    if (compChain != NULL && compChain->next != NULL) {
        templator_comparison_chain_free(compChain->next);
        free(compChain->next);
        compChain->next = NULL;
    }
}

int templator_comparison_chain_validate_operator(char* data, size_t len) {
    switch (len) {
        case 1:
            switch (data[0]) {
                case '>':
                    return TEMPLATOR_COMPARISON_OPERATOR_GT;
                case '<':
                    return TEMPLATOR_COMPARISON_OPERATOR_LT;
                default:
                    return TEMPLATOR_INVALID_COMPARISON_OPERATOR;
            }
        case 2:
            if (data[1] != '=') {
                return TEMPLATOR_INVALID_COMPARISON_OPERATOR;
            }
            switch (data[0]) {
                case '>':
                    return TEMPLATOR_COMPARISON_OPERATOR_GE;
                case '<':
                    return TEMPLATOR_COMPARISON_OPERATOR_LE;
                case '!':
                    return TEMPLATOR_COMPARISON_OPERATOR_NE;
                case '=':
                    return TEMPLATOR_COMPARISON_OPERATOR_EQ;
                default:
                    return TEMPLATOR_INVALID_COMPARISON_OPERATOR;
            }
    }
    return 0;
}

TemplatorValue* templator_comparison_get_lhs(TemplatorComparison* comparison, TemplatorTemplate* templ, TemplatorVariables* variables) {
    switch (comparison->lhsType) {
        case TEMPLATOR_SIDE_COMPARISON_TYPE_LOCAL_VALUE:
            return &comparison->lhs.local;
        case TEMPLATOR_SIDE_COMPARISON_TYPE_VARIABLE_REFERENCE: {
            char * variableName = templ->variables[comparison->lhs.variableIndex];
            return templator_variables_get_variable(variables, variableName);
        }
        case TEMPLATOR_SIDE_COMPARISON_TYPE_COMPARISON_CHAIN:
            return NULL;
    }
    return NULL;
}

TemplatorValue* templator_comparison_get_rhs(TemplatorComparison* comparison, TemplatorTemplate* templ, TemplatorVariables* variables) {
    switch (comparison->rhsType) {
        case TEMPLATOR_SIDE_COMPARISON_TYPE_LOCAL_VALUE:
            return &comparison->rhs.local;
        case TEMPLATOR_SIDE_COMPARISON_TYPE_VARIABLE_REFERENCE: {
            char * variableName = templ->variables[comparison->rhs.variableIndex];
            return templator_variables_get_variable(variables, variableName);
        }
        case TEMPLATOR_SIDE_COMPARISON_TYPE_COMPARISON_CHAIN:
            return NULL;
    }
    return NULL;
}

int templator_comparison_operator_validate(char* data, size_t len) {
    if (len != 2 || data[0] != data[1] || (data[0] != '&' && data[0] != '|')) {
        return TEMPLATOR_INVALID_CHAIN_OPERATOR;
    }

    switch(data[0]) {
        case '&':
            return TEMPLATOR_CHAIN_OPERATOR_AND;
        case '|':
            return TEMPLATOR_CHAIN_OPERATOR_OR;
        default:
            return TEMPLATOR_INVALID_CHAIN_OPERATOR;
    }

    return TEMPLATOR_INVALID_CHAIN_OPERATOR;
}
