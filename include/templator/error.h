#ifndef TEMPLATOR_ERROR_H
#define TEMPLATOR_ERROR_H

#define TEMPLATE_NO_INSTRUCTION_IN_BRACKETS -1
#define TEMPLATE_INCOMPLETE_INSTRUCTION_BRACKETS -2
#define TEMPLATE_VARIABLE_NOT_SET -3

#ifdef __cplusplus
extern "C" {
#endif

const char* templator_strerror(int error_code);

#ifdef __cplusplus
}
#endif

#endif//TEMPLATOR_ERROR_H
