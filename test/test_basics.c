#include "templator/templator.h"
#include "templator/definitions.h"

#include <assert.h>
#include <string.h>

void realloc_str_and_append(void* data, const char* src, size_t len) {
    char** strRef = (char**) data;
    char* str = *strRef;
    size_t originalLen = strlen(str);
    size_t newLen = originalLen + len;
    str = realloc(str, newLen + 1);
    strncpy(str + originalLen, src, len);
    str[newLen] = 0;
    *strRef = str;
}

void test_template_string_with_no_template(void) {
    const char* template = "toto je genericky text";

    Templator templator;
    char* str = malloc(1);
    str[0] = 0;
    templator_init(&templator);

    int res = templator_run_external(&templator, (char*)template, strlen(template), NULL, (void*)&str, realloc_str_and_append);
    (void)res;
    assert(res == 0);
    assert(strcmp(str, template) == 0);

    templator_free(&templator);
    free(str);
}

void test_basic_variable_insertion(void) {
    const char* template = "a: {%a%}, b: {% b %}, c: {%   c %}, d: {% d   %}, a: {%a %}, b: {% b%}, test";

    Templator templator;
    char* str = malloc(1);
    str[0] = 0;
    templator_init(&templator);

    Variables vars;
    variables_init(&vars);

    variables_set_int_variable(&vars, "a", -69);
    variables_set_uint_variable(&vars, "b", 420);
    variables_set_cpy_str_variable(&vars, "c", (char *)"coppied text", TEMPLATOR_STR_NUL_TERMINATED);
    variables_set_str_variable(&vars, "d", (char *)"original text", 13);

    int res = templator_run_external(&templator, (char*)template, strlen(template), &vars, (void*)&str, realloc_str_and_append);
    (void)res;
    assert(res == 0);
    assert(strcmp(str, "a: -69, b: 420, c: coppied text, d: original text, a: -69, b: 420, test") == 0);
}

int main(void) {
    test_template_string_with_no_template();
    test_basic_variable_insertion();
    return 0;
}
