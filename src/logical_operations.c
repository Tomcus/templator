#include "templator/logical_operations.h"
#include "templator/command_tokenizer.h"
#include "templator/error.h"
#include "templator/template.h"
#include "templator/variable_comparisons.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

bool templator_is_value_truthy(TemplatorValue* var) {
    if (var == NULL) {
        return false;
    }

    switch (var->type) {
        case TEMPLATOR_VALUE_TYPE_INT:
            return (bool)(var->i != 0);
        case TEMPLATOR_VALUE_TYPE_UINT:
            return (bool)(var->u > 0);
        case TEMPLATOR_VALUE_TYPE_CSTR_OWN:
        case TEMPLATOR_VALUE_TYPE_CSTR_REF:
            return (bool)(var->s.len > 0);
    }
    return false;
}

bool templator_is_comparison_result_ok(TEMPLATOR_COMPARISON_OPERATOR operator, TEMPLATOR_COMPARISON_RESULT res) {
    switch (res) {
        case TEMPLATOR_COMPARISON_RESULT_EQ:
            return (bool)(operator == TEMPLATOR_COMPARISON_OPERATOR_EQ || operator == TEMPLATOR_COMPARISON_OPERATOR_GE || operator == TEMPLATOR_COMPARISON_OPERATOR_LE);
        case TEMPLATOR_COMPARISON_RESULT_GT:
            return (bool)(operator == TEMPLATOR_COMPARISON_OPERATOR_NE || operator == TEMPLATOR_COMPARISON_OPERATOR_GE || operator == TEMPLATOR_COMPARISON_OPERATOR_GT);
        case TEMPLATOR_COMPARISON_RESULT_LT:
            return (bool)(operator == TEMPLATOR_COMPARISON_OPERATOR_NE || operator == TEMPLATOR_COMPARISON_OPERATOR_LE || operator == TEMPLATOR_COMPARISON_OPERATOR_LT);
    }
    return false;
}

const TemplatorCmpValues VAR_CMPS[4][4] = {
    { templator_cmp_int_int_values, templator_cmp_int_int_values, templator_unknown_cmp, templator_unknown_cmp },
    { templator_cmp_int_int_values, templator_cmp_int_int_values, templator_unknown_cmp, templator_unknown_cmp },
    { templator_unknown_cmp, templator_unknown_cmp, templator_cmp_str_str_values, templator_cmp_str_str_values },
    { templator_unknown_cmp, templator_unknown_cmp, templator_cmp_str_str_values, templator_cmp_str_str_values },
};

int templator_compare_values(TemplatorValue* lhs, TemplatorValue* rhs) {
    TemplatorCmpValues func = VAR_CMPS[lhs->type][rhs->type];
    return func(lhs, rhs);
}

int templator_comparison_parse(TemplatorComparison* comparison, TemplatorParser* parser, TemplatorTemplate* templ) {
    Token lhs_token = templator_parser_next_token(parser);
    switch (lhs_token.type) {
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
            comparison->lhs.variableIndex = templator_template_try_insert_variable(templ, lhs_token.data, lhs_token.len);
            comparison->lhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_VARIABLE_REFERENCE;
        } break;
        case NUMB: {
            char buffer[8 * sizeof(intmax_t)];
            strncpy(buffer, lhs_token.data, lhs_token.len);
            buffer[lhs_token.len] = 0;
            intmax_t val;
            sscanf(buffer, "%" SCNdMAX, &val);
            comparison->lhs.local.type = TEMPLATOR_VALUE_TYPE_INT;
            comparison->lhs.local.i = val;
            comparison->lhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_LOCAL_VALUE;
        } break;
        default:
            return TEMPLATOR_UNEXPECTED_TOKEN;
    }

    Token op_token = templator_parser_peek_token(*parser);
    switch (op_token.type) {
        case PAREN_CLOSE:
            comparison->op = TEMPLATOR_COMPARISON_OPERATOR_VARIABLE_EVAL;
            return 0;
        case OPERATOR: {
            int res = templator_comparison_chain_validate_operator(op_token.data, op_token.len);
            if (res < 0) {
                if (templator_comparison_operator_validate(op_token.data, op_token.len) >= 0) {
                    comparison->op = TEMPLATOR_COMPARISON_OPERATOR_VARIABLE_EVAL;
                    return 0;
                }
                return res;
            }
            templator_parser_next_token(parser);
            comparison->op = (TEMPLATOR_COMPARISON_OPERATOR)res;
        } break;
        default:
            return TEMPLATOR_UNEXPECTED_TOKEN;
    }

    Token rhs_token = templator_parser_next_token(parser);
    switch (rhs_token.type) {
        case WORD: {
            comparison->rhs.variableIndex = templator_template_try_insert_variable(templ, rhs_token.data, rhs_token.len);
            comparison->rhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_VARIABLE_REFERENCE;
        } break;
        case NUMB: {
            char buffer[8 * sizeof(intmax_t)];
            strncpy(buffer, rhs_token.data, rhs_token.len);
            buffer[rhs_token.len] = 0;
            intmax_t val;
            sscanf(buffer, "%" SCNdMAX, &val);
            comparison->rhs.local.type = TEMPLATOR_VALUE_TYPE_INT;
            comparison->rhs.local.i = val;
            comparison->rhsType = TEMPLATOR_SIDE_COMPARISON_TYPE_LOCAL_VALUE;
        } break;
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
        bool kill_loop = false;

        if (res < 0) {
            return res;
        }

        Token op_token = templator_parser_next_token(parser);
        switch (op_token.type) {
            case PAREN_CLOSE:
                kill_loop = true;
                break;
            case OPERATOR: {
                int res = templator_comparison_operator_validate(op_token.data, op_token.len);
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
            } break;
            default:
                return TEMPLATOR_UNEXPECTED_TOKEN;
        }

        if (kill_loop) {
            break;
        }
    }
    return 0;
}

int templator_comparison_chain_eval(TemplatorComparisonChain* compChain, TemplatorTemplate* templ, TemplatorVariables* variables) {
    if (compChain->next == NULL) {
        return templator_comparison_eval(&compChain->cmp, templ, variables);
    }
    const TEMPLATOR_CHAIN_OPERATOR operator = compChain->op;
    TemplatorComparisonChain* curr = compChain;
    int res;

    while (curr != NULL) {
        res = templator_comparison_eval(&curr->cmp, templ, variables);
        if (res < 0) {
            return res;
        }

        if (operator == TEMPLATOR_CHAIN_OPERATOR_OR && res) {
            return 1;
        }
        if (operator == TEMPLATOR_CHAIN_OPERATOR_AND && !res) {
            return 0;
        }

        curr = curr->next;
    }

    switch (operator) {
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
            return (int)templator_is_value_truthy(lhs);
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
                return (int)templator_is_comparison_result_ok(comparison->op, res);
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

int templator_comparison_chain_validate_operator(const char* data, size_t len) {
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
            char* variable_name = templ->variables[comparison->lhs.variableIndex];
            return templator_variables_get_variable(variables, variable_name);
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
            char* variable_name = templ->variables[comparison->rhs.variableIndex];
            return templator_variables_get_variable(variables, variable_name);
        }
        case TEMPLATOR_SIDE_COMPARISON_TYPE_COMPARISON_CHAIN:
            return NULL;
    }
    return NULL;
}

int templator_comparison_operator_validate(const char* data, size_t len) {
    if (len != 2 || data[0] != data[1] || (data[0] != '&' && data[0] != '|')) {
        return TEMPLATOR_INVALID_CHAIN_OPERATOR;
    }

    switch (data[0]) {
        case '&':
            return TEMPLATOR_CHAIN_OPERATOR_AND;
        case '|':
            return TEMPLATOR_CHAIN_OPERATOR_OR;
        default:
            return TEMPLATOR_INVALID_CHAIN_OPERATOR;
    }

    return TEMPLATOR_INVALID_CHAIN_OPERATOR;
}
