#include "stack.h"
#include <stdio.h>
#include "error.h"
#include "token.h"

#define DEFAULT_SIZE 8

stack_t stack_new() {
    // Create new struct
    stack_t stack = {
        .tokens = malloc(DEFAULT_SIZE * sizeof(token_term_t)),
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
    free(stack->tokens);
    // Reset values just to be sure
    stack->tokens = NULL;
    stack->len = 0;
    stack->size = 0;
}

void stack_push(stack_t* stack, token_term_t token) {
    // Enlarge buffer if needed
    if (stack->len + 1 >= stack->size) {
        token_term_t* new_tokens = realloc(stack->tokens, stack->size * 2 * sizeof(token_term_t));
        if (new_tokens == NULL) {
            error_exit(ERR_INTERNAL);
        }
        stack->tokens = new_tokens;
        stack->size *= 2;
    }

    // Add token to the stack
    stack->tokens[stack->len] = token;
    stack->len++;
}

token_term_t stack_pop(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_INTERNAL);
    }

    // Get token from the stack
    token_term_t token = stack->tokens[stack->len - 1];
    stack->len--;

    return token;
}

token_term_t stack_top(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_INTERNAL);
    }

    // Get token from the stack
    token_term_t token = stack->tokens[stack->len - 1];

    return token;
}

token_term_t stack_pop_terminal(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_INTERNAL);
    }
    token_term_t token;

    for (int i = 1; token.terminal == false && i < stack->len; i++) {
        token = stack->tokens[stack->len - i];
    }

    stack->len--;

    return token;
}

token_term_t stack_top_terminal(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_INTERNAL);
    }
    token_term_t token;
    bool found = false;

    for (int i = 1; token.terminal == false && i <= stack->len; i++) {
        token = stack->tokens[stack->len - i];
        found = true;
    }
    if (!found) {
        error_exit(ERR_INTERNAL);
    }
    return token;
}

void stack_pprint(stack_t* stack) {
    if (stack->len == 0) {
        printf("empty stackerino\n");
        return;
    }
    for (int i = 0; i < stack->len; i++) {
        printf("%d[%d]: %s\n", i, stack->tokens[i].terminal,
               token_to_string(stack->tokens[i].token.type));
    }
}