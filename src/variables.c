#include "templator/variables.h"
#include "templator/avl_tree.h"
#include "templator/value.h"

#include <string.h>

void templator_value_free(TemplatorValue* val) {
    if (val->type == TEMPLATOR_VALUE_TYPE_CSTR_OWN) {
        free(val->s.data);
        val->s.data = NULL;
        val->s.len = 0;
    }
}

const char* templator_value_get_name(TemplatorValue* val) {
    return val->name;
}

TemplatorAVLTreeConfig templator_variables_setup_avl_tree() {
    TemplatorAVLTreeConfig conf;
    conf.valueSize = sizeof(TemplatorValue);
    conf.compareKeys = (int (*)(void*, void*))strcmp;
    conf.getKey = (void* (*)(void*))templator_value_get_name;
    conf.freeNodeData = (void (*)(void*))templator_value_free;
    return conf;
}

static TemplatorAVLTreeConfig TEMPLATOR_VARIABLES_TREE_CONFIG;

void templator_variables_init(TemplatorVariables* variables) {
    if (TEMPLATOR_VARIABLES_TREE_CONFIG.getKey == NULL) {
        TEMPLATOR_VARIABLES_TREE_CONFIG = templator_variables_setup_avl_tree();
    }
    templator_avl_tree_init(&variables->tree, &TEMPLATOR_VARIABLES_TREE_CONFIG);
}

void templator_variables_free(TemplatorVariables* variables) {
    templator_avl_tree_free(&variables->tree);
}

void templator_variables_set_int_variable(TemplatorVariables* variables, const char* name, intmax_t value) {
    TemplatorValue* var = templator_avl_tree_insert_with_key_hint(&variables->tree, (void*)name);
    if (var == NULL) { // already exists
        var = templator_avl_tree_get_with_key_hint(&variables->tree, (void*)name);
        templator_value_free(var);
    }
    var->name = name;
    var->type = TEMPLATOR_VALUE_TYPE_INT;
    var->i = value;
}

void templator_variables_set_uint_variable(TemplatorVariables* variables, const char* name, uintmax_t value) {
    TemplatorValue* var = templator_avl_tree_insert_with_key_hint(&variables->tree, (void*)name);
    if (var == NULL) { // already exists
        var = templator_avl_tree_get_with_key_hint(&variables->tree, (void*)name);
        templator_value_free(var);
    }
    var->name = name;
    var->type = TEMPLATOR_VALUE_TYPE_UINT;
    var->u = value;
}

#define STR_ASSIGN(dst, src, length)              \
    size_t strLen = length;                       \
    if (strLen == TEMPLATOR_STR_NUL_TERMINATED) { \
        strLen = strlen(src);                     \
    }                                             \
    STR_CPY(dst, src, strLen);                    \
    (dst).len = strLen;

#define STR_CPY(dst, src, len) \
    dst.data = src;

void templator_variables_set_str_variable(TemplatorVariables* variables, const char* name, char* value, size_t valueLen) {
    TemplatorValue* var = templator_avl_tree_insert_with_key_hint(&variables->tree, (void*)name);
    if (var == NULL) { // already exists
        var = templator_avl_tree_get_with_key_hint(&variables->tree, (void*)name);
        templator_value_free(var);
    }
    var->name = name;
    var->type = TEMPLATOR_VALUE_TYPE_CSTR_REF;
    STR_ASSIGN(var->s, value, valueLen);
}

#undef STR_CPY

#define STR_CPY(dst, src, len)     \
    dst.data = malloc((len) + 1);  \
    strncpy((dst).data, src, len); \
    (dst).data[len] = 0;

void templator_variables_set_cpy_str_variable(TemplatorVariables* variables, const char* name, char* value, size_t valueLen) {
    TemplatorValue* var = templator_avl_tree_insert_with_key_hint(&variables->tree, (void*)name);
    if (var == NULL) { // already exists
        var = templator_avl_tree_get_with_key_hint(&variables->tree, (void*)name);
        templator_value_free(var);
    }
    var->name = name;
    var->type = TEMPLATOR_VALUE_TYPE_CSTR_OWN;
    STR_ASSIGN(var->s, value, valueLen);
}

void templator_variables_clear_variables(TemplatorVariables* variables) {
    templator_avl_tree_free(&variables->tree);
    variables->tree.root = NULL;
}

void templator_variables_remove_variable(TemplatorVariables* /*variables*/, const char* /*name*/) {
    // FIXME: Implement removing variables
}

TemplatorValue* templator_variables_get_variable(TemplatorVariables* variables, const char* name) {
    return templator_avl_tree_get_with_key_hint(&variables->tree, (void*)name);
}
