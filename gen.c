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
        .variable = str_new(),
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
    str_add_cstr(&gen->header, "DEFVAR GF@_tmp1\n");
    str_add_cstr(&gen->header, "DEFVAR GF@_tmp2\n");
    str_add_cstr(&gen->header, "DEFVAR GF@_tmp3\n");
    gen->current = &gen->global;
    gen->current_header = &gen->header;
}

void gen_footer(gen_t* gen) {
    str_add_cstr(&gen->global, "EXIT int@0\n");
    str_add_cstr(&gen->global, "LABEL !ERR_CALL\n");
    str_add_cstr(&gen->global, "EXIT int@3\n");  // TODO: Check error code
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

void gen_value(gen_t* gen, token_t* token) {
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
            str_add_cstr(gen->current, "GF@");  // TODO: Get legit scope
            str_add_cstr(gen->current, token->attr.val_s.val);
            break;
        default:
            error_exit(ERR_INTERNAL);
    }
}

void gen_function(gen_t* gen, token_t* token) {
    // Define variable for declaration check
    str_add_cstr(&gen->header, "DEFVAR GF@?");
    str_add_cstr(&gen->header, token->attr.val_s.val);
    str_add_cstr(&gen->header, "$declared\n");
    str_add_cstr(&gen->header, "MOVE GF@?");
    str_add_cstr(&gen->header, token->attr.val_s.val);
    str_add_cstr(&gen->header, "$declared bool@false\n");
    // Mark function as declared
    str_add_cstr(&gen->global, "MOVE GF@?");
    str_add_cstr(&gen->global, token->attr.val_s.val);
    str_add_cstr(&gen->global, "$declared bool@true\n");
    // Generate label for our function
    str_add_cstr(&gen->function_header, "LABEL ");
    str_add_cstr(&gen->function_header, token->attr.val_s.val);
    str_add_cstr(&gen->function_header, "\n");
    gen->current = &gen->function;
    gen->current_header = &gen->function_header;
}

void gen_function_end(gen_t* gen) {
    str_add_cstr(&gen->function, "RETURN\n");
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
    // Check if function is declared
    str_add_cstr(gen->current, "JUMPIFEQ !ERR_CALL bool@false GF@?");
    str_add_cstr(gen->current, token->attr.val_s.val);
    str_add_cstr(gen->current, "$declared\n");
    // Generate call frame
    str_add_cstr(gen->current, "CREATEFRAME\n");
    str_add_str(&gen->function_name, &token->attr.val_s);
}

void gen_exp_from_tree(gen_t* gen, token_term_t* root) {
    if (root == NULL) {
        return;
    }

    gen_exp_from_tree(gen, root->left);
    gen_exp_from_tree(gen, root->right);

    if (token_is_literal(&root->value) || root->value.type == TOK_VAR) {
        str_add_cstr(gen->current, "PUSHS ");
        gen_value(gen, &root->value);
        str_add_cstr(gen->current, "\n");
    } else {
        switch (root->value.type) {
            case TOK_PLUS:
                str_add_cstr(gen->current, "ADDS\n");
                break;
            case TOK_MINUS:
                str_add_cstr(gen->current, "SUBS\n");
                break;
            case TOK_MULTIPLY:
                str_add_cstr(gen->current, "MULS\n");
                break;
            case TOK_DIVIDE:
                str_add_cstr(gen->current, "DIVS\n");
                break;
            case TOK_DOT:
                str_add_cstr(gen->current, "POPS GF@_tmp1\n");
                str_add_cstr(gen->current, "POPS GF@_tmp2\n");
                str_add_cstr(gen->current, "CONCAT GF@_tmp3 GF@_tmp2 GF@_tmp1\n");
                str_add_cstr(gen->current, "PUSHS GF@_tmp3\n");
                break;
            default:
                str_add_cstr(gen->current, "Not implemented yet\n");
        }
    }

    // token_print(&root->value);
}

void gen_exp(gen_t* gen, token_term_t* root) {
    gen_exp_from_tree(gen, root);
    str_add_cstr(gen->current, "POPS GF@");
    str_add_str(gen->current, &gen->variable);
    str_add_cstr(gen->current, "\n");

    str_add_cstr(gen->current, "DPRINT GF@");
    str_add_str(gen->current, &gen->variable);
    str_add_cstr(gen->current, "\n");
}

void gen_function_call_param(gen_t* gen, token_t* token) {
    str_add_cstr(gen->current, "DEFVAR TF@_");
    gen_int(gen, gen->param_count);
    str_add_char(gen->current, '\n');
    str_add_cstr(gen->current, "MOVE TF@_");
    gen_int(gen, gen->param_count);
    str_add_char(gen->current, ' ');
    gen_value(gen, token);
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
    str_free(&gen->variable);
}

void gen_emit(gen_t* gen) {
    str_print(&gen->header);
    str_print(&gen->global);
    str_print(&gen->functions);
}
