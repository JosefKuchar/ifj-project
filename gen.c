#include "gen.h"
#include <stdio.h>

gen_t gen_new() {
    return (gen_t){.start = str_new()};
}

void gen_int(gen_t* gen, int val) {
    char buf[16];
    sprintf(buf, "%d", val);
    str_add_cstr(&gen->start, buf);
}

void gen_header(gen_t* gen) {
    str_add_cstr(&gen->start, ".IFJcode22\n");
}

void gen_if(gen_t* gen, int construct_count) {
    str_add_cstr(&gen->start, "IF ");
    gen_int(gen, construct_count);
    str_add_cstr(&gen->start, "\n");
}

void gen_else(gen_t* gen, int construct_count) {
    str_add_cstr(&gen->start, "ELSE ");
    gen_int(gen, construct_count);
    str_add_cstr(&gen->start, "\n");
}

void gen_if_else_end(gen_t* gen, int construct_count) {
    str_add_cstr(&gen->start, "ELSE_IF_END ");
    gen_int(gen, construct_count);
    str_add_cstr(&gen->start, "\n");
}

void gen_while(gen_t* gen, int construct_count) {
    str_add_cstr(&gen->start, "WHILE ");
    gen_int(gen, construct_count);
    str_add_cstr(&gen->start, "\n");
}

void gen_while_end(gen_t* gen, int construct_count) {
    str_add_cstr(&gen->start, "WHILE_END ");
    gen_int(gen, construct_count);
    str_add_cstr(&gen->start, "\n");
}

void gen_function(gen_t* gen, token_t* token) {
    str_add_cstr(&gen->start, "LABEL ");
    str_add_cstr(&gen->start, token->attr.val_s.val);
    str_add_cstr(&gen->start, "\n");
}

void gen_function_end(gen_t* gen) {
    str_add_cstr(&gen->start, "FUNCTION_END\n");
}

void gen_function_call(gen_t* gen, token_t* token) {
    str_add_cstr(&gen->start, "CALL ");
    str_add_cstr(&gen->start, token->attr.val_s.val);
    str_add_cstr(&gen->start, "\n");
}

void gen_free(gen_t* gen) {
    str_free(&gen->start);
}

void gen_emit(gen_t* gen) {
    str_print(&gen->start);
}
