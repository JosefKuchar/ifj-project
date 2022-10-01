#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdbool.h>
#include "gen.h"
#include "scanner.h"
#include "token.h"

typedef struct {
    scanner_t* scanner;  // Scanner instance
    gen_t* gen;          // Generator instance
    token_t token;       // Current token
} parser_t;

typedef struct {
    bool in_loop;  // Whether we're in a loop (e.g. while)
    int exp;
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

bool token_is_type(parser_t *parser, token_type_t type);

void next_token(parser_t *parser);

bool token_check_by_function(parser_t *parser, bool (*check_function)(token_t *));

#endif  // __PARSER_H__
