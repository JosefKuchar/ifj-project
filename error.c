#include "error.h"
#include <stdio.h>
#include <stdlib.h>

// Error messages for all return codes
const char* error_msgs[] = {
    [RET_OK] = "",
    [ERR_LEX] = "Lexical error",
    [ERR_SYN] = "Syntax error",
    [ERR_SEM_FUN] = "Semantic error - function (undeclared, redefinition, ...)",
    [ERR_SEM_CALL] = "Semantic error - function call (wrong number of parameters, ...)",
    [ERR_SEM_VAR] = "Semantic error - undefined variable",
    [ERR_SEM_RET] = "Semantic error - return statement",
    [ERR_SEM_COMP] = "Semantic error - incompatible types",
    [ERR_SEM] = "Other semantic error",
    [ERR_INTERNAL] = "Internal error (e.g. memory allocation error)"};

void error_exit(enum return_code code) {
    fprintf(stderr, "%s\n", error_msgs[code]);
    exit(code);
}

void error_not_implemented() {
    fprintf(stderr, "Not implemented!\n");
    exit(ERR_INTERNAL);
}
