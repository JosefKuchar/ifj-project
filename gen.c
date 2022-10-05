#include "gen.h"
#include <stdio.h>

gen_t gen_new() {
    gen_t gen = {
        .header = str_new(),
        .global = str_new(),
        .functions = str_new(),
        .function_header = str_new(),
        .function = str_new(),
    };
    return gen;
}

void gen_int(gen_t* gen, int val) {
    char buf[16];
    sprintf(buf, "%d", val);
    str_add_cstr(gen->current, buf);
}

void gen_header(gen_t* gen) {
    str_add_cstr(&gen->header, ".IFJcode22\n");
    gen->current = &gen->header;
    gen->current_header = &gen->header;
}

void gen_footer(gen_t* gen) {
    str_add_cstr(&gen->global, "STOP\n");
}

void gen_if(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "IF ");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_else(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "ELSE ");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_if_else_end(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "ELSE_IF_END ");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_while(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "WHILE ");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_while_end(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "WHILE_END ");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_function(gen_t* gen, token_t* token) {
    str_add_cstr(&gen->function, "LABEL ");
    str_add_cstr(&gen->function, token->attr.val_s.val);
    str_add_cstr(&gen->function, "\n");
    gen->current = &gen->function;
    gen->current_header = &gen->function_header;
}

void gen_function_end(gen_t* gen) {
    str_add_cstr(&gen->function, "FUNCTION_END\n");
    str_add_str(&gen->functions, &gen->function_header);
    str_add_str(&gen->functions, &gen->function);
    str_clear(&gen->function_header);
    str_clear(&gen->function);
    gen->current = &gen->global;
    gen->current_header = &gen->header;
}

void gen_function_call(gen_t* gen, token_t* token) {
    str_add_cstr(gen->current, "CALL ");
    str_add_cstr(gen->current, token->attr.val_s.val);
    str_add_cstr(gen->current, "\n");
}

void gen_free(gen_t* gen) {
    str_free(&gen->header);
    str_free(&gen->global);
    str_free(&gen->functions);
    str_free(&gen->function_header);
    str_free(&gen->function);
}

void gen_emit(gen_t* gen) {
    str_print(&gen->header);
    str_print(&gen->global);
    str_print(&gen->functions);
}
