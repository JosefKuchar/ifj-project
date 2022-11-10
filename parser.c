/**
 * @file parser.c
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Semantic and syntax parser
 */

#include "parser.h"
#include <stdio.h>
#include "error.h"
#include "exp.h"

void check_variable_exists(parser_t* parser, parser_state_t state) {
    if (parser->token.type == TOK_VAR) {
        htab_pair_t* value =
            htab_find(state.in_function ? parser->local_symtable : parser->global_symtable,
                      parser->token.attr.val_s.val);
        if (value == NULL) {
            error_exit(ERR_SEM_VAR);
        }
    }
}

void next_token_keep(parser_t* parser, parser_state_t state) {
    parser->token = scanner_get_next(parser->scanner);
    check_variable_exists(parser, state);
#ifdef DEBUG_TOK
    token_print(&parser->token);
#endif  // DEBUG_TOK
}

/**
 * @brief Get next token from scanner and save it into parser instance
 * Frees the previous token
 *
 * @param parser Parser instance
 */
void next_token(parser_t* parser) {
    token_free(&parser->token);
    parser->token = scanner_get_next(parser->scanner);
}

/**
 * @brief Check if current token is of given type
 *
 * @param parser Parser instance
 * @param type Checked token
 * @return true if current token is of given type
 */
bool token_is_type(parser_t* parser, token_type_t type) {
    return parser->token.type == type;
}

/**
 * @brief Check if next token is of given type
 *
 * @param parser Parser instance
 * @param type Checked token
 * @return true if next token is of given type
 */
bool next_token_is_type(parser_t* parser, token_type_t type) {
    next_token(parser);
    return token_is_type(parser, type);
}

/**
 * @brief Check if current token is accepted by given function
 *
 * @param parser Parser instance
 * @param check_function Checker function
 * @return true if current token is accepted by given function
 */
bool token_check_by_function(parser_t* parser, bool (*check_function)(token_t*)) {
    return check_function(&parser->token);
}

/**
 * @brief Check if current token is of given type, if not, throw error
 *
 * @param parser Parser instance
 * @param type Token type
 */
void token_check_type(parser_t* parser, token_type_t type) {
    if (!token_is_type(parser, type)) {
#ifdef DEBUG_TOK
        printf("Expected token_type: %s, gotten:\n", token_to_string(type));
        token_print(&parser->token);
#endif  // DEBUG_TOK
        error_exit(ERR_SYN);
    }
}

/**
 * @brief Check if next token is of given type, if not, throw error
 *
 * @param parser Parser instance
 * @param type Token type
 */
void next_token_check_type(parser_t* parser, token_type_t type) {
    next_token(parser);
    token_check_type(parser, type);
}

/**
 * @brief Check if next token is accepted by given function, if not, throw error
 *
 * @param parser  Parser instance
 * @param check_function Checker function
 */
void next_token_check_by_function(parser_t* parser, bool (*check_function)(token_t*)) {
    next_token(parser);
    if (!token_check_by_function(parser, check_function)) {
        error_exit(ERR_SYN);
    }
}

/**
 * @brief Increment construct count both in parser instance and recursive state
 *
 * @param parser Parser instance
 * @param state Recursive state
 */
void increment_construct_count(parser_t* parser, parser_state_t* state) {
    state->construct_count = parser->construct_count;
    parser->construct_count++;
}

parser_t parser_new(scanner_t* scanner, gen_t* gen) {
    parser_t parser = {.scanner = scanner,
                       .gen = gen,
                       .local_symtable = htab_new(),
                       .global_symtable = htab_new()};

    // Define buildin functions
    htab_define_buildin(parser.global_symtable);

    return parser;
}

void parser_free(parser_t* parser) {
    htab_free(parser->local_symtable);
    htab_free(parser->global_symtable);
}

void rule_additional_param(parser_t* parser, parser_state_t state) {
    (void)state;
    if (next_token_is_type(parser, TOK_COMMA)) {                         // ,
        next_token_check_by_function(parser, token_is_datatype);         // type
        htab_function_add_param(parser->function, &parser->token);       //
        next_token_check_type(parser, TOK_VAR);                          // $var
        htab_function_add_param_name(parser->function, &parser->token);  //
        /* TODO: Types! */                                               //
        htab_add_variable(parser->local_symtable, &parser->token);       //
        rule_additional_param(parser, state);                            // <additional_param>
    }
}

void rule_param(parser_t* parser, parser_state_t state) {
    (void)state;
    next_token(parser);
    if (token_is_datatype(&parser->token)) {                             // type
        htab_function_add_param(parser->function, &parser->token);       //
        next_token_check_type(parser, TOK_VAR);                          // $var
        htab_function_add_param_name(parser->function, &parser->token);  //
        /* TODO: Types! */                                               //
        htab_add_variable(parser->local_symtable, &parser->token);       //
        rule_additional_param(parser, state);                            // <additional_param>
    }
}

void rule_additional_call_param(parser_t* parser, parser_state_t state) {
    (void)state;
    if (next_token_is_type(parser, TOK_COMMA)) {                     // ,
        next_token(parser);                                          //
        if (!token_is_type(parser, TOK_VAR) &&                       // $var | literal
            !token_is_literal(&parser->token)) {                     //
            error_exit(ERR_SYN);                                     //
        }                                                            //
        check_variable_exists(parser, state);                        //
        gen_function_call_param(parser->gen,                         //
                                &parser->token, state.in_function);  //
        rule_additional_call_param(parser, state);                   // <additional_call_param>
    }
}

void rule_call_param(parser_t* parser, parser_state_t state) {
    (void)state;
    next_token(parser);
    if (token_is_type(parser, TOK_VAR) ||                            // $var | literal
        token_is_literal(&parser->token)) {                          //
        check_variable_exists(parser, state);                        //
        gen_function_call_param(parser->gen,                         //
                                &parser->token, state.in_function);  //
        rule_additional_call_param(parser, state);                   // <additional_call_param>
    }
}

void rule_function_call(parser_t* parser, parser_state_t state) {
    (void)state;
    parser->function_call = htab_add_function(parser->global_symtable, &parser->token, false);
    gen_function_call_frame(parser->gen, &parser->token);
    next_token_check_type(parser, TOK_LPAREN);          // (
    rule_call_param(parser, state);                     // <call_param>
    token_check_type(parser, TOK_RPAREN);               // )
    next_token_check_type(parser, TOK_SEMICOLON);       // ;
    gen_function_call(parser->gen, state.in_function);  //
}

void rule_value(parser_t* parser, parser_state_t state) {
    if (token_is_expression(&parser->token)) {         //
        rule_exp(parser, state);                       // <exp>
        token_check_type(parser, TOK_SEMICOLON);       // ;
    } else if (token_is_type(parser, TOK_FUN_NAME)) {  //
        rule_function_call(parser, state);             // <function_call>
    } else {                                           //
        error_exit(ERR_SYN);                           //
    }
}

void rule_statement(parser_t* parser, parser_state_t state) {
    (void)state;
    switch (parser->token.type) {
        case TOK_VAR:                                                              // $var
            str_clear(&parser->gen->variable);                                     //
            str_add_str(&parser->gen->variable, &parser->token.attr.val_s);        //
            if (htab_add_variable(                                                 //
                    state.in_function ?                                            //
                        parser->local_symtable                                     //
                                      : parser->global_symtable,                   //
                    &parser->token)) {                                             //
                gen_variable_def(parser->gen, &parser->token, state.in_function);  //
            }                                                                      //
            next_token_check_type(parser, TOK_ASSIGN);                             // =
            next_token(parser);                                                    //
            rule_value(parser, state);                                             // <value>
            token_check_type(parser, TOK_SEMICOLON);                               // ;

            break;
        case TOK_IF:                                              // if
            str_clear(&parser->gen->variable);                    //
            increment_construct_count(parser, &state);            //
            next_token_check_type(parser, TOK_LPAREN);            // (
            next_token(parser);                                   //
            rule_exp(parser, state);                              // <exp>
            gen_if(parser->gen, state.construct_count);           //
            token_check_type(parser, TOK_RPAREN);                 // )
            next_token_check_type(parser, TOK_LBRACE);            // {
            next_token(parser);                                   //
            rule_statement(parser, state);                        // <statement>
            token_check_type(parser, TOK_RBRACE);                 // }
            next_token_check_type(parser, TOK_ELSE);              // else
            gen_else(parser->gen, state.construct_count);         //
            next_token_check_type(parser, TOK_LBRACE);            // {
            next_token(parser);                                   //
            rule_statement(parser, state);                        // <statement>
            token_check_type(parser, TOK_RBRACE);                 // }
            gen_if_else_end(parser->gen, state.construct_count);  //
            break;
        case TOK_WHILE:                                          // while
            str_clear(&parser->gen->variable);                   //
            increment_construct_count(parser, &state);           //
            gen_while(parser->gen, state.construct_count);       //
            next_token_check_type(parser, TOK_LPAREN);           // (
            next_token(parser);                                  //
            rule_exp(parser, state);                             // <exp>
            gen_while_exit(parser->gen, state.construct_count);  //
            token_check_type(parser, TOK_RPAREN);                // )
            next_token_check_type(parser, TOK_LBRACE);           // {
            next_token(parser);                                  //
            rule_statement(parser, state);                       // <statement>
            token_check_type(parser, TOK_RBRACE);                // }
            gen_while_end(parser->gen, state.construct_count);   //
            break;
        case TOK_FUN_NAME:                                        // function_name
            str_clear(&parser->gen->variable);                    //
            rule_function_call(parser, state);                    // <function_call>
            break;                                                //
        case TOK_RETURN:                                          // return
            str_clear(&parser->gen->variable);                    //
            next_token(parser);                                   //
            if (parser->token.type == TOK_SEMICOLON) {            //
                gen_return_void(parser->gen, state.in_function);  // [;]
                break;                                            //
            }                                                     //
            rule_value(parser, state);                            // <value>
            gen_return(parser->gen, state.in_function);           //
            break;
        default:
            if (token_is_expression(&parser->token)) {    //
                rule_exp(parser, state);                  // <exp>
                token_check_type(parser, TOK_SEMICOLON);  // ;
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
    next_token_check_type(parser, TOK_FUN_NAME);                       // function_name
    parser->function = htab_add_function(parser->global_symtable,      //
                                         &parser->token, true);        //
    gen_function(parser->gen, &parser->token);                         //
    next_token_check_type(parser, TOK_LPAREN);                         // (
    rule_param(parser, state);                                         // <param>
    token_check_type(parser, TOK_RPAREN);                              // )
    next_token_check_type(parser, TOK_COLON);                          // :
    next_token_check_by_function(parser, token_is_returntype);         // type
    next_token_check_type(parser, TOK_LBRACE);                         // {
    next_token(parser);                                                //
    rule_statement(parser, state);                                     // <statement>
    token_check_type(parser, TOK_RBRACE);                              // }
    gen_function_end(parser->gen, &parser->function->value.function);  //
    htab_clear(parser->local_symtable);                                //
}

void rule_program(parser_t* parser, parser_state_t state) {
    switch (parser->token.type) {
        case TOK_FUNCTION:
            rule_function(parser, state);
            next_token(parser);
            break;
        case TOK_EOF:
            return;
        default:
            rule_statement(parser, state);
            break;
    }
    rule_program(parser, state);
}

void check_prolog(parser_t* parser) {
    // TODO Check error codes
    next_token_check_type(parser, TOK_FUN_NAME);                      // declare
    if (strcmp(parser->token.attr.val_s.val, "declare") != 0) {       //
        error_exit(ERR_SYN);                                          //
    }                                                                 //
    next_token_check_type(parser, TOK_LPAREN);                        // (
    next_token_check_type(parser, TOK_FUN_NAME);                      // strict_types
    if (strcmp(parser->token.attr.val_s.val, "strict_types") != 0) {  //
        error_exit(ERR_SYN);                                          //
    }                                                                 //
    next_token_check_type(parser, TOK_ASSIGN);                        // =
    next_token_check_type(parser, TOK_INT_LIT);                       // 1
    if (parser->token.attr.val_i != 1) {                              //
        error_exit(ERR_SYN);                                          //
    }                                                                 //
    next_token_check_type(parser, TOK_RPAREN);                        // )
    next_token_check_type(parser, TOK_SEMICOLON);                     // ;
}

void parser_run(parser_t* parser) {
#ifndef DEBUG_LEX
    parser_state_t state = {
        .in_loop = false,
        .in_function = false,
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
