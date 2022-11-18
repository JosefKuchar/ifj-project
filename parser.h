/**
 * @file parser.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Header file for semantic and syntax parser
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdbool.h>
#include "gen.h"
#include "scanner.h"
#include "symtable.h"
#include "token.h"

typedef struct {
    scanner_t* scanner;          // Scanner instance
    gen_t* gen;                  // Generator instance
    token_t token;               // Current token
    htab_t* local_symtable;      // Local symbol table
    htab_t* global_symtable;     // Global symbol table
    htab_pair_t* function;       // Current function
    htab_pair_t* function_call;  // Current function call
    int construct_count;         // Counter of if/else/while constructs (for generator)
    int param_count;             // Counter of function call params (for checking)
    bool buffer_token_valid;     // Is old token valid?
    token_t buffer_token;        // Old token
} parser_t;

typedef struct {
    bool in_loop;      // Whether we're in a loop (e.g. while)
    bool in_function;  // Whether we're in a function
    int construct_count;
} parser_state_t;

/**
 * @brief Initialize parser
 *
 * @param scanner Scanner instance
 * @return Initialized parser
 */
parser_t parser_new(scanner_t* scanner, gen_t* gen);

/**
 * @brief Free exising parser
 *
 * @param state Parser state
 */
void parser_free(parser_t* state);

/**
 * @brief Run parser
 *
 */
void parser_run(parser_t* state);

/**
 * @brief Asks scanner for next token and keeps the previous one
 *
 * @param parser Instance of parser
 */
void next_token_keep(parser_t* parser);

#endif  // __PARSER_H__
