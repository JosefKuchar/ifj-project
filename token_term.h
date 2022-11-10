/**
 * @file token_term.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Declaration of token terminal functions
 */

#ifndef __TOKEN_TERM_H__
#define __TOKEN_TERM_H__

#include "token.h"

typedef struct token_term {
    token_t value;             // +
    token_type_t result;       // int
    struct token_term* left;   // 1
    struct token_term* right;  // 2
    bool terminal;             // false
} token_term_t;

/**
 * @brief Creates new token_term_t with specified token and value of terminal
 *
 * @param token Token to be used
 * @param is_term Value whether token_term_t is or isn't terminal
 *
 * @return new instance of token_term_t
 */
token_term_t* token_term_new(token_t value, bool terminal);

/**
 * @brief Pretty prints token_term_t
 *
 * @param token Token to be printed
 */
void token_term_pprint(token_term_t token);

void token_graph_print(token_term_t* token, int depth, int foo);

void token_term_free(token_term_t* root);
#endif