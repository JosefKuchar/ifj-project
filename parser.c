/**
 * Implementace překladače imperativního jazyka IFJ22
 *
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

void next_token_keep(parser_t* parser) {
    parser->last_default = false;
    if (parser->buffer_token_valid) {
        parser->buffer_token_valid = false;
        parser->token = parser->buffer_token;
    } else {
        parser->token = scanner_get_next(parser->scanner);
    }
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
    parser->last_default = false;
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

void parser_var_to_symtable(parser_t* parser, parser_state_t state) {
    if (htab_add_variable(state.in_function ? parser->local_symtable : parser->global_symtable,
                          &parser->token)) {
        gen_variable_def(parser->gen, &parser->token, state.in_function);
    }
}

parser_t parser_new(scanner_t* scanner, gen_t* gen) {
    parser_t parser = {.scanner = scanner,
                       .gen = gen,
                       .local_symtable = htab_new(),
                       .global_symtable = htab_new(),
                       .buffer_token_valid = false,
                       .last_default = false,
                       .param_count = 0};

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
        if (token_is_type(parser, TOK_VAR)) {                        //
            parser_var_to_symtable(parser, state);                   //
        }                                                            //
        gen_function_call_param(parser->gen,                         //
                                &parser->token, state.in_function);  //
        parser->param_count++;                                       //
        rule_additional_call_param(parser, state);                   // <additional_call_param>
    }
}

void rule_call_param(parser_t* parser, parser_state_t state) {
    (void)state;
    next_token(parser);
    if (token_is_type(parser, TOK_VAR) ||                            // $var | literal
        token_is_literal(&parser->token)) {                          //
        if (token_is_type(parser, TOK_VAR)) {                        //
            parser_var_to_symtable(parser, state);                   //
        }                                                            //
        gen_function_call_param(parser->gen,                         //
                                &parser->token, state.in_function);  //
        parser->param_count++;                                       //
        rule_additional_call_param(parser, state);                   // <additional_call_param>
    }
}

void rule_function_call(parser_t* parser, parser_state_t state) {
    (void)state;
    parser->function_call = htab_add_function(parser->global_symtable, &parser->token, false);
    gen_function_call_frame(parser->gen, &parser->token);
    next_token_check_type(parser, TOK_LPAREN);     // (
    rule_call_param(parser, state);                // <call_param>
    token_check_type(parser, TOK_RPAREN);          // )
    next_token_check_type(parser, TOK_SEMICOLON);  // ;
    htab_function_check_params(parser->function_call, parser->param_count, false);
    parser->param_count = 0;
    gen_function_call(parser->gen, state.in_function);
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

void rule_exp_for(parser_t* parser, parser_state_t state) {
    if (token_is_type(parser, TOK_SEMICOLON)) {
        return;
    } else {
        token_check_type(parser, TOK_VAR);                               // $var
        str_clear(&parser->gen->variable);                               //
        str_add_str(&parser->gen->variable, &parser->token.attr.val_s);  //
        parser_var_to_symtable(parser, state);                           //
        next_token_check_type(parser, TOK_ASSIGN);                       // =
        next_token(parser);                                              //
        rule_exp(parser, state);                                         // <exp>
    }
}

void rule_statement(parser_t* parser, parser_state_t state) {
    (void)state;
    switch (parser->token.type) {
        case TOK_VAR: {                                                      // $var
            token_t tmp = parser->token;                                     //
            str_clear(&parser->gen->variable);                               //
            str_add_str(&parser->gen->variable, &parser->token.attr.val_s);  //
            parser_var_to_symtable(parser, state);                           //
            next_token_keep(parser);                                         // =
            if (token_is_type(parser, TOK_ASSIGN)) {                         //
                token_free(&tmp);                                            //
                next_token(parser);                                          //
                rule_value(parser, state);                                   // <value>
                token_check_type(parser, TOK_SEMICOLON);                     // ;
            } else {                                                         //
                str_clear(&parser->gen->variable);                           //
                parser->buffer_token_valid = true;                           //
                parser->buffer_token = parser->token;                        //
                parser->token = tmp;                                         //
                rule_exp(parser, state);                                     // <exp>
                parser->buffer_token_valid = false;                          //
                token_check_type(parser, TOK_SEMICOLON);                     // ;
            }
            // token_free(&parser->old_token);

            break;
        }
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
        case TOK_WHILE: {                                       // while
            str_clear(&parser->gen->variable);                  //
            increment_construct_count(parser, &state);          //
            gen_loop(parser->gen, state.construct_count);       //
            next_token_check_type(parser, TOK_LPAREN);          // (
            next_token(parser);                                 //
            rule_exp(parser, state);                            // <exp>
            gen_loop_exit(parser->gen, state.construct_count);  //
            token_check_type(parser, TOK_RPAREN);               // )
            next_token_check_type(parser, TOK_LBRACE);          // {
            next_token(parser);                                 //
            parser_state_t child = state;
            child.loop_parent = state.construct_count;
            rule_statement(parser, child);                     // <statement>
            token_check_type(parser, TOK_RBRACE);              // }
            gen_loop_end(parser->gen, state.construct_count);  //
            break;
        }
        case TOK_FOR: {                                    // for
            str_clear(&parser->gen->variable);             //
            increment_construct_count(parser, &state);     //
            next_token_check_type(parser, TOK_LPAREN);     // (
            next_token(parser);                            //
            rule_exp_for(parser, state);                   // <ExpFor>
            gen_loop(parser->gen, state.construct_count);  // Initialize for
            token_check_type(parser, TOK_SEMICOLON);       // ;
            next_token(parser);                            //
            if (!token_is_type(parser, TOK_SEMICOLON)) {
                str_clear(&parser->gen->variable);
                rule_exp(parser, state);  // <exp>
                gen_loop_exit(parser->gen, state.construct_count);
            }
            token_check_type(parser, TOK_SEMICOLON);  // ;
            next_token(parser);                       //
            gen_for_modify_start(parser->gen, state.construct_count);
            if (!token_is_type(parser, TOK_RPAREN)) {
                rule_exp_for(parser, state);  // <ExpFor>
            }
            gen_for_modify_end(parser->gen, state.construct_count);
            token_check_type(parser, TOK_RPAREN);       // )
            next_token_check_type(parser, TOK_LBRACE);  // {
            next_token(parser);                         //
            parser_state_t child = state;
            child.loop_parent = state.construct_count;
            rule_statement(parser, child);         // <statement>
            token_check_type(parser, TOK_RBRACE);  // }
            gen_for_end(parser->gen, state.construct_count);
            break;
        }
        case TOK_BREAK:                                    // break
            gen_break(parser->gen, state.loop_parent);     //
            next_token_check_type(parser, TOK_SEMICOLON);  // ;
            break;
        case TOK_CONTINUE:                                 // continue
            gen_continue(parser->gen, state.loop_parent);  //
            next_token_check_type(parser, TOK_SEMICOLON);  // ;
            break;
        case TOK_FUN_NAME:                                             // function_name
            str_clear(&parser->gen->variable);                         //
            rule_function_call(parser, state);                         // <function_call>
            break;                                                     //
        case TOK_RETURN:                                               // return
            str_clear(&parser->gen->variable);                         //
            next_token(parser);                                        //
            if (parser->token.type == TOK_SEMICOLON) {                 //
                gen_return_void(parser->gen,                           //
                                state.in_function ?                    //
                                    &parser->function->value.function  //
                                                  : NULL);             // [;]
                break;                                                 //
            }                                                          //
            rule_value(parser, state);                                 // <value>
            increment_construct_count(parser, &state);                 //
            gen_return(parser->gen,                                    //
                       state.in_function ?                             //
                           &parser->function->value.function           //
                                         : NULL,                       //
                       state.construct_count);                         //
            break;
        default:
            if (token_is_expression(&parser->token)) {    //
                rule_exp(parser, state);                  // <exp>
                token_check_type(parser, TOK_SEMICOLON);  // ;
            } else {
                // We are stuck in a loop
                if (parser->last_default) {
                    error_exit(ERR_SYN);
                }
                parser->last_default = true;
                return;
            }
            break;
    }
    next_token(parser);
    rule_statement(parser, state);
}

void rule_function(parser_t* parser, parser_state_t state) {
    state.in_function = true;
    next_token_check_type(parser, TOK_FUN_NAME);                      // function_name
    parser->function = htab_add_function(parser->global_symtable,     //
                                         &parser->token, true);       //
    gen_function(parser->gen, &parser->token);                        //
    next_token_check_type(parser, TOK_LPAREN);                        // (
    rule_param(parser, state);                                        // <param>
    token_check_type(parser, TOK_RPAREN);                             // )
    next_token_check_type(parser, TOK_COLON);                         // :
    next_token_check_by_function(parser, token_is_returntype);        // type
    htab_function_add_return_type(parser->function, &parser->token);  //
    next_token_check_type(parser, TOK_LBRACE);                        // {
    next_token(parser);                                               //
    rule_statement(parser, state);                                    // <statement>
    token_check_type(parser, TOK_RBRACE);                             // }
    gen_function_end(parser->gen, &parser->function->value.function,  //
                     (char*)parser->function->key);                   //
    htab_function_check_params(parser->function, 0, true);            //
    htab_clear(parser->local_symtable);                               //
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
    htab_function_check_all_defined(parser->global_symtable);
    gen_footer(parser->gen);
#endif
#ifdef DEBUG_LEX
    next_token(parser);
    while (parser->token.type != TOK_EOF) {
        next_token(parser);
    }
#endif
}
