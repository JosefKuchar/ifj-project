#ifndef __GEN_H__
#define __GEN_H__

#include "str.h"

typedef struct {
    str_t start;
} gen_t;

gen_t gen_new();

void gen_emit(gen_t* gen);

void gen_header(gen_t* gen);

void gen_free();

#endif  // __GEN_H__
