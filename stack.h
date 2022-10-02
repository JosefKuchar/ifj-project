#ifndef __STACK_H__
#define __STACK_H__

#include "token.h"

typedef struct {
    token_t* tokens;
    int len;
    int size;
} stack_t;

/**
 * @brief Initialize new stack struct
 *
 * @return New stack_t
 */
stack_t stack_new();

/**
 * @brief Frees stack
 *
 * @param stack to be freed
 */
void stack_free(stack_t* stack);

/**
 * @brief Pushes value on the stack
 *
 * @param stack to be pushed to, token to be pushed
 */
void stack_push(stack_t* stack, token_t token);

/**
 * @brief Pops top of the stack
 *
 * @param stack to be popped from
 */
token_t stack_pop(stack_t* stack);

#endif  // __STACK_H__
