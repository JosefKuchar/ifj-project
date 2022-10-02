#ifndef __EXP_H__
#define __EXP_H__

#include "parser.h"

/**
 * @brief Implements expression parsing
 *
 * @param parser Parser instance
 * @param state Current state
 */
void rule_exp(parser_t* parser, parser_state_t state);

#endif  // __EXP_H__
