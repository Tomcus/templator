#ifndef TEMPLATOR_H
#define TEMPLATOR_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "definitions.h"
#include "variables.h"
#include "template.h"

typedef struct  {
    const char* name;
    Template template;
} NameAndTemplatePair;

typedef struct {
    NameAndTemplatePair* templates;
    size_t templatesCap;
    size_t templatesCnt;
} Templator;

void templator_init(Templator* templator);
void templator_free(Templator* templator);

bool templator_add_named_template(Templator* templator, const char* name, char* data, size_t len);
Template* templator_get_template_by_name(const Templator* templator, const char* name);

typedef void(*AppendFunction)(void*, const char*, size_t);

int templator_run(const Templator* templator, Template* template, Variables* variables, void* data, AppendFunction appendFunction);
int templator_run_named(const Templator* templator, const char* name, Variables* variables, void* data, AppendFunction appendFunction);
int templator_run_external(const Templator* templator, char* templateData, size_t templateLen, Variables* variables, void* data, AppendFunction appendFunction);

#endif//TEMPLATOR_H
