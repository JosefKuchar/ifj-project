#ifndef __STACK_H__
#define __STACK_H__

#include "token_term.h"

typedef struct {
    token_term_t* tokens;
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
void stack_push(stack_t* stack, token_term_t token);

/**
 * @brief Pops top of the stack
 *
 * @param stack to be popped from
 */
token_term_t stack_pop(stack_t* stack);

/**
 * @brief Pops top of the stack
 *
 * @param stack to be popped from
 */
token_term_t stack_top(stack_t* stack);

/**
 * @brief Print stack
 *
 * @param stack to be printed
 */
void stack_pprint(stack_t* stack);

token_term_t stack_pop_terminal(stack_t* stack);

token_term_t stack_top_terminal(stack_t* stack);

void stack_push_after_terminal(stack_t* stack);

void resize_stack(stack_t* stack);
#endif  // __STACK_H__
