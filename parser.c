#include "parser.h"
#include <stdio.h>
#include "error.h"
#include "exp.h"

void skip_next(parser_t* parser) {
    parser->skip_next = true;
}

void next_token(parser_t* parser) {
    if (parser->skip_next) {
        parser->skip_next = false;
        return;
    }

    token_free(&parser->token);
    parser->token = scanner_get_next(parser->scanner);
#ifdef DEBUG_TOK
    token_print(&parser->token);
#endif  // DEBUG_TOK
}

void next_token_keep(parser_t* parser) {
    if (parser->skip_next) {
        parser->skip_next = false;
        return;
    }

    parser->token = scanner_get_next(parser->scanner);
#ifdef DEBUG_TOK
    token_print(&parser->token);
#endif  // DEBUG_TOK
}

bool token_is_type(parser_t* parser, token_type_t type) {
    return parser->token.type == type;
}

bool next_token_is_type(parser_t* parser, token_type_t type) {
    next_token(parser);
    return token_is_type(parser, type);
}

bool token_check_by_function(parser_t* parser, bool (*check_function)(token_t*)) {
    return check_function(&parser->token);
}

void token_check_type(parser_t* parser, token_type_t type) {
    if (!token_is_type(parser, type)) {
#ifdef DEBUG_TOK
        printf("Expected token_type: %s, gotten:\n", token_to_string(type));
        token_print(&parser->token);
#endif  // DEBUG_TOK
        error_exit(ERR_SYN);
    }
}

void next_token_check_type(parser_t* parser, token_type_t type) {
    next_token(parser);
    token_check_type(parser, type);
}

void next_token_check_by_function(parser_t* parser, bool (*check_function)(token_t*)) {
    next_token(parser);
    if (!token_check_by_function(parser, check_function)) {
        error_exit(ERR_SYN);
    }
}

void increment_construct_count(parser_t* parser, parser_state_t* state) {
    state->construct_count = parser->construct_count;
    parser->construct_count++;
}

parser_t parser_new(scanner_t* scanner, gen_t* gen) {
    return (parser_t){.scanner = scanner,
                      .gen = gen,
                      .local_symtable = htab_new(),
                      .global_symtable = htab_new()};
}

void parser_free(parser_t* parser) {
    htab_free(parser->local_symtable);
    htab_free(parser->global_symtable);
}

void rule_program(parser_t* parser, parser_state_t state);

void rule_additional_param(parser_t* parser, parser_state_t state) {
    (void)state;
    if (next_token_is_type(parser, TOK_COMMA)) {
        next_token_check_by_function(parser, token_is_datatype);
        htab_function_add_param(parser->function, &parser->token);
        next_token_check_type(parser, TOK_VAR);
        htab_function_add_param_name(parser->function, &parser->token);
        rule_additional_param(parser, state);
    }
}

void rule_param(parser_t* parser, parser_state_t state) {
    (void)state;
    next_token(parser);
    if (token_is_datatype(&parser->token)) {
        htab_function_add_param(parser->function, &parser->token);
        next_token_check_type(parser, TOK_VAR);
        htab_function_add_param_name(parser->function, &parser->token);
        rule_additional_param(parser, state);
    }
}

void rule_additional_call_param(parser_t* parser, parser_state_t state) {
    (void)state;
    if (next_token_is_type(parser, TOK_COMMA)) {
        next_token(parser);
        if (!token_is_type(parser, TOK_VAR) && !token_is_literal(&parser->token)) {
            error_exit(ERR_SYN);
        }
        gen_function_call_param(parser->gen, &parser->token);
        rule_additional_call_param(parser, state);
    }
}

void rule_call_param(parser_t* parser, parser_state_t state) {
    (void)state;
    next_token(parser);
    if (token_is_type(parser, TOK_VAR) || token_is_literal(&parser->token)) {
        gen_function_call_param(parser->gen, &parser->token);
        rule_additional_call_param(parser, state);
    }
}

void rule_function_call(parser_t* parser, parser_state_t state) {
    (void)state;
    parser->function = htab_add_function(parser->global_symtable, &parser->token, false);
    gen_function_call_frame(parser->gen, &parser->token);
    next_token_check_type(parser, TOK_LPAREN);
    rule_call_param(parser, state);
    token_check_type(parser, TOK_RPAREN);
    next_token_check_type(parser, TOK_SEMICOLON);
    gen_function_call(parser->gen);
}

void rule_value(parser_t* parser, parser_state_t state) {
    next_token(parser);
    if (token_is_expression(&parser->token)) {
        rule_exp(parser, state);
        token_check_type(parser, TOK_SEMICOLON);
    } else if (token_is_type(parser, TOK_FUN_NAME)) {
        rule_function_call(parser, state);
    } else {
        error_exit(ERR_SYN);
    }
}

void rule_statement(parser_t* parser, parser_state_t state) {
    (void)state;
    switch (parser->token.type) {
        case TOK_VAR:
            str_clear(&parser->gen->variable);
            str_add_str(&parser->gen->variable, &parser->token.attr.val_s);
            if (htab_add_variable(
                    state.in_function ? parser->local_symtable : parser->global_symtable,
                    &parser->token)) {
                gen_variable_def(parser->gen, &parser->token, state.in_function);
            }
            next_token_check_type(parser, TOK_ASSIGN);
            rule_value(parser, state);
            token_check_type(parser, TOK_SEMICOLON);

            break;
        case TOK_IF:
            str_clear(&parser->gen->variable);
            str_add_cstr(&parser->gen->variable, "_tmp1");
            increment_construct_count(parser, &state);
            next_token_check_type(parser, TOK_LPAREN);
            next_token(parser);
            rule_exp(parser, state);
            gen_if(parser->gen, state.construct_count);
            token_check_type(parser, TOK_RPAREN);
            next_token_check_type(parser, TOK_LBRACE);
            next_token(parser);
            rule_statement(parser, state);
            token_check_type(parser, TOK_RBRACE);
            next_token_check_type(parser, TOK_ELSE);
            gen_else(parser->gen, state.construct_count);
            next_token_check_type(parser, TOK_LBRACE);
            next_token(parser);
            rule_statement(parser, state);
            token_check_type(parser, TOK_RBRACE);
            gen_if_else_end(parser->gen, state.construct_count);
            break;
        case TOK_WHILE:
            str_clear(&parser->gen->variable);
            str_add_cstr(&parser->gen->variable, "_tmp1");
            increment_construct_count(parser, &state);
            gen_while(parser->gen, state.construct_count);
            next_token_check_type(parser, TOK_LPAREN);
            next_token(parser);
            rule_exp(parser, state);
            gen_while_exit(parser->gen, state.construct_count);
            token_check_type(parser, TOK_RPAREN);
            next_token_check_type(parser, TOK_LBRACE);
            next_token(parser);
            rule_statement(parser, state);
            token_check_type(parser, TOK_RBRACE);
            gen_while_end(parser->gen, state.construct_count);
            break;
        case TOK_FUN_NAME:
            rule_function_call(parser, state);
            break;
        case TOK_RETURN:
            rule_value(parser, state);
            break;
        default:
            if (token_is_expression(&parser->token)) {
                rule_exp(parser, state);
                token_check_type(parser, TOK_SEMICOLON);
            } else {
                return;
            }
            break;
    }
    next_token(parser);
    rule_statement(parser, state);
}

void rule_function(parser_t* parser, parser_state_t state) {
    state.in_function = true;
    next_token_check_type(parser, TOK_FUN_NAME);
    parser->function = htab_add_function(parser->global_symtable, &parser->token, true);
    gen_function(parser->gen, &parser->token);
    next_token_check_type(parser, TOK_LPAREN);
    rule_param(parser, state);
    token_check_type(parser, TOK_RPAREN);
    next_token_check_type(parser, TOK_COLON);
    next_token_check_by_function(parser, token_is_datatype);
    next_token_check_type(parser, TOK_LBRACE);
    next_token(parser);
    rule_statement(parser, state);
    token_check_type(parser, TOK_RBRACE);
    gen_function_end(parser->gen);
    htab_clear(parser->local_symtable);
}

void rule_program(parser_t* parser, parser_state_t state) {
    switch (parser->token.type) {
        case TOK_FUNCTION:
            rule_function(parser, state);
            break;
        case TOK_EOF:
            return;
        default:
            rule_statement(parser, state);
            skip_next(parser);
            break;
    }
    next_token(parser);
    rule_program(parser, state);
}

void check_prolog(parser_t* parser) {
    // TODO Check error codes
    next_token_check_type(parser, TOK_FUN_NAME);
    if (strcmp(parser->token.attr.val_s.val, "declare") != 0) {
        error_exit(ERR_SYN);
    }
    next_token_check_type(parser, TOK_LPAREN);
    next_token_check_type(parser, TOK_FUN_NAME);
    if (strcmp(parser->token.attr.val_s.val, "strict_types") != 0) {
        error_exit(ERR_SYN);
    }
    next_token_check_type(parser, TOK_ASSIGN);
    next_token_check_type(parser, TOK_INT_LIT);
    if (parser->token.attr.val_i != 1) {
        error_exit(ERR_SYN);
    }
    next_token_check_type(parser, TOK_RPAREN);
    next_token_check_type(parser, TOK_SEMICOLON);
}

void parser_run(parser_t* parser) {
#ifndef DEBUG_LEX
    parser_state_t state = {
        .in_loop = false,
        .in_function = false,
        .exp = 0,
    };

    gen_header(parser->gen);
    check_prolog(parser);
    next_token(parser);
    rule_program(parser, state);
    gen_footer(parser->gen);
#endif
#ifdef DEBUG_LEX
    next_token(parser);
    while (parser->token.type != TOK_EOF) {
        next_token(parser);
    }
#endif
}
