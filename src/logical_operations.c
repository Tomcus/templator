#include "templator/command_tokenizer.h"
#include "templator/error.h"
#include "templator/logical_operations.h"
#include "templator/template.h"
#include "templator/variable_comparisons.h"

#include <inttypes.h>
#include <string.h>
#include <stdio.h>

bool is_variable_truthy(Variable* var) {
    if (var == NULL) {
        return false;
    }

    switch(var->type) {
        case VARIABLE_INT:
            return var->i != 0; 
        case VARIABLE_UINT:
            return var->u > 0;
        case VARIABLE_CSTR_OWN:
        case VARIABLE_CSTR_REF:
            return var->s.len > 0;
    }
    return false;
}

bool is_comparison_result_ok(COMPARISON_OPERATOR op, COMPARISON_RESULT res) {
    switch(res) {
        case EQ:
            return op == CMP_EQ || op == CMP_GE || op == CMP_LE;
        case GT:
            return op == CMP_NE || op == CMP_GE || op == CMP_GT;
        case LT:
            return op == CMP_NE || op == CMP_LE || op == CMP_LT;
    }
    return false;
}

const cmpFunction VAR_CMPS[4][4] = {
    {variables_cmp_int_int, variables_cmp_int_int, unknown_cmp, unknown_cmp},
    {variables_cmp_int_int, variables_cmp_int_int, unknown_cmp, unknown_cmp},
    {unknown_cmp, unknown_cmp, variables_cmp_str_str, variables_cmp_str_str},
    {unknown_cmp, unknown_cmp, variables_cmp_str_str, variables_cmp_str_str},
}; 


int variables_compare(Variable* a, Variable* b) {
    cmpFunction func = VAR_CMPS[a->type][b->type];
    return func(a, b);
}

int templator_comparison_parse(Comparison* comparison, Parser* parser, Template* templ) {
    Token lhsToken = templator_parser_next_token(parser);
    switch(lhsToken.type) {
        case PAREN_OPEN: {
            comparison->lhs.chain = malloc(sizeof(ComparisonChain));
            comparison->lhsType = COMPARISON_CHAIN;
            int res = comparison_chain_parse(comparison->lhs.chain, parser, templ);
            if (res != 0) {
                return res;
            }
            comparison->op = CMP_CHAIN_EVAL;
            return 0;
        }
        case WORD: {
            comparison->lhs.variableIndex = template_try_insert_variable(templ, lhsToken.data, lhsToken.len);
            comparison->lhsType = VARIABLE_REFERENCE;
        }
        break;
        case NUMB: {
            char buffer[8*sizeof(intmax_t)];
            strncpy(buffer, lhsToken.data, lhsToken.len);
            buffer[lhsToken.len] = 0;
            intmax_t val;
            sscanf(buffer, "%"SCNdMAX, &val);
            comparison->lhs.local.type = VARIABLE_INT;
            comparison->lhs.local.i = val;
            comparison->lhsType = LOCAL_VARIABLE;
        }
        break;
        default:
            return TEMPLATOR_UNEXPECTED_TOKEN;
    }

    Token opToken = peek_next_token(*parser);
    switch (opToken.type) {
        case PAREN_CLOSE:
            comparison->op = CMP_VARIABLE_EVAL;
            return 0;
        case OPERATOR: {
            int res = comparison_chain_validate_operator(opToken.data, opToken.len);
            if (res < 0) {
                if (templator_comparison_operator_validate(opToken.data, opToken.len) >= 0) {
                    comparison->op = CMP_VARIABLE_EVAL;
                    return 0;
                }
                return res;
            }
            templator_parser_next_token(parser);
            comparison->op = (COMPARISON_OPERATOR)res;
        }
        break;
        default:
            return TEMPLATOR_UNEXPECTED_TOKEN;
    }

    Token rhsToken = templator_parser_next_token(parser);
    switch (rhsToken.type) {
        case WORD: {
            comparison->rhs.variableIndex = template_try_insert_variable(templ, rhsToken.data, rhsToken.len);
            comparison->rhsType = VARIABLE_REFERENCE;
        }
        break;
        case NUMB: {
            char buffer[8*sizeof(intmax_t)];
            strncpy(buffer, rhsToken.data, rhsToken.len);
            buffer[rhsToken.len] = 0;
            intmax_t val;
            sscanf(buffer, "%"SCNdMAX, &val);
            comparison->rhs.local.type = VARIABLE_INT;
            comparison->rhs.local.i = val;
            comparison->rhsType = LOCAL_VARIABLE;
        }
        break;
        default:
            return TEMPLATOR_UNEXPECTED_TOKEN;
    }
    return 0;
}

int comparison_chain_parse(ComparisonChain* compChain, Parser* parser, Template* templ) {
    compChain->next = NULL;
    ComparisonChain* curr = compChain;

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
                    compChain->op = (CHAIN_OPERATOR)res;
                } else if (compChain->op != (CHAIN_OPERATOR)res) {
                    return TEMPLATOR_CHAIN_OPERATOR_CHANGED;
                } else {
                    curr->op = (CHAIN_OPERATOR)res;
                }

                curr->next = malloc(sizeof(ComparisonChain));
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

int comparison_chain_eval(ComparisonChain* compChain, Template* templ, Variables* variables) {
    if (compChain->next == NULL) {
        return templator_comparison_eval(&compChain->cmp, templ, variables);
    }
    const CHAIN_OPERATOR op = compChain->op;
    ComparisonChain* curr = compChain;
    int res;

    while (curr != NULL) {
        res = templator_comparison_eval(&curr->cmp, templ, variables);
        if (res < 0) {
            return res;
        }

        if (op == TEMPLATOR_CMP_OR && res) {
            return 1;
        } else if (op == TEMPLATOR_CMP_AND && !res) {
            return 0;
        }

        curr = curr->next;
    }

    switch (op) {
        case TEMPLATOR_CMP_AND:
            return 1;
        case TEMPLATOR_CMP_OR:
            return 0;
    }
    return 0;
}

int templator_comparison_eval(Comparison* comparison, Template* templ, Variables* variables) {
    Variable* lhs;
    Variable* rhs;
    switch (comparison->op) {
        case CMP_CHAIN_EVAL:
            return comparison_chain_eval(comparison->lhs.chain, templ, variables);
        case CMP_VARIABLE_EVAL:
            lhs = templator_comparison_get_lhs(comparison, templ, variables);
            if (lhs == NULL) {
                return TEMPLATE_VARIABLE_NOT_SET;
            }
            return is_variable_truthy(lhs);
        default:
            lhs = templator_comparison_get_lhs(comparison, templ, variables);
            if (lhs == NULL) {
                return TEMPLATE_VARIABLE_NOT_SET;
            }
            rhs = templator_comparison_get_rhs(comparison, templ, variables);
            if (rhs == NULL) {
                return TEMPLATE_VARIABLE_NOT_SET;
            }

            int res = variables_compare(lhs, rhs);
            if (res >= -1 && res <= 1) {
                return is_comparison_result_ok(comparison->op, res);
            }
            return res;
    }
}

void comparison_chain_free(ComparisonChain* compChain) {
    if (compChain != NULL && compChain->next != NULL) {
        comparison_chain_free(compChain->next);
        free(compChain->next);
        compChain->next = NULL;
    }
}

int comparison_chain_validate_operator(char* data, size_t len) {
    switch (len) {
        case 1:
            switch (data[0]) {
                case '>':
                    return CMP_GT;
                case '<':
                    return CMP_LT;
                default:
                    return TEMPLATOR_INVALID_COMPARISON_OPERATOR;
            }
        case 2:
            if (data[1] != '=') {
                return TEMPLATOR_INVALID_COMPARISON_OPERATOR;
            }
            switch (data[0]) {
                case '>':
                    return CMP_GE;
                case '<':
                    return CMP_LE;
                case '!':
                    return CMP_NE;
                case '=':
                    return CMP_EQ;
                default:
                    return TEMPLATOR_INVALID_COMPARISON_OPERATOR;
            }
    }
    return 0;
}

Variable* templator_comparison_get_lhs(Comparison* comparison, Template* templ, Variables* variables) {
    switch (comparison->lhsType) {
        case LOCAL_VARIABLE:
            return &comparison->lhs.local;
        case VARIABLE_REFERENCE: {
            char * variableName = templ->variables[comparison->lhs.variableIndex];
            return variables_get_variable(variables, variableName);
        }
        case COMPARISON_CHAIN:
            return NULL;
    }
    return NULL;
}

Variable* templator_comparison_get_rhs(Comparison* comparison, Template* templ, Variables* variables) {
    switch (comparison->rhsType) {
        case LOCAL_VARIABLE:
            return &comparison->rhs.local;
        case VARIABLE_REFERENCE: {
            char * variableName = templ->variables[comparison->rhs.variableIndex];
            return variables_get_variable(variables, variableName);
        }
        case COMPARISON_CHAIN:
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
            return TEMPLATOR_CMP_AND;
        case '|':
            return TEMPLATOR_CMP_OR;
        default:
            return TEMPLATOR_INVALID_CHAIN_OPERATOR;
    }

    return TEMPLATOR_INVALID_CHAIN_OPERATOR;
}
