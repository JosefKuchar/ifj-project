/**
 * Implementace překladače imperativního jazyka IFJ22
 *
 * @file main.c
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Entry point of IFJ22 compiler
 */

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
