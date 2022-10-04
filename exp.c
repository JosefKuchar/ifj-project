#include "exp.h"
#include <stdio.h>
#include "error.h"
#include "parser.h"
#include "stack.h"
#include "token.h"
#include "token_term.h"

#define TABLE_SIZE 19

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
        return -1;
    }
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

token_term_t parse_expression(stack_t* stack) {
    if (stack->len == 3) {
        if (stack->tokens[2].token.type == TOK_EOF && stack->tokens[1].token.type == TOK_PLUS &&
            stack->tokens[0].token.type == TOK_EOF) {
            return token_term_new(token_new(TOK_EXP_END), false);
        }
    }
    return token_term_new(token_new(TOK_EOF), false);
}

void rule_exp(parser_t* parser, parser_state_t state) {
    (void)state;
    stack_t stack = stack_new();
    stack_push(&stack, token_term_new(token_new(TOK_DOLLAR), true));
    while (true) {
        if (token_is_type(parser, TOK_LPAREN)) {
            state.exp++;
        } else if (token_is_type(parser, TOK_RPAREN)) {
            state.exp--;
            if (state.exp < 0) {
                return;
            }
        }
        stack_t current_expression = stack_new();
        int precedence = get_precedence(stack_top_terminal(&stack).token, parser->token);

        if (precedence == -1) {
            precedence = R;
        }
        token_term_t token;

        if (stack_top(&stack).token.type == TOK_EXP_END) {
            return;
        }

        printf("Comparing tokens: %s and %s\n",
               token_to_string(stack_top_terminal(&stack).token.type),
               token_to_string(parser->token.type));
        printf("result is: %d\n", precedence);

        switch (precedence) {
            case L:
                stack_push_after_terminal(&stack);
                stack_push(&stack, token_term_new(parser->token, true));
                next_token_keep(parser);
                break;
            case R:
                stack_pprint(&stack);
                printf("-------------\n");
                while ((token = stack_pop(&stack)).token.type != TOK_HANDLE_START) {
                    if (token.token.type != TOK_HANDLE_START) {
                        stack_push(&current_expression, token);
                    }
                }
                stack_push(&stack, parse_expression(&current_expression));
                break;
            case E:
                stack_push(&stack, token_term_new(parser->token, true));
                next_token_keep(parser);
                break;
            case X:
                error_exit(ERR_SYN);
                break;
            default:
                break;
        }
        // stack_pprint(&stack);
        // printf("-----\n");
        // printf("%d\n", stack.len);
        stack_free(&current_expression);
    }
    return;
}
