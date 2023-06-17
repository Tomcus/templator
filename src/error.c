#include "templator/error.h"

const char* templator_strerror(int error_code) {
    switch (error_code) {
        case TEMPLATE_NO_INSTRUCTION_IN_BRACKETS:
            return "No instruction inside {% %}";
        case TEMPLATE_INCOMPLETE_INSTRUCTION_BRACKETS:
            return "Unable to find closing \"%}\"";
        default:
            return "Unknown error";
    }
}
