#ifndef __TOKEN_TERM_H__
#define __TOKEN_TERM_H__

#include "token.h"
typedef struct {
    token_t token;
    bool terminal;
} token_term_t;

token_term_t token_term_new(token_t token, bool is_term);

void token_term_pprint(token_term_t token);
#endif