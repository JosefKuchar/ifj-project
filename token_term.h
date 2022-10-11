#ifndef __TOKEN_TERM_H__
#define __TOKEN_TERM_H__

#include "token.h"
typedef struct {
    token_t token;
    bool terminal;
} token_term_t;


/**
 * @brief Creates new token_term_t with specified token and value of terminal
 *
 * @param token Token to be used
 * @param is_term Value whether token_term_t is or isn't terminal
 * 
 * @return new instance of token_term_t
 */
token_term_t token_term_new(token_t token, bool is_term);

/**
 * @brief Pretty prints token_term_t
 *
 * @param token Token to be printed
 */
void token_term_pprint(token_term_t token);
#endif