#ifndef __GEN_H__
#define __GEN_H__

#include "str.h"
#include "token.h"

typedef struct {
    str_t start;
} gen_t;

gen_t gen_new();

void gen_emit(gen_t* gen);

void gen_header(gen_t* gen);

void gen_free();

/**
 * @brief Generate if statement
 *
 * @param gen Generator instance
 */
void gen_if(gen_t* gen, int construct_count);

/**
 * @brief Generate else statement (+ jump to end of if)
 *
 * @param gen Generator instance
 */
void gen_else(gen_t* gen, int construct_count);

/**
 * @brief Generate end of if-else statement
 *
 * @param gen Generator instance
 */
void gen_if_else_end(gen_t* gen, int construct_count);

/**
 * @brief Generate while statement
 *
 * @param gen Generator instance
 */
void gen_while(gen_t* gen, int construct_count);

/**
 * @brief Generate end of while statement
 *
 * @param gen Generator instance
 */
void gen_while_end(gen_t* gen, int construct_count);

void gen_function(gen_t* gen, token_t* token);

void gen_function_end(gen_t* gen);

void gen_function_call(gen_t* gen, token_t* token);

#endif  // __GEN_H__
