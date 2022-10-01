#include "exp.h"
#include "parser.h"
#include "error.h"
#include "token.h"
#include <stdio.h>

void rule_exp(parser_t *parser, parser_state_t state) {
    if (token_is_type(parser, TOK_LPAREN)) {
        state.exp++;
    } else if (token_is_type(parser, TOK_RPAREN)) {
        state.exp--;
        if (state.exp < 0) {
            return;
        }
    } else if (token_is_type(parser, TOK_PLUS) || token_is_type(parser, TOK_MINUS) || token_is_type(parser, TOK_DOT) ||
                token_is_type(parser, TOK_MULTIPLY) || token_is_type(parser, TOK_DIVIDE) ||
                token_is_type(parser, TOK_LESS) || token_is_type(parser, TOK_GREATER) ||
                token_is_type(parser, TOK_LESS_E) || token_is_type(parser, TOK_GREATER_E) ||
                token_is_type(parser, TOK_EQUALS) || token_is_type(parser, TOK_NEQUALS) || token_is_type(parser, TOK_VAR) ||
                token_check_by_function(parser, token_is_literal)) {
        // do nothing
    } else {
        if (state.exp != 0) {
            error_exit(ERR_SYN);
        }
        return;
    }
    next_token(parser);
    rule_exp(parser, state);
}
