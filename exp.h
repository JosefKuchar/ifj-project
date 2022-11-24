/**
 * Implementace překladače imperativního jazyka IFJ22
 *
 * @file exp.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Header file for syntax analysis of expressions
 */

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

#endif  // __EXP_H__
