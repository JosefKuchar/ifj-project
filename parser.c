#include "parser.h"
#include <stdio.h>
#include "error.h"

void next_token(parser_t* parser) {
    parser->token = scanner_get_next(parser->scanner);
#ifdef DEBUG_TOK
    token_print(&parser->token);
#endif  // DEBUG_TOK
}

void next_token_check_type(parser_t* parser, token_type_t type) {
    next_token(parser);
    if (parser->token.type != type) {
        error_exit(ERR_SYN);
    }
}

void next_token_check_by_function(parser_t* parser, bool (*check_function)(token_t*)) {
    next_token(parser);
    if (!check_function(&parser->token)) {
        error_exit(ERR_SYN);
    }
}

parser_t parser_new(scanner_t* scanner, gen_t* gen) {
    return (parser_t){.scanner = scanner, .gen = gen};
}

void parser_free(parser_t* parser) {
    // Empty for now
    (void)parser;
}

void rule_function(parser_t* parser, parser_state_t state) {
    (void)state;
    next_token_check_type(parser, TOK_FUN_NAME);
    next_token_check_type(parser, TOK_LPAREN);
    next_token_check_type(parser, TOK_RPAREN);
    next_token_check_type(parser, TOK_COLON);
    next_token_check_by_function(parser, token_is_datatype);
    next_token_check_type(parser, TOK_LBRACE);
    next_token_check_type(parser, TOK_RBRACE);
}

void rule_program(parser_t* parser, parser_state_t state) {
    next_token(parser);
    switch (parser->token.type) {
        case TOK_FUNCTION:
            rule_function(parser, state);
            break;
        case TOK_EOF:
            return;
        default:
            error_exit(ERR_SYN);
    }
    rule_program(parser, state);
}

void parser_run(parser_t* parser) {
#ifndef DEBUG_LEX
    parser_state_t state = {
        .in_loop = false,
    };

    gen_header(parser->gen);
    rule_program(parser, state);
#endif
#ifdef DEBUG_LEX
    next_token(parser);
    while (parser->token.type != TOK_EOF) {
        next_token(parser);
    }
#endif
}
