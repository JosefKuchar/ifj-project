#include "exp.h"
#include <stdio.h>
#include "error.h"
#include "parser.h"
#include "token.h"

#define TABLE_SIZE 18

enum {
    L,  // <
    R,  // >
    E,  // =
    X   // Invalid
};

const int precedence_table[TABLE_SIZE][TABLE_SIZE] = {
    // only ==/!== for formatting reasons
    /*+ -  *  /  <  <= >  >= == != (  )  ID IN FL ST .  $ */
    {R, R, L, L, R, R, R, R, R, R, L, R, L, L, L, X, R, R},  // +
    {R, R, L, L, R, R, R, R, R, R, L, R, L, L, L, X, R, R},  // -
    {R, R, R, R, R, R, R, R, R, R, L, R, L, L, L, X, R, R},  // *
    {R, R, R, R, R, R, R, R, R, R, L, R, L, L, L, X, R, R},  // /
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, R},  // <
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, R},  // <=
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, R},  // >
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, R},  // >=
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, R},  // ==
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, R},  // !=
    {L, L, L, L, L, L, L, L, L, L, L, E, L, L, L, L, L, R},  // (
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, R},  // )
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, R},  // ID
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, R},  // IN
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, R},  // FL
    {X, X, X, X, R, R, R, R, R, R, X, R, X, X, X, X, R, R},  // ST
    {R, R, L, L, R, R, R, R, R, R, L, R, L, L, L, L, R, R},  // .
    {L, L, L, L, L, L, L, L, L, L, L, X, L, L, L, L, L, R},  // $
};

void mock_exp(parser_t* parser, parser_state_t state);

void rule_exp(parser_t* parser, parser_state_t state) {
    if (token_is_type(parser, TOK_LPAREN)) {
        state.exp++;
    } else if (token_is_type(parser, TOK_RPAREN)) {
        state.exp--;
        if (state.exp < 0) {
            return;
        }
    } else if (token_is_type(parser, TOK_PLUS) || token_is_type(parser, TOK_MINUS) ||
               token_is_type(parser, TOK_DOT) || token_is_type(parser, TOK_MULTIPLY) ||
               token_is_type(parser, TOK_DIVIDE) || token_is_type(parser, TOK_LESS) ||
               token_is_type(parser, TOK_GREATER) || token_is_type(parser, TOK_LESS_E) ||
               token_is_type(parser, TOK_GREATER_E) || token_is_type(parser, TOK_EQUALS) ||
               token_is_type(parser, TOK_NEQUALS) || token_is_type(parser, TOK_VAR) ||
               token_is_type(parser, TOK_NULL) ||
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
