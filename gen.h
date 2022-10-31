/**
 * @file gen.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Header file for IFJ22 code generator
 */

#ifndef __GEN_H__
#define __GEN_H__

#include "str.h"
#include "symtable.h"
#include "token.h"
#include "token_term.h"

typedef struct {
    str_t header;           // Global header (init, definition of global variables)
    str_t global;           // Global code
    str_t functions;        // All functions
    str_t function_header;  // Current function header (definition of local variables)
    str_t function;         // Current function code
    str_t function_name;    // Current function name
    str_t variable;         // Current variable name
    str_t params;           // Params for funcion calls
    int param_count;        // Number of call params
    str_t* current;         // Pointer to current string (function or global)
    str_t* current_header;  // Pointer to current header (function or global)
} gen_t;

/**
 * @brief Initialize new generator
 *
 * @return Initiazed generator
 */
gen_t gen_new();

/**
 * @brief Emit final code to stdout
 *
 * @param gen Generator instance
 */
void gen_emit(gen_t* gen);

/**
 * @brief Generate program header
 *
 * @param gen Generator instance
 */
void gen_header(gen_t* gen);

/**
 * @brief Generate program footer
 *
 * @param gen Generator instance
 */
void gen_footer(gen_t* gen);

/**
 * @brief Free existing generator
 *
 */
void gen_free(gen_t* gen);

/**
 * @brief Generate if statement
 *
 * @param gen Generator instance
 */
void gen_if(gen_t* gen, int construct_count);

/**
 * @brief Generate else statement (+ jump to end of if)
 *
 * @param gen Generator instance
 */
void gen_else(gen_t* gen, int construct_count);

/**
 * @brief Generate end of if-else statement
 *
 * @param gen Generator instance
 */
void gen_if_else_end(gen_t* gen, int construct_count);

/**
 * @brief Generate while statement
 *
 * @param gen Generator instance
 */
void gen_while(gen_t* gen, int construct_count);

/**
 * @brief Generate while condition jump
 *
 * @param gen Generator instance
 * @param construct_count
 */
void gen_while_exit(gen_t* gen, int construct_count);

/**
 * @brief Generate end of while statement
 *
 * @param gen Generator instance
 */
void gen_while_end(gen_t* gen, int construct_count);

/**
 * @brief Generate function declaration
 *
 * @param gen Generator instance
 * @param token Token with function name
 */
void gen_function(gen_t* gen, token_t* token);

/**
 * @brief Generate function end
 *
 * @param gen Generator instance
 * @param function Pointer to function data in table
 */
void gen_function_end(gen_t* gen, htab_fun_t* function);

/**
 * @brief Generate function call
 *
 * @param gen Generator instance
 * @param in_function Whether are we in function scope
 */
void gen_function_call(gen_t* gen, bool in_function);

/**
 * @brief Generate function call frame
 *
 * @param gen Generator instance
 * @param token Token with function name
 */
void gen_function_call_frame(gen_t* gen, token_t* token);

/**
 * @brief Generate one function call parameter
 *
 * @param gen Generator instance
 * @param token Token with variable name
 * @param in_function Whether are we in function scope
 */
void gen_function_call_param(gen_t* gen, token_t* token, bool in_function);

/**
 * @brief Generate variable definition (e.g. $var =)
 *
 * @param gen Generator instance
 * @param token Token with variable name
 * @param in_function Whether are we in function scope
 */
void gen_variable_def(gen_t* gen, token_t* token, bool in_function);

/**
 * @brief Generate expression from expression tree generated by exp.c
 *
 * @param gen Generator instance
 * @param root Root node
 * @param in_function Wheter are we in function scope
 */
void gen_exp(gen_t* gen, token_term_t* root, bool in_function);

/**
 * @brief Generate return (with value - from expression)
 *
 * @param gen Generator instance
 */
void gen_return(gen_t* gen);

/**
 * @brief Generator return (without value)
 *
 * @param gen Generator instance
 */
void gen_return_void(gen_t* gen);

#endif  // __GEN_H__
