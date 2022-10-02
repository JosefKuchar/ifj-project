#include "stack.h"
#include "error.h"

#define DEFAULT_SIZE 8

stack_t stack_new() {
    // Create new struct
    stack_t stack = {
        .tokens = malloc(DEFAULT_SIZE * sizeof(token_t)),
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

void stack_push(stack_t* stack, token_t token) {
    // Enlarge buffer if needed
    if (stack->len + 1 >= stack->size) {
        token_t* new_tokens = realloc(stack->tokens, stack->size * 2 * sizeof(token_t));
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

token_t stack_pop(stack_t* stack) {
    // Check if stack is empty
    if (stack->len == 0) {
        error_exit(ERR_INTERNAL);
    }

    // Get token from the stack
    token_t token = stack->tokens[stack->len - 1];
    stack->len--;

    return token;
}
