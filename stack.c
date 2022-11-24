/**
 * Implementace překladače imperativního jazyka IFJ22
 *
 * @file stack.c
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Implementation of helper functions for working with stack
 */

#include "stack.h"
#include <stdio.h>
#include "error.h"
#include "token.h"

#define DEFAULT_SIZE 8

stack_t stack_new() {
    // Create new struct
    stack_t stack = {
        .tokens = malloc(DEFAULT_SIZE * sizeof(token_term_t*)),
        .size = DEFAULT_SIZE,
        .len = 0,
    };

    // Check if malloc failed
    if (stack.tokens == NULL) {
        error_exit(ERR_INTERNAL);
    }

    return stack;
}

void stack_free(stack_t* stack) {
    // Free the stack
    for (int i = 0; i < stack->len; i++) {
        token_term_free(stack->tokens[i]);
        stack->tokens[i] = NULL;
    }
    free(stack->tokens);
    // Reset values just to be sure
    stack->tokens = NULL;
    stack->len = 0;
    stack->size = 0;
}

void stack_empty(stack_t* stack) {
    // Just set length to 0 (this is correct)
    stack->len = 0;
}

void resize_stack(stack_t* stack) {
    token_term_t** new_tokens = realloc(stack->tokens, stack->size * 2 * sizeof(token_term_t*));
    if (new_tokens == NULL) {
        error_exit(ERR_INTERNAL);
    }
    stack->tokens = new_tokens;
    stack->size *= 2;
}

void stack_push(stack_t* stack, token_term_t* token) {
    // Enlarge buffer if needed
    if (stack->len + 1 >= stack->size) {
        resize_stack(stack);
    }

    // Add token to the stack
    stack->tokens[stack->len] = token;
    stack->len++;
}

token_term_t* stack_pop(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_SYN);
    }

    // Get token from the stack
    token_term_t* token = stack->tokens[stack->len - 1];
    stack->len--;

    return token;
}

token_term_t* stack_top(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_SYN);
    }

    // Get token from the stack
    token_term_t* token = stack->tokens[stack->len - 1];

    return token;
}

token_term_t* stack_pop_terminal(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_SYN);
    }
    token_term_t* token = NULL;

    for (int i = 1; i <= stack->len; i++) {
        token = stack->tokens[stack->len - i];
        if (token->terminal) {
            stack->len--;
            return token;
        }
    }
    error_exit(ERR_SYN);
    return token;
}

token_term_t* stack_top_terminal(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_SYN);
    }
    token_term_t* token = NULL;

    for (int i = 1; i <= stack->len; i++) {
        token = stack->tokens[stack->len - i];
        if (token->terminal) {
            return token;
        }
    }
    error_exit(ERR_SYN);
    return token;
}

void stack_push_after_terminal(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_SYN);
    }
    token_term_t* token;

    for (int i = 1; i <= stack->len; i++) {
        token = stack->tokens[stack->len - i];
        if (token->terminal) {
            if (stack->len + 1 >= stack->size) {
                resize_stack(stack);
            }
            for (int j = 1; j < i + 1; j++) {
                stack->tokens[stack->len - j + 1] = stack->tokens[stack->len - j];
            }
            stack->tokens[stack->len - i + 1] =
                token_term_new(token_new(TOK_HANDLE_START, 0, 0), false);
            stack->len++;

            return;
        }
    }
    error_exit(ERR_SYN);
}

void stack_pprint(stack_t* stack) {
    if (stack->len == 0) {
        fprintf(stderr, "empty stack\n");
        return;
    }
    for (int i = 0; i < stack->len; i++) {
        fprintf(stderr, "%d[%d]: %s\n", i, stack->tokens[i]->terminal,
                token_to_string(stack->tokens[i]->value.type));
    }
}
