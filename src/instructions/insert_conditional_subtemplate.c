#include "templator/instructions/conditional_insert_text_instruction.h"
#include "templator/template.h"

void templator_insert_conditional_subtemplate_data_init(TemplatorInsertConditionalSubtemplateData* citd, TemplatorTemplate innerTemplatorTemplate, TemplatorComparisonChain comp) {
    citd->chain = comp;
    citd->templ = malloc(sizeof(TemplatorTemplate));
    *citd->templ = innerTemplatorTemplate;
}

void templator_insert_conditional_subtemplate_data_free(TemplatorInsertConditionalSubtemplateData* citd) {
    templator_comparison_chain_free(&citd->chain);
    templator_template_free(citd->templ);
    free(citd->templ);
}
