#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdbool.h>
#include "scanner.h"
#include "token.h"

typedef struct {
    scanner_t* scanner;  // Scanner instance
    token_t token;       // Current token
} parser_t;

typedef struct {
    bool in_loop;  // Whether we're in a loop (e.g. while)
} parser_state_t;

/**
 * @brief Initialize parser
 *
 * @param scanner Scanner instance
 * @return Initialized parser
 */
parser_t parser_new(scanner_t* scanner);

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

#endif  // __PARSER_H__
