#include "exp.h"
#include <stdio.h>
#include "error.h"
#include "parser.h"
#include "stack.h"
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

int get_precedence(token_t stack_top, token_t input) {
    if (input.type >= TABLE_SIZE || stack_top.type >= TABLE_SIZE) {
        error_exit(ERR_INTERNAL);  // TODO: What error should this be?
    }
    printf("Stack top is : %s\n", token_to_string(stack_top.type));
    printf("parsed token is : %s\n", token_to_string(input.type));
    return precedence_table[stack_top.type][input.type];
}

void rule_exp2(parser_t* parser, parser_state_t state) {
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
    } else {
        if (state.exp != 0) {
            error_exit(ERR_SYN);
        }
        return;
    }
    next_token(parser);
    rule_exp(parser, state);
}

void rule_exp(parser_t* parser, parser_state_t state) {
    (void)state;
    stack_t stack = stack_new();
    stack_push(&stack, token_new(TOK_DOLLAR));

    while (token_is_type(parser, TOK_PLUS) || token_is_type(parser, TOK_MINUS) ||
           token_is_type(parser, TOK_DOT) || token_is_type(parser, TOK_MULTIPLY) ||
           token_is_type(parser, TOK_DIVIDE) || token_is_type(parser, TOK_LESS) ||
           token_is_type(parser, TOK_GREATER) || token_is_type(parser, TOK_LESS_E) ||
           token_is_type(parser, TOK_GREATER_E) || token_is_type(parser, TOK_EQUALS) ||
           token_is_type(parser, TOK_NEQUALS) || token_is_type(parser, TOK_VAR) ||
           token_is_type(parser, TOK_NULL) || token_check_by_function(parser, token_is_literal) ||
           token_is_type(parser, TOK_LPAREN) || token_is_type(parser, TOK_RPAREN)) {
        int precedence = get_precedence(stack_top(&stack), parser->token);
        token_t token;
        switch (precedence) {
            case L:
                stack_push(&stack, token_new(TOK_HANDLE_START));
                stack_push(&stack, parser->token);
                next_token_keep(parser);
                break;
            case R:
                while ((token=stack_pop(&stack)).type != TOK_DOLLAR);
                // ---------
                stack_push(&stack, token_new(TOK_EOF));
                break;
            case E:
                stack_push(&stack, parser->token);
                next_token_keep(parser);
                break;
            case X:
                error_exit(ERR_SYN);
                break;
            default:
                error_exit(ERR_INTERNAL);
                break;
        }
    }
    printf("The stack len is %d \n", stack.len);
    if (stack.len == 2 && stack.tokens[0].type == TOK_DOLLAR) {
        return;
    } else {
        printf("erroring\n");
        error_exit(ERR_SYN);
    }
}
