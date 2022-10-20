#include "gen.h"
#include <stdio.h>
#include <string.h>
#include "error.h"

/**
 * @brief function write (term1 ,term2 , …, termn) : void
 * Writes all terms to the standard output.
 * Similar to echo in php.
 *
 * @param gen Generator instance
 */
void gen_func_write(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?write$declared\n"
                 "MOVE GF@?write$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL write\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@i\n"                          // Loop counter
                 "DEFVAR LF@current\n"                    // Current term
                 "POPS LF@i\n"                            // Get number of terms
                 "LABEL !write_loop\n"                    // Loop
                 "JUMPIFEQ !write_loop_end int@0 LF@i\n"  // Exit loop if i == 0
                 "SUB LF@i LF@i int@1\n"                  // i--
                 "POPS LF@current\n"                      // Get current term
                 "WRITE LF@current\n"                     // Output current term
                 "JUMP !write_loop\n"                     // Back to loop
                 "LABEL !write_loop_end\n"                // End of loop
                 "POPFRAME\n"
                 "RETURN\n");
}

/**
 * @brief function readi() : ?int
 * Reads an integer from the standard input.
 * Returns null if input is invalid.
 *
 * @param gen Generator instance
 */
void gen_func_readi(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?readi$declared\n"
                 "MOVE GF@?readi$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL readi\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "READ LF@tmp int\n"  // Read int
                 "PUSHS LF@tmp\n"     // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

/**
 * @brief function readf() : ?int
 * Reads a float from the standard input.
 * Returns null if input is invalid.
 *
 * @param gen Generator instance
 */
void gen_func_readf(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?readf$declared\n"
                 "MOVE GF@?readf$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL readf\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "READ LF@tmp float\n"  // Read float
                 "PUSHS LF@tmp\n"       // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

/**
 * @brief function readf() : ?int
 * Reads a string from the standard input.
 * Returns null if input is invalid.
 *
 * @param gen Generator instance
 */
void gen_func_reads(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?reads$declared\n"
                 "MOVE GF@?reads$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL reads\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "READ LF@tmp string\n"  // Read string
                 "PUSHS LF@tmp\n"        // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

/**
 * @brief function strlen(string $s) : int
 * Returns length of string.
 *
 * @param gen Generator instance
 */
void gen_func_strlen(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?strlen$declared\n"
                 "MOVE GF@?strlen$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL strlen\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"           // Get string
                 "STRLEN LF@tmp LF@tmp\n"  // Get length
                 "PUSHS LF@tmp\n"          // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

/**
 * @brief function chr(int $i) : string
 * Returns character with ASCII code $i.
 *
 * @param gen Generator instance
 */
void gen_func_chr(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?chr$declared\n"
                 "MOVE GF@?chr$declared bool@true\n");

    // TODO: Check if $i is in range 0-255
    str_add_cstr(&gen->functions,
                 "LABEL chr\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"             // Get int
                 "INT2CHAR LF@tmp LF@tmp\n"  // Convert to char
                 "PUSHS LF@tmp\n"            // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

/**
 * @brief function ord(string $c) : int
 * Returns ASCII code of first character of $c.
 * Returns 0 if $c is empty.
 *
 * @param gen Generator instance
 */
void gen_func_ord(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?ord$declared\n"
                 "MOVE GF@?ord$declared bool@true\n");

    // TODO: Return 0 if $c is empty
    str_add_cstr(&gen->functions,
                 "LABEL ord\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"                   // Get string
                 "STRI2INT LF@tmp LF@tmp int@0\n"  // Get ASCII code of first char
                 "PUSHS LF@tmp\n"                  // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

/**
 * @brief function substring(string $s, int $i, int $j) : ?string
 * Returns substring of $s starting at $i and ending at $j.
 *
 * Returns null when
 * $i < 0
 * $j < 0
 * $i > $j
 * $i >= strlen($s)
 * $j > strlen($s)
 *
 * @param gen
 */
void gen_func_substring(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?substring$declared\n"
                 "MOVE GF@?substring$declared bool@true\n");

    // TODO: Return null when conditions in function description are met
    str_add_cstr(&gen->functions,
                 "LABEL substring\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@i\n"
                 "DEFVAR LF@j\n"
                 "DEFVAR LF@str\n"
                 "DEFVAR LF@tmp\n"
                 "DEFVAR LF@res\n"
                 "POPS LF@str\n"                             // Get string
                 "POPS LF@i\n"                               // Get start index
                 "POPS LF@j\n"                               // Get end index
                 "MOVE LF@res string@\n"                     // res = ""
                 "LABEL !substring_loop\n"                   // Loop
                 "JUMPIFEQ !substring_loop_end LF@i LF@j\n"  // If i == j, end
                 "GETCHAR LF@tmp LF@str LF@i\n"              // Get char at index i
                 "CONCAT LF@res LF@res LF@tmp\n"             // res += char
                 "ADD LF@i LF@i int@1\n"                     // i++
                 "JUMP !substring_loop\n"                    // Jump to loop
                 "LABEL !substring_loop_end\n"               // Loop end
                 "PUSHS LF@res\n"                            // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

gen_t gen_new() {
    gen_t gen = {
        .header = str_new(),
        .global = str_new(),
        .functions = str_new(),
        .function_header = str_new(),
        .function = str_new(),
        .function_name = str_new(),
        .params = str_new(),
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

    gen_func_write(gen);
    gen_func_readi(gen);
    gen_func_readf(gen);
    gen_func_reads(gen);
    gen_func_strlen(gen);
    gen_func_chr(gen);
    gen_func_ord(gen);
    gen_func_substring(gen);

    gen->current = &gen->global;
    gen->current_header = &gen->header;
}

void gen_footer(gen_t* gen) {
    str_add_cstr(&gen->global, "EXIT int@0\n");
    str_add_cstr(&gen->global, "LABEL !ERR_CALL\n");
    str_add_cstr(&gen->global, "EXIT int@3\n");  // TODO: Check error code
}

void gen_if(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "JUMPIFEQ !else_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, " GF@_tmp1 bool@false\n");
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
    str_add_cstr(gen->current, "LABEL !elseifend_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_while(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "LABEL !while_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_while_exit(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "JUMPIFEQ !whileend_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, " GF@_tmp1 bool@false\n");
}

void gen_while_end(gen_t* gen, int construct_count) {
    str_add_cstr(gen->current, "JUMP !while_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
    str_add_cstr(gen->current, "LABEL !whileend_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_value(str_t* str, token_t* token, bool in_function) {
    switch (token->type) {
        case TOK_INT_LIT: {
            str_add_cstr(str, "int@");
            char buf[16];
            sprintf(buf, "%d", token->attr.val_i);
            str_add_cstr(str, buf);
            break;
        }
        case TOK_FLOAT_LIT: {
            str_add_cstr(str, "float@");
            char buf[32];
            sprintf(buf, "%a", token->attr.val_f);
            str_add_cstr(str, buf);
            break;
        }
        case TOK_STR_LIT:
            str_add_cstr(str, "string@");
            for (size_t i = 0; i < token->attr.val_s.len; i++) {
                char c = token->attr.val_s.val[i];
                if ((c >= 0 && c <= 32) || c == 35 || c == 92) {
                    str_add_char(str, '\\');
                    char buf[5];
                    sprintf(buf, "%03d", c);
                    str_add_cstr(str, buf);
                } else {
                    str_add_char(str, c);
                }
            }
            break;
        case TOK_NULL:
            str_add_cstr(str, "nil@nil");
            break;
        case TOK_VAR:
            if (in_function) {
                str_add_cstr(str, "LF@");
            } else {
                str_add_cstr(str, "GF@");
            }
            str_add_cstr(str, token->attr.val_s.val);
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
    str_add_cstr(&gen->function_header,
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@_tmp1\n");

    gen->current = &gen->function;
    gen->current_header = &gen->function_header;
}

void gen_function_end(gen_t* gen, htab_fun_t* function) {
    for (int i = 0; i < function->param_count; i++) {
        str_add_cstr(&gen->function_header, "DEFVAR LF@");
        str_add_str(&gen->function_header, &function->params[i].name);
        str_add_char(&gen->function_header, '\n');

        str_add_cstr(&gen->function_header, "POPS LF@");
        str_add_str(&gen->function_header, &function->params[i].name);
        str_add_char(&gen->function_header, '\n');
    }

    str_add_cstr(&gen->function,
                 "POPFRAME\n"
                 "RETURN\n");
    str_add_str(&gen->functions, &gen->function_header);
    str_add_str(&gen->functions, &gen->function);
    str_clear(&gen->function_header);
    str_clear(&gen->function);
    gen->current = &gen->global;
    gen->current_header = &gen->header;
}

void gen_return(gen_t* gen) {
    str_add_cstr(gen->current,
                 "PUSHS GF@_tmp1\n"  // TODO global scope
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_return_void(gen_t* gen) {
    str_add_cstr(gen->current,
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_function_call(gen_t* gen, bool in_function) {
    str_add_str(gen->current, &gen->params);
    str_clear(&gen->params);

    if (strcmp(gen->function_name.val, "write") == 0) {
        str_add_cstr(gen->current, "PUSHS int@");
        gen_int(gen, gen->param_count);
        str_add_char(gen->current, '\n');
    }

    str_add_cstr(gen->current, "CALL ");
    str_add_str(gen->current, &gen->function_name);
    str_add_cstr(gen->current, "\n");
    gen->param_count = 0;
    str_clear(&gen->function_name);

    if (strlen(gen->variable.val) != 0) {
        str_add_cstr(gen->current, "POPS ");
        if (in_function) {
            str_add_cstr(gen->current, "LF@");
        } else {
            str_add_cstr(gen->current, "GF@");
        }
        str_add_str(gen->current, &gen->variable);
        str_add_char(gen->current, '\n');
    }

    str_add_cstr(gen->current, "# CALL END\n");
}

void gen_function_call_frame(gen_t* gen, token_t* token) {
    str_add_cstr(gen->current, "# CALL START\n");
    // Check if function is declared
    str_add_cstr(gen->current, "JUMPIFEQ !ERR_CALL bool@false GF@?");
    str_add_cstr(gen->current, token->attr.val_s.val);
    str_add_cstr(gen->current, "$declared\n");
    // Generate call frame
    /*
    str_add_cstr(gen->current, "CREATEFRAME\n");*/
    str_add_str(&gen->function_name, &token->attr.val_s);
}

void gen_exp_from_tree(gen_t* gen, token_term_t* root, bool in_function) {
    if (root == NULL) {
        return;
    }

    gen_exp_from_tree(gen, root->left, in_function);
    gen_exp_from_tree(gen, root->right, in_function);

    if (token_is_literal(&root->value) || root->value.type == TOK_VAR) {
        str_add_cstr(gen->current, "PUSHS ");
        gen_value(gen->current, &root->value, in_function);
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
            case TOK_EQUALS:
                str_add_cstr(gen->current, "EQS\n");
                break;
            case TOK_NEQUALS:
                str_add_cstr(gen->current, "EQS\n");
                str_add_cstr(gen->current, "NOTS\n");
                break;
            case TOK_LESS:
                str_add_cstr(gen->current, "LTS\n");
                break;
            case TOK_LESS_E:
                str_add_cstr(gen->current, "GTS\n");
                str_add_cstr(gen->current, "NOTS\n");
                break;
            case TOK_GREATER:
                str_add_cstr(gen->current, "GTS\n");
                break;
            case TOK_GREATER_E:
                str_add_cstr(gen->current, "LTS\n");
                str_add_cstr(gen->current, "NOTS\n");
                break;
            default:
                str_add_cstr(gen->current, "Not implemented yet\n");
        }
    }

    // token_print(&root->value);
}

void gen_exp(gen_t* gen, token_term_t* root, bool in_function) {
    gen_exp_from_tree(gen, root, in_function);

    if (gen->variable.len == 0) {
        str_add_cstr(gen->current, "POPS GF@_tmp1\n");
    } else {
        if (in_function) {
            str_add_cstr(gen->current, "POPS LF@");
        } else {
            str_add_cstr(gen->current, "POPS GF@");
        }
        str_add_str(gen->current, &gen->variable);
        str_add_cstr(gen->current, "\n");
    }
}

void gen_function_call_param(gen_t* gen, token_t* token, bool in_function) {
    str_t str = str_new();
    str_add_cstr(&str, "PUSHS ");
    gen_value(&str, token, in_function);
    str_add_cstr(&str, "\n");
    str_add_str(&str, &gen->params);
    str_clear(&gen->params);
    str_add_str(&gen->params, &str);
    str_free(&str);

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
    str_free(&gen->params);
}

void gen_emit(gen_t* gen) {
    str_print(&gen->header);
    str_print(&gen->global);
    str_print(&gen->functions);
}
