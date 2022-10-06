#include "token_term.h"
#include <stdio.h>

token_term_t token_term_new(token_t token, bool is_term) {
    return (token_term_t){.token = token, .terminal = is_term};
}

void token_term_pprint(token_term_t token) {
    printf("%s[%d]\n", token_to_string(token.token.type), token.terminal);
}