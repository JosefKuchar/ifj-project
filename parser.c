#include "parser.h"

parser_t parser_new(scanner_t* scanner) {
    return (parser_t){
        .scanner = scanner,
        .in_loop = false,
    };
}

void parser_free(parser_t* state) {
    // Empty for now
    (void)state;
}

void parser_run(parser_t* state) {
    // TODO
    token_t token = scanner_get_next(state->scanner);
    print_token(&token);
    token_free(&token);

    while (token.type != TOK_EOF) {
        token = scanner_get_next(state->scanner);
        print_token(&token);
        token_free(&token);
    }
}
