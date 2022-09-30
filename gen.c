#include "gen.h"

gen_t gen_new() {
    return (gen_t){.start = str_new()};
}

void gen_header(gen_t* gen) {
    str_add_cstr(&gen->start, ".IFJcode22\n");
}

void gen_free(gen_t* gen) {
    str_free(&gen->start);
}

void gen_emit(gen_t* gen) {
    str_print(&gen->start);
}
