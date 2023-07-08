#include "templator/instructions/conditional_insert_text_instruction.h"
#include "templator/template.h"

void conditional_insert_text_data_init(ConditionalInsertTextData* citd, TemplatorTemplate innerTemplatorTemplate, TemplatorComparisonChain comp) {
    citd->chain = comp;
    citd->templ = malloc(sizeof(TemplatorTemplate));
    *citd->templ = innerTemplatorTemplate;
}

void conditional_insert_text_data_free(ConditionalInsertTextData* citd) {
    templator_comparison_chain_free(&citd->chain);
    templator_template_free(citd->templ);
    free(citd->templ);
}
