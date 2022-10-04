#ifndef __EXP_H__
#define __EXP_H__

#include "parser.h"
#include "stack.h"
#include "token_term.h"

/**
 * @brief Implements expression parsing
 *
 * @param parser Parser instance
 * @param state Current state
 */
void rule_exp(parser_t* parser, parser_state_t state);

token_term_t _parse_expression(stack_t* stack);

token_term_t parse_paren(stack_t* stack);

token_term_t parse_arithmetic(stack_t* stack);

token_term_t parse_concat(stack_t* stack);

token_term_t parse_comparison(stack_t* stack);
#endif  // __EXP_H__
