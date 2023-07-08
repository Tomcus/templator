#include "templator/instructions/conditional_insert_text_instruction.h"
#include "templator/template.h"

void conditional_insert_text_data_init(ConditionalInsertTextData* citd, Template innerTemplate, ComparisonChain comp) {
    citd->chain = comp;
    citd->templ = malloc(sizeof(Template));
    *citd->templ = innerTemplate;
}

void conditional_insert_text_data_free(ConditionalInsertTextData* citd) {
    comparison_chain_free(&citd->chain);
    template_free(citd->templ);
    free(citd->templ);
}
