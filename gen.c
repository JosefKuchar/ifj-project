/**
 * @file gen.c
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief IFJ22 code generator
 */

#include "gen.h"
#include <stdio.h>
#include <string.h>
#include "buildin.h"
#include "error.h"

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
    // Program header
    str_add_cstr(&gen->header, ".IFJcode22\n");
    // Temporary variables for operations
    str_add_cstr(&gen->header, "DEFVAR GF@?tmp1\n");
    str_add_cstr(&gen->header, "DEFVAR GF@?tmp2\n");
    str_add_cstr(&gen->header, "DEFVAR GF@?tmp3\n");
    str_add_cstr(&gen->header, "DEFVAR GF@?tmp4\n");
    str_add_cstr(&gen->header, "DEFVAR GF@?type1\n");
    str_add_cstr(&gen->header, "DEFVAR GF@?type2\n");
    // Generate buidin functions
    gen_func_write(gen);
    gen_func_readi(gen);
    gen_func_readf(gen);
    gen_func_reads(gen);
    gen_func_strlen(gen);
    gen_func_chr(gen);
    gen_func_ord(gen);
    gen_func_substring(gen);
    gen_func_floatval(gen);
    gen_func_intval(gen);
    gen_func_strval(gen);
    // Generate helper functions
    gen_num_prepare(gen);
    gen_num_prepare_div(gen);
    gen_concat(gen);
    gen_to_bool(gen);
    gen_equals(gen);
    gen_comp_prepare(gen);
    gen_greater(gen);
    gen_greater_equals(gen);
    // Set current scope
    gen->current = &gen->global;
    gen->current_header = &gen->header;
}

void gen_footer(gen_t* gen) {
    // Global exit (success)
    str_add_cstr(&gen->global, "EXIT int@0\n");
    // Error exits
    str_add_cstr(&gen->global,
                 "LABEL !ERR_CALL\n"
                 "EXIT int@3\n"
                 "LABEL !ERR_SEM_FUN\n"
                 "EXIT int@4\n"
                 "LABEL !ERR_SEM_VAR\n"
                 "EXIT int@5\n"
                 "LABEL !ERR_SEM_RET\n"
                 "EXIT int@6\n"
                 "LABEL !ERR_SEM_COMP\n"
                 "EXIT int@7\n");
}

void gen_if(gen_t* gen, int construct_count) {
    // Jump to else branch if condition is not met
    str_add_cstr(gen->current,
                 "CALL !to_bool\n"
                 "JUMPIFEQ !else_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, " GF@?tmp1 bool@false\n");
}

void gen_else(gen_t* gen, int construct_count) {
    // Jump to if-else end in if branch
    str_add_cstr(gen->current, "JUMP !elseifend_");
    gen_int(gen, construct_count);
    str_add_char(gen->current, '\n');
    // Else branch label
    str_add_cstr(gen->current, "LABEL !else_");
    gen_int(gen, construct_count);
    str_add_char(gen->current, '\n');
}

void gen_if_else_end(gen_t* gen, int construct_count) {
    // If-else end label
    str_add_cstr(gen->current, "LABEL !elseifend_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_while(gen_t* gen, int construct_count) {
    // While label
    str_add_cstr(gen->current, "LABEL !while_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

void gen_while_exit(gen_t* gen, int construct_count) {
    // Jump to while end if condition is not met
    str_add_cstr(gen->current,
                 "CALL !to_bool\n"
                 "JUMPIFEQ !whileend_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, " GF@?tmp1 bool@false\n");
}

void gen_while_end(gen_t* gen, int construct_count) {
    // Jump back to while label
    str_add_cstr(gen->current, "JUMP !while_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
    // Define while end label
    str_add_cstr(gen->current, "LABEL !whileend_");
    gen_int(gen, construct_count);
    str_add_cstr(gen->current, "\n");
}

/**
 * @brief Generate value (literal / variable) from token
 *
 * @param str Destination
 * @param token Source token
 * @param in_function Whether are we in function scope
 */
void gen_value(str_t* str, token_t* token, bool in_function) {
    switch (token->type) {
        case TOK_INT_LIT: {
            str_add_cstr(str, "PUSHS ");
            str_add_cstr(str, "int@");
            char buf[16];
            sprintf(buf, "%d", token->attr.val_i);
            str_add_cstr(str, buf);
            break;
        }
        case TOK_FLOAT_LIT: {
            str_add_cstr(str, "PUSHS ");
            str_add_cstr(str, "float@");
            char buf[32];
            sprintf(buf, "%a", token->attr.val_f);
            str_add_cstr(str, buf);
            break;
        }
        case TOK_STR_LIT:
            str_add_cstr(str, "PUSHS ");
            str_add_cstr(str, "string@");
            for (size_t i = 0; i < token->attr.val_s.len; i++) {
                char c = token->attr.val_s.val[i];
                // These ASCII codes have to be represented with escape sequences
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
            str_add_cstr(str, "PUSHS ");
            str_add_cstr(str, "nil@nil");
            break;
        case TOK_VAR:
            str_add_cstr(str, "TYPE GF@?type1 ");
            if (in_function) {
                str_add_cstr(str, "LF@");
            } else {
                str_add_cstr(str, "GF@");
            }
            str_add_str(str, &token->attr.val_s);
            str_add_cstr(str, "\nJUMPIFEQ !ERR_SEM_VAR string@ GF@?type1\n");
            str_add_cstr(str, "PUSHS ");
            if (in_function) {
                str_add_cstr(str, "LF@");
            } else {
                str_add_cstr(str, "GF@");
            }
            str_add_str(str, &token->attr.val_s);
            break;
        default:
            // Other token types shouldn't be here
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
    // Create function frame and return value
    str_add_cstr(&gen->function_header,
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@?tmp1\n");
    // Set scope to function
    gen->current = &gen->function;
    gen->current_header = &gen->function_header;
}

void gen_function_end(gen_t* gen, htab_fun_t* function) {
    // Get values from stack to local variables
    for (int i = 0; i < function->param_count; i++) {
        // Define local variable
        str_add_cstr(&gen->function_header, "DEFVAR LF@");
        str_add_str(&gen->function_header, &function->params[i].name);
        str_add_char(&gen->function_header, '\n');
        // Pop from stack
        str_add_cstr(&gen->function_header, "POPS LF@");
        str_add_str(&gen->function_header, &function->params[i].name);
        str_add_char(&gen->function_header, '\n');
    }

    // No return where expected
    if (function->returns.type != TOK_VOID && function->returns.required != false) {
        str_add_cstr(&gen->function, "JUMP !ERR_SEM_FUN\n");
    }

    // Generate default return from function without passing value
    str_add_cstr(&gen->function,
                 "POPFRAME\n"
                 "RETURN\n");
    // Add our complete function to other functions
    str_add_str(&gen->functions, &gen->function_header);
    str_add_str(&gen->functions, &gen->function);
    // Clear current function strings
    str_clear(&gen->function_header);
    str_clear(&gen->function);
    // Set scope back to global
    gen->current = &gen->global;
    gen->current_header = &gen->header;
}

void gen_return(gen_t* gen, htab_fun_t* function) {
    if (function != NULL) {
        // Check if function returns value
        if (function->returns.type == TOK_VOID) {
            str_add_cstr(gen->current_header, "JUMP !ERR_SEM_RET\n");
        }

        // Check return value type

        // Return value that we got from last expression
        str_add_cstr(gen->current,
                     "PUSHS GF@?tmp1\n"
                     "POPFRAME\n"
                     "RETURN\n");
    } else {
        // Return from main scope
        str_add_cstr(gen->current, "EXIT int@0\n");
    }
}

void gen_return_void(gen_t* gen, htab_fun_t* function) {
    if (function != NULL) {
        // Check if we can return without value
        if (function->returns.type != TOK_VOID && function->returns.required != false) {
            str_add_cstr(gen->current, "JUMP !ERR_SEM_RET\n");
        }

        // Just return without returning value
        str_add_cstr(gen->current,
                     "POPFRAME\n"
                     "RETURN\n");
    } else {
        // Return from main scope
        str_add_cstr(gen->current, "EXIT int@0\n");
    }
}

void gen_function_call(gen_t* gen, bool in_function) {
    // Include call parameters
    str_add_str(gen->current, &gen->params);
    str_clear(&gen->params);

    // This is special case: If function is "write" (with variable term count)
    // We push number of terms to stack so the function knows how many there are
    if (strcmp(gen->function_name.val, "write") == 0) {
        str_add_cstr(gen->current, "PUSHS int@");
        gen_int(gen, gen->param_count);
        str_add_char(gen->current, '\n');
    }

    // Do the actual call
    str_add_cstr(gen->current, "CALL ");
    str_add_str(gen->current, &gen->function_name);
    str_add_cstr(gen->current, "\n");
    // Cleanup
    gen->param_count = 0;
    str_clear(&gen->function_name);

    // Get returned value
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
}

void gen_function_call_frame(gen_t* gen, token_t* token) {
    // Check if function is declared
    str_add_cstr(gen->current, "JUMPIFEQ !ERR_CALL bool@false GF@?");
    str_add_cstr(gen->current, token->attr.val_s.val);
    str_add_cstr(gen->current, "$declared\n");
    // Save function name for future use (actual calling)
    str_add_str(&gen->function_name, &token->attr.val_s);
}

/**
 * @brief Generate expression instructions from expression tree (post-order traversal)
 *
 * @param gen Generator instance
 * @param root Root node
 * @param in_function Whether are we in function
 */
void gen_exp_from_tree(gen_t* gen, token_term_t* root, bool in_function) {
    // Parent is leaf
    if (root == NULL) {
        return;
    }

    // Generate children
    gen_exp_from_tree(gen, root->left, in_function);
    gen_exp_from_tree(gen, root->right, in_function);

    // If token is literal / variable (leaf node) then just generate value
    if (token_is_literal(&root->value) || root->value.type == TOK_VAR) {
        gen_value(gen->current, &root->value, in_function);
        str_add_cstr(gen->current, "\n");
    } else {
        // Do operations
        switch (root->value.type) {
            case TOK_PLUS:
                str_add_cstr(gen->current,
                             "CALL !num_prepare\n"
                             "ADDS\n");
                break;
            case TOK_MINUS:
                str_add_cstr(gen->current,
                             "CALL !num_prepare\n"
                             "SUBS\n");
                break;
            case TOK_MULTIPLY:
                str_add_cstr(gen->current,
                             "CALL !num_prepare\n"
                             "MULS\n");
                break;
            case TOK_DIVIDE:
                str_add_cstr(gen->current,
                             "CALL !num_prepare_div\n"
                             "DIVS\n");
                break;
            case TOK_DOT:
                str_add_cstr(gen->current, "CALL !concat\n");
                break;
            case TOK_EQUALS:
                str_add_cstr(gen->current, "CALL !equals\n");
                break;
            case TOK_NEQUALS:
                str_add_cstr(gen->current,
                             "CALL !equals\n"
                             "NOTS\n");
                break;
            case TOK_LESS:
                str_add_cstr(gen->current,
                             "POPS GF@?tmp1\n"
                             "POPS GF@?tmp2\n"
                             "CALL !greater\n");
                break;
            case TOK_LESS_E:
                str_add_cstr(gen->current,
                             "POPS GF@?tmp1\n"
                             "POPS GF@?tmp2\n"
                             "CALL !greater_equals\n");
                break;
            case TOK_GREATER:
                str_add_cstr(gen->current,
                             "POPS GF@?tmp2\n"
                             "POPS GF@?tmp1\n"
                             "CALL !greater\n");
                break;
            case TOK_GREATER_E:
                // Greater-than-equals is negated less-than
                str_add_cstr(gen->current,
                             "POPS GF@?tmp2\n"
                             "POPS GF@?tmp1\n"
                             "CALL !greater_equals\n");
                break;
            default:
                // This shouldn't happen
                error_not_implemented();
        }
    }
}

void gen_exp(gen_t* gen, token_term_t* root, bool in_function) {
    // Generate actual expression from tree
    gen_exp_from_tree(gen, root, in_function);

    // Return expression / expression without assignment
    if (gen->variable.len == 0) {
        str_add_cstr(gen->current, "POPS GF@?tmp1\n");
        // Assign (pop from stack) expression result to saved variable name
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
    // Add instruction parameters to stack
    // We have to push them in reverse order
    // TODO: Rewrite this, this is ugly
    str_t str = str_new();
    gen_value(&str, token, in_function);
    str_add_cstr(&str, "\n");
    str_add_str(&str, &gen->params);
    str_clear(&gen->params);
    str_add_str(&gen->params, &str);
    str_free(&str);

    gen->param_count++;
}

void gen_variable_def(gen_t* gen, token_t* token, bool in_function) {
    // Define new variable based on scope
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
