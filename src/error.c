#include "templator/error.h"

const char* templator_strerror(int error_code) {
    switch (error_code) {
        case TEMPLATOR_NO_INSTRUCTION_IN_BRACKETS:
            return "No instruction inside {% %}";
        case TEMPLATOR_INCOMPLETE_INSTRUCTION_BRACKETS:
            return "Unable to find closing \"%}\"";
        case TEMPLATOR_VARIABLE_NOT_SET:
            return "Variable not set";
        case TEMPLATOR_UNEXPECTED_TOKEN:
            return "Unexpected token";
        case TEMPLATOR_UNABLE_TO_PARSE_INSTRUCTION:
            return "Unable to parse valid instruction";
        case TEMPLATOR_PARSING_ENDED_WITH_ENDIF:
            return "Template ended with not matched endif";
        case TEMPLATOR_PARSING_DIDNT_END_WITH_ENDIF:
            return "Template parsing ended with endif";
        case TEMPLATOR_UNSUPORTED_COMPARISON:
            return "Unsuported comarison between types";
        case TEMPLATOR_INVALID_COMPARISON_OPERATOR:
            return "Inalid comparison operator";
        case TEMPLATOR_CHAIN_OPERATOR_CHANGED:
            return "Unsuported change of logic operator (enclose your code in '()').";
        case TEMPLATOR_INVALID_CHAIN_OPERATOR:
            return "Ivalid logic chaining operator";
        default:
            return "Unknown error";
    }
}
