#include <stdio.h>
#include "error.h"
#include "gen.h"
#include "parser.h"
#include "scanner.h"

int main() {
    scanner_t scanner = scanner_new(stdin);
    gen_t gen = gen_new();
    parser_t parser = parser_new(&scanner, &gen);
    parser_run(&parser);
    gen_emit(&gen);
    parser_free(&parser);
    gen_free(&gen);
    scanner_free(&scanner);
    return RET_OK;
}
