#include "gen.h"
#include <stdio.h>
#include "error.h"

gen_t gen_new() {
    gen_t gen = {
        .header = str_new(),
        .global = str_new(),
        .functions = str_new(),
        .function_header = str_new(),
        .function = str_new(),
        .function_name = str_new(),
        .param_count = 0,
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
    gen->current = &gen->global;
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
    str_add_cstr(gen->current, "JUMP !elseifend_");
    gen_int(gen, construct_count);
    str_add_char(gen->current, '\n');
    str_add_cstr(gen->current, "LABEL !else_");
    gen_int(gen, construct_count);
    str_add_char(gen->current, '\n');
}

void gen_if_else_end(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "!elseifend_");
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
    str_add_cstr(&gen->function_header, "LABEL ");
    str_add_cstr(&gen->function_header, token->attr.val_s.val);
    str_add_cstr(&gen->function_header, "\n");
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

void gen_function_call(gen_t* gen) {
    str_add_cstr(gen->current, "CALL ");
    str_add_str(gen->current, &gen->function_name);
    str_add_cstr(gen->current, "\n");
    gen->param_count = 0;
    str_clear(&gen->function_name);
    str_add_cstr(gen->current, "# CALL END\n");
}

void gen_function_call_frame(gen_t* gen, token_t* token) {
    str_add_cstr(gen->current, "# CALL START\n");
    str_add_cstr(gen->current, "CREATEFRAME\n");
    str_add_str(&gen->function_name, &token->attr.val_s);
}

void gen_function_call_param(gen_t* gen, token_t* token) {
    str_add_cstr(gen->current, "DEFVAR TF@_");
    gen_int(gen, gen->param_count);
    str_add_char(gen->current, '\n');
    str_add_cstr(gen->current, "MOVE TF@_");
    gen_int(gen, gen->param_count);
    str_add_char(gen->current, ' ');
    switch (token->type) {
        case TOK_INT_LIT: {
            str_add_cstr(gen->current, "int@");
            char buf[16];
            sprintf(buf, "%d", token->attr.val_i);
            str_add_cstr(gen->current, buf);
            break;
        }
        case TOK_FLOAT_LIT: {
            str_add_cstr(gen->current, "float@");
            char buf[32];
            sprintf(buf, "%a", token->attr.val_f);
            str_add_cstr(gen->current, buf);
            break;
        }
        case TOK_STR_LIT:
            str_add_cstr(gen->current, "string@");
            for (size_t i = 0; i < token->attr.val_s.len; i++) {
                char c = token->attr.val_s.val[i];
                if ((c >= 0 && c <= 32) || c == 35 || c == 92) {
                    str_add_char(gen->current, '\\');
                    char buf[5];
                    sprintf(buf, "%03d", c);
                    str_add_cstr(gen->current, buf);
                } else {
                    str_add_char(gen->current, c);
                }
            }
            break;
        case TOK_VAR:
            str_add_cstr(gen->current, token->attr.val_s.val);
            break;
        default:
            error_exit(ERR_INTERNAL);
    }
    str_add_char(gen->current, '\n');
    gen->param_count++;
}

void gen_variable_def(gen_t* gen, token_t* token, bool in_function) {
    str_add_cstr(gen->current_header, "DEFVAR ");
    if (in_function) {
        str_add_cstr(gen->current_header, "LF@");
    } else {
        str_add_cstr(gen->current_header, "GF@");
    }
    str_add_str(gen->current_header, &token->attr.val_s);
    str_add_char(gen->current_header, '\n');
}

void gen_free(gen_t* gen) {
    str_free(&gen->header);
    str_free(&gen->global);
    str_free(&gen->functions);
    str_free(&gen->function_header);
    str_free(&gen->function);
    str_free(&gen->function_name);
}

void gen_emit(gen_t* gen) {
    str_print(&gen->header);
    str_print(&gen->global);
    str_print(&gen->functions);
}
