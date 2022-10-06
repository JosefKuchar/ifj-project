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

token_term_t parse_paren(stack_t* stack) {
    // stack_pprint(stack);
    if (!(token_is_literal(&stack->tokens[1].token) || stack->tokens[1].token.type == TOK_VAR)) {
        error_exit(ERR_SYN);
    }

    if (stack->tokens[1].terminal) {
        error_exit(ERR_SYN);
    }

    if (stack->tokens[0].token.type != TOK_RPAREN || stack->tokens[2].token.type != TOK_LPAREN) {
        error_exit(ERR_SYN);
    }

    token_type_t type = stack->tokens[1].token.type;
    return token_term_new(token_new(type), false);
}

token_term_t parse_arithmetic(stack_t* stack) {
    if (!(token_is_literal(&stack->tokens[0].token) || stack->tokens[0].token.type == TOK_VAR)) {
        error_exit(ERR_LEX);
    }
    if (!(token_is_literal(&stack->tokens[2].token) || stack->tokens[2].token.type == TOK_VAR)) {
        error_exit(ERR_LEX);
    }
    if (stack->tokens[0].terminal || stack->tokens[2].terminal) {
        error_exit(ERR_SYN);
    }

    token_type_t a = stack->tokens[0].token.type;
    token_type_t b = stack->tokens[2].token.type;
    if (a == TOK_FLOAT_LIT || b == TOK_FLOAT_LIT) {
        if (!type_is_number(a) || !type_is_number(b)) {
            error_exit(ERR_SYN);
        }
        return token_term_new(token_new(TOK_FLOAT_LIT), false);
    }

    if (a == TOK_INT_LIT && b == TOK_INT_LIT) {
        return token_term_new(token_new(TOK_INT_LIT), false);
    }
    error_exit(ERR_SYN);
    return token_term_new(token_new(stack->tokens[0].token.type), false);
}

token_term_t parse_concat(stack_t* stack) {
    token_type_t a = stack->tokens[0].token.type;
    token_type_t b = stack->tokens[2].token.type;

    if (a != TOK_STR_LIT || b != TOK_STR_LIT) {
        error_exit(ERR_SYN);
    }
    if (stack->tokens[0].terminal || stack->tokens[2].terminal) {
        error_exit(ERR_SYN);
    }

    return token_term_new(token_new(TOK_STR_LIT), false);
}

token_term_t parse_comparison(stack_t* stack) {
    if (!(token_is_literal(&stack->tokens[0].token) || stack->tokens[0].token.type == TOK_VAR)) {
        error_exit(ERR_LEX);
    }
    if (!(token_is_literal(&stack->tokens[2].token) || stack->tokens[2].token.type == TOK_VAR)) {
        error_exit(ERR_LEX);
    }
    if (stack->tokens[0].terminal || stack->tokens[2].terminal) {
        error_exit(ERR_SYN);
    }

    return token_term_new(token_new(TOK_BOOL_LIT), false);
}

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

token_term_t parse_expression(stack_t* stack) {
    token_term_t tok = token_term_new(token_new(TOK_EOF), false);
    if (stack->len == 3) {
        int type = stack->tokens[1].token.type;
        if (stack->tokens[2].token.type == TOK_LPAREN) {
            return parse_paren(stack);
        }
        switch (type) {
            case TOK_PLUS:
            case TOK_MINUS:
            case TOK_MULTIPLY:
            case TOK_DIVIDE:
                return parse_arithmetic(stack);
                break;

            case TOK_LESS:
            case TOK_LESS_E:
            case TOK_GREATER:
            case TOK_GREATER_E:
            case TOK_EQUALS:
            case TOK_NEQUALS:
                return parse_comparison(stack);
                break;

            case TOK_DOT:
                return parse_concat(stack);
                break;

            default:

                error_exit(ERR_SYN);
                break;
        }
    } else if (stack->len == 1) {  // Rules for single identifier/single non terminal
        if (token_is_literal(&stack->tokens[0].token) || stack->tokens[0].token.type == TOK_VAR) {
            if (stack->tokens[0].terminal) {
                return token_term_new(token_new(stack->tokens[0].token.type), false);
            } else {
                if (token_is_literal(&stack->tokens[0].token) ||
                    stack->tokens[0].token.type == TOK_VAR) {
                    return token_term_new(token_new(TOK_EXP_END), false);
                }
            }
        }
    }

    error_exit(ERR_SYN);
    return tok;
}

void rule_exp(parser_t* parser, parser_state_t state) {
    (void)state;
    stack_t stack = stack_new();
    stack_push(&stack, token_term_new(token_new(TOK_DOLLAR), true));
    stack_t current_expression = stack_new();

    while (true) {
        // if (token_is_type(parser, TOK_LPAREN)) {
        //     state.exp++;
        // } else if (token_is_type(parser, TOK_RPAREN)) {
        //     state.exp--;
        //     if (state.exp < 0) {  // TODO: Fix this
        //         break;
        //     }
        // }
        int precedence = get_precedence(stack_top_terminal(&stack).token, parser->token);

        if (precedence == -1) {
            precedence = R;
        }
        token_term_t token;

        if (!stack_top(&stack).terminal && stack.len == 2 && precedence == R) {
            printf("Finishing analysis\n");
            printf("Stack is: \n");
            stack_pprint(&stack);
            break;
        }

        switch (precedence) {
            case L:
                stack_push_after_terminal(&stack);
                stack_push(&stack, token_term_new(parser->token, true));
                next_token_keep(parser);
                break;
            case R:
                while ((token = stack_pop(&stack)).token.type != TOK_HANDLE_START) {
                    if (token.token.type != TOK_HANDLE_START) {
                        stack_push(&current_expression, token);
                    } else {
                        token_free(&token.token);
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
        stack_empty(&current_expression);
    }
    stack_free(&current_expression);
    stack_free(&stack);
    return;
}
