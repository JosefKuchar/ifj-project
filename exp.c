/**
 * @file exp.c
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Syntax analysis for expressions
 */

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
    X,  // Invalid
};

/**
 * @brief Parses parenthesis
 *
 * @param stack instance of stack containing the expression
 * @return token_term_t result of the expression
 */
token_term_t* parse_paren(stack_t* stack) {
    if (!(type_is_literal(stack->tokens[1]->result) || stack->tokens[1]->result == TOK_VAR)) {
        error_exit(ERR_SYN);
    }

    if (stack->tokens[1]->terminal) {
        error_exit(ERR_SYN);
    }

    if (stack->tokens[0]->value.type != TOK_RPAREN || stack->tokens[2]->result != TOK_LPAREN) {
        error_exit(ERR_SYN);
    }

    token_term_free(stack->tokens[0]);
    token_term_free(stack->tokens[2]);
    return stack->tokens[1];
}

/**
 * @brief Parses arithmetic expression
 *
 * @param stack instance of stack containing the expression
 * @return token_term_t result of the expression
 */
token_term_t* parse_arithmetic(stack_t* stack) {
    token_term_t* new = stack->tokens[1];
    new->terminal = false;
    new->right = stack->tokens[0];
    new->left = stack->tokens[2];

    if (!(type_is_literal(stack->tokens[0]->result) || stack->tokens[0]->result == TOK_VAR)) {
        error_exit(ERR_SYN);
    }
    if (!(type_is_literal(stack->tokens[2]->result) || stack->tokens[2]->result == TOK_VAR)) {
        error_exit(ERR_SYN);
    }
    if (stack->tokens[0]->terminal || stack->tokens[2]->terminal) {
        error_exit(ERR_SYN);
    }

    token_type_t a = stack->tokens[0]->result;
    token_type_t b = stack->tokens[2]->result;

    if (a == TOK_NULL) {
        a = TOK_INT_LIT;
    }
    if (b == TOK_NULL) {
        b = TOK_INT_LIT;
    }

    if (a == TOK_VAR || b == TOK_VAR) {
        new->result = TOK_VAR;
        return new;
    }

    if (a == TOK_FLOAT_LIT || b == TOK_FLOAT_LIT) {
        if (!type_is_number(a) || !type_is_number(b)) {
            error_exit(ERR_SYN);
        }
        new->result = TOK_FLOAT_LIT;
        return new;
    }

    if (a == TOK_INT_LIT && b == TOK_INT_LIT) {
        new->result = TOK_INT_LIT;
        return new;
    }
    error_exit(ERR_SYN);
    return new;
}

/**
 * @brief Parses string concatenation
 *
 * @param stack instance of stack containing the expression
 * @return token_term_t result of the expression
 */
token_term_t* parse_concat(stack_t* stack) {
    token_term_t* new = stack->tokens[1];
    new->terminal = false;
    new->right = stack->tokens[0];
    new->left = stack->tokens[2];
    new->result = TOK_STR_LIT;

    if (stack->tokens[0]->terminal || stack->tokens[2]->terminal) {
        error_exit(ERR_SYN);
    }
    return new;
}

/**
 * @brief Parses comparison
 *
 * @param stack instance of stack containing the expression
 * @return token_term_t result of the expression
 */
token_term_t* parse_comparison(stack_t* stack) {
    token_term_t* new = stack->tokens[1];
    new->terminal = false;
    new->right = stack->tokens[0];
    new->left = stack->tokens[2];
    new->result = TOK_BOOL_LIT;

    if (!(type_is_literal(stack->tokens[0]->result) || stack->tokens[0]->result == TOK_VAR)) {
        error_exit(ERR_SYN);
    }
    if (!(type_is_literal(stack->tokens[2]->result) || stack->tokens[2]->result == TOK_VAR)) {
        error_exit(ERR_SYN);
    }
    if (stack->tokens[0]->terminal || stack->tokens[2]->terminal) {
        error_exit(ERR_SYN);
    }

    return new;
}

const int precedence_table[TABLE_SIZE][TABLE_SIZE] = {
    // =/!= only for formatting reasons
    /*+ -  *  /  <  <= >  >= == != (  )  ID IN FL ST .  NI $ */
    {R, R, L, L, R, R, R, R, R, R, L, R, L, L, L, X, R, L, R},  // +
    {R, R, L, L, R, R, R, R, R, R, L, R, L, L, L, X, R, L, R},  // -
    {R, R, R, R, R, R, R, R, R, R, L, R, L, L, L, X, R, L, R},  // *
    {R, R, R, R, R, R, R, R, R, R, L, R, L, L, L, X, R, L, R},  // /
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, L, R},  // <
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, L, R},  // <=
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, L, R},  // >
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, L, R},  // >=
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, L, R},  // ==
    {L, L, L, L, X, X, X, X, X, X, L, R, L, L, L, L, L, L, R},  // !=
    {L, L, L, L, L, L, L, L, L, L, L, E, L, L, L, L, L, L, R},  // (
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, X, R},  // )
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, X, R},  // ID
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, X, R},  // IN
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, X, R},  // FL
    {X, X, X, X, R, R, R, R, R, R, X, R, X, X, X, X, R, X, R},  // ST
    {R, R, L, L, R, R, R, R, R, R, L, R, L, L, L, L, R, L, R},  // .
    {R, R, R, R, R, R, R, R, R, R, X, R, X, X, X, X, R, X, R},  // NI
    {L, L, L, L, L, L, L, L, L, L, L, X, L, L, L, L, L, L, R},  // $
};

int get_precedence(token_t stack_top, token_t input) {
    if (input.type >= TABLE_SIZE || stack_top.type >= TABLE_SIZE) {
        return R;
    }

    return precedence_table[stack_top.type][input.type];
}

token_term_t* parse_expression(stack_t* stack) {
    if (stack->len == 3) {
        int type = stack->tokens[1]->value.type;
        if (stack->tokens[2]->value.type == TOK_LPAREN) {
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
        if (token_is_literal(&stack->tokens[0]->value) || stack->tokens[0]->value.type == TOK_VAR) {
            if (stack->tokens[0]->terminal) {
                stack->tokens[0]->terminal = false;
                return stack->tokens[0];
            } else {
                error_not_implemented();
                if (token_is_literal(&stack->tokens[0]->value) ||
                    stack->tokens[0]->value.type == TOK_VAR) {
                    return token_term_new(token_new(TOK_EXP_END, 0, 0), false);
                }
            }
        }
    }

    error_exit(ERR_SYN);
    return NULL;
}

void rule_exp(parser_t* parser, parser_state_t state) {
    (void)state;
    stack_t stack = stack_new();
    stack_t current_expression = stack_new();

    stack_push(&stack, token_term_new(token_new(TOK_DOLLAR, 0, 0), true));

    while (true) {
        int precedence = get_precedence(stack_top_terminal(&stack)->value, parser->token);

        token_term_t* token = NULL;

        if (!stack_top(&stack)->terminal && stack.len == 2 &&
            (precedence == R || precedence == X)) {
            gen_exp(parser->gen, stack_top(&stack), state.in_function);
            break;
        }
        switch (precedence) {
            case L:
                stack_push_after_terminal(&stack);
                stack_push(&stack, token_term_new(parser->token, true));
                next_token_keep(parser);
                break;
            case R:
                while ((token = stack_pop(&stack))->value.type != TOK_HANDLE_START) {
                    if (token->value.type != TOK_HANDLE_START) {
                        stack_push(&current_expression, token);
                    } else {
                        token_term_free(token);
                    }
                }
                token_term_free(token);
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
