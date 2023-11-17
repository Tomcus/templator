#ifndef TEMPLATOR_H
#define TEMPLATOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "definitions.h"
#include "template.h"
#include "variables.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* name;
    TemplatorTemplate templ;
} TemplatorNameAndTemplatePair;

typedef struct {
    TemplatorNameAndTemplatePair* templates;
    size_t templatesCap;
    size_t templatesCnt;
} Templator;

void templator_init(Templator* templator);
void templator_free(Templator* templator);

int templator_add_named_template(Templator* templator, const char* name, char* data, size_t len);
TemplatorTemplate* templator_get_template_by_name(const Templator* templator, const char* name);

typedef void (*TemplatorAppendStrFunction)(void*, const char*, size_t);

int templator_run(const Templator* templator, TemplatorTemplate* templ, TemplatorVariables* variables, void* data, TemplatorAppendStrFunction appendFunction);
int templator_run_named(const Templator* templator, const char* name, TemplatorVariables* variables, void* data, TemplatorAppendStrFunction appendFunction);
int templator_run_external(const Templator* templator, char* templateData, size_t templateLen, TemplatorVariables* variables, void* data, TemplatorAppendStrFunction appendFunction);

#ifdef __cplusplus
}
#endif

#endif // TEMPLATOR_H
