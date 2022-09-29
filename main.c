#include <stdio.h>
#include "error.h"
#include "parser.h"
#include "scanner.h"

int main() {
    scanner_t scanner = scanner_new();
    parser_t parser = parser_new(&scanner);
    parser_run(&parser);
    parser_free(&parser);
    scanner_free(&scanner);
    return RET_OK;
}
