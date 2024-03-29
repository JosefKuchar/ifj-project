/**
 * Implementace překladače imperativního jazyka IFJ22
 *
 * @file scanner.c
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Lexical analysis
 */

#include "scanner.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "str.h"

/**
 * @brief Get keyword token type
 *
 * @param str Keyword name
 * @return Keyword token type or -1 if string is not keyword
 */
int get_keyword_token_type(str_t* str) {
    if (strcmp("else", str->val) == 0)
        return TOK_ELSE;
    if (strcmp("float", str->val) == 0)
        return TOK_FLOAT;
    if (strcmp("function", str->val) == 0)
        return TOK_FUNCTION;
    if (strcmp("if", str->val) == 0)
        return TOK_IF;
    if (strcmp("int", str->val) == 0)
        return TOK_INT;
    if (strcmp("null", str->val) == 0)
        return TOK_NULL;
    if (strcmp("return", str->val) == 0)
        return TOK_RETURN;
    if (strcmp("string", str->val) == 0)
        return TOK_STRING;
    if (strcmp("void", str->val) == 0)
        return TOK_VOID;
    if (strcmp("while", str->val) == 0)
        return TOK_WHILE;
    if (strcmp("for", str->val) == 0)
        return TOK_FOR;
    if (strcmp("break", str->val) == 0)
        return TOK_BREAK;
    if (strcmp("continue", str->val) == 0)
        return TOK_CONTINUE;
                
    return -1;
}

token_t scanner_get_next(scanner_t* scanner) {
    while (true) {
        scanner->col_nr++;
        const int c = fgetc(scanner->input);

        switch (scanner->state) {
            case SC_CODE_START: {
                // If we encounter EOF in this state it's an error
                if (c == EOF) {
                    error_exit(ERR_LEX);
                }

                str_add_char(&scanner->buffer, c);
                if (scanner->buffer.len == 5) {  // <?php
                    if (strcmp(scanner->buffer.val, "<?php") != 0) {
                        error_exit(ERR_LEX);
                    } else {
                        str_clear(&scanner->buffer);
                        scanner->state = SC_START;
                        break;
                    }
                }
                break;
            }
            case SC_START: {
                // Skip whitespaces
                if (isspace(c)) {
                    if (c == '\n') {
                        scanner->col_nr = 0;
                        scanner->line_nr++;
                    } else {
                    }
                    break;
                }

                // Single char tokens (that are for sure end states)
                switch (c) {
                    case '+':
                        return token_new(TOK_PLUS, scanner->line_nr, scanner->col_nr);
                    case '-':
                        return token_new(TOK_MINUS, scanner->line_nr, scanner->col_nr);
                    case '*':
                        return token_new(TOK_MULTIPLY, scanner->line_nr, scanner->col_nr);
                    case '(':
                        return token_new(TOK_LPAREN, scanner->line_nr, scanner->col_nr);
                    case ')':
                        return token_new(TOK_RPAREN, scanner->line_nr, scanner->col_nr);
                    case '{':
                        return token_new(TOK_LBRACE, scanner->line_nr, scanner->col_nr);
                    case '}':
                        return token_new(TOK_RBRACE, scanner->line_nr, scanner->col_nr);
                    case '.':
                        return token_new(TOK_DOT, scanner->line_nr, scanner->col_nr);
                    case ',':
                        return token_new(TOK_COMMA, scanner->line_nr, scanner->col_nr);
                    case ';':
                        return token_new(TOK_SEMICOLON, scanner->line_nr, scanner->col_nr);
                    case ':':
                        return token_new(TOK_COLON, scanner->line_nr, scanner->col_nr);
                    case EOF:
                        return token_new(TOK_EOF, scanner->line_nr, scanner->col_nr);
                }

                // Multi char tokens
                switch (c) {
                    case '=':
                        scanner->state = SC_ASSIGN;
                        continue;
                    case '!':
                        scanner->state = SC_EXCLAMATION;
                        continue;
                    case '<':
                        scanner->state = SC_LESS;
                        continue;
                    case '>':
                        scanner->state = SC_GREATER;
                        continue;
                    case '$':
                        scanner->state = SC_VARIABLE_START;
                        str_add_char(&scanner->buffer, c);
                        continue;
                    case '"':
                        scanner->state = SC_STRING_LIT;
                        continue;
                    case '/':
                        scanner->state = SC_DIVIDE;
                        continue;
                    case '?':
                        scanner->state = SC_QUESTION_MARK;
                        continue;
                }

                // Function names and keywords
                if (isalpha(c) || c == '_') {
                    scanner->state = SC_FUNCTION;

                    str_add_char(&scanner->buffer, c);
                    break;
                }

                // Numbers
                if (isdigit(c)) {
                    scanner->state = SC_NUMBER;

                    str_add_char(&scanner->buffer, c);
                    break;
                }

                error_exit(ERR_LEX);
                break;
            }
            case SC_ASSIGN: {
                if (c == '=') {
                    scanner->state = SC_EQUALS;
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_START;
                    return token_new(TOK_ASSIGN, scanner->line_nr, scanner->col_nr);
                }
                break;
            }
            case SC_EQUALS: {
                if (c == '=') {
                    scanner->state = SC_START;
                    return token_new(TOK_EQUALS, scanner->line_nr, scanner->col_nr);
                } else {
                    // We don't have == token
                    error_exit(ERR_LEX);
                }
                break;
            }
            case SC_EXCLAMATION: {
                if (c == '=') {
                    scanner->state = SC_NEQUALS;
                } else {
                    // We don't have ! token
                    error_exit(ERR_LEX);
                }
                break;
            }
            case SC_NEQUALS: {
                if (c == '=') {
                    scanner->state = SC_START;
                    return token_new(TOK_NEQUALS, scanner->line_nr, scanner->col_nr);
                } else {
                    // We don't have != token
                    error_exit(ERR_LEX);
                }
                break;
            }
            case SC_LESS: {
                scanner->state = SC_START;
                if (c == '=') {
                    return token_new(TOK_LESS_E, scanner->line_nr, scanner->col_nr);
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    return token_new(TOK_LESS, scanner->line_nr, scanner->col_nr);
                }
                break;
            }
            case SC_GREATER: {
                scanner->state = SC_START;
                if (c == '=') {
                    return token_new(TOK_GREATER_E, scanner->line_nr, scanner->col_nr);
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    return token_new(TOK_GREATER, scanner->line_nr, scanner->col_nr);
                }
                break;
            }
            case SC_VARIABLE_START: {
                if (isalpha(c) || c == '_') {
                    str_add_char(&scanner->buffer, c);
                    scanner->state = SC_VARIABLE;
                } else {
                    // There has to be valid character after $
                    error_exit(ERR_LEX);
                }
                break;
            }
            case SC_VARIABLE: {
                if (isalnum(c) || c == '_') {
                    str_add_char(&scanner->buffer, c);
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_START;
                    return token_new_with_string(TOK_VAR, &scanner->buffer, scanner->line_nr,
                                                 scanner->col_nr);
                }
                break;
            }
            case SC_FUNCTION: {
                if (isalnum(c) || c == '_') {
                    str_add_char(&scanner->buffer, c);
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_START;
                    // Check if string is keyword
                    int keyword = get_keyword_token_type(&scanner->buffer);
                    if (keyword != -1) {
                        str_clear(&scanner->buffer);
                        // False means it's not optional type
                        return token_new_with_bool(keyword, false, scanner->line_nr,
                                                   scanner->col_nr);
                    } else {
                        return token_new_with_string(TOK_FUN_NAME, &scanner->buffer,
                                                     scanner->line_nr, scanner->col_nr);
                    }
                }
                break;
            }
            case SC_STRING_LIT: {
                if (c == EOF) {
                    error_exit(ERR_LEX);
                }

                // Handle escape sequence
                if (c == '\\') {
                    const int c2 = getc(scanner->input);
                    if (c2 == '"') {
                        str_add_char(&scanner->buffer, '"');
                        break;
                    } else if (c2 == '\\') {
                        str_add_char(&scanner->buffer, '\\');
                    } else {
                        ungetc(c2, scanner->input);
                    }
                }

                if (c == '"') {
                    scanner->state = SC_START;
                    return token_new_with_string_literal(TOK_STR_LIT, &scanner->buffer,
                                                         scanner->line_nr, scanner->col_nr);
                } else if (c >= 32) {
                    str_add_char(&scanner->buffer, c);
                } else {
                    error_exit(ERR_LEX);
                }
                break;
            }
            case SC_DIVIDE: {
                if (c == '*') {
                    scanner->col_nr = 0;
                    scanner->line_nr++;
                    scanner->state = SC_MCOMMENT;
                } else if (c == '/') {
                    scanner->state = SC_LCOMMENT;
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_START;
                    return token_new(TOK_DIVIDE, scanner->line_nr, scanner->col_nr);
                }
                break;
            }
            case SC_LCOMMENT: {
                if (c == '\n' || c == EOF) {
                    if (c == '\n') {
                        scanner->col_nr = 0;
                        scanner->line_nr++;
                    }
                    scanner->state = SC_START;
                }
                break;
            }
            case SC_MCOMMENT: {
                if (c == EOF) {
                    error_exit(ERR_LEX);  // Unterminated comment
                } else if (c == '*') {
                    scanner->col_nr = 0;
                    scanner->line_nr++;
                    const int c2 = fgetc(scanner->input);
                    if (c2 == '/') {
                        scanner->state = SC_START;
                    } else if (c == EOF) {
                        error_exit(ERR_LEX);
                    } else {
                        ungetc(c2, scanner->input);
                    }
                }
                break;
            }
            case SC_QUESTION_MARK: {
                if (c == '>') {
                    scanner->state = SC_END;
                } else if (isalpha(c)) {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_TYPE_OPTIONAL;
                } else {
                    error_exit(ERR_LEX);
                }
                break;
            }
            case SC_END: {
                if (c == EOF) {
                    return token_new(TOK_EOF, scanner->line_nr, scanner->col_nr);
                }

                if (c == '\n' || (c == '\r' && getc(scanner->input) == '\n')) {
                    scanner->state = SC_HARDEND;
                    break;
                } else {
                    // There can't be anything after ?> except \n and \r\n
                    error_exit(ERR_SYN);
                }
                break;
            }
            case SC_HARDEND: {
                // Only EOF is allowed after hard end
                if (c == EOF) {
                    return token_new(TOK_EOF, scanner->line_nr, scanner->col_nr);
                }
                error_exit(ERR_SYN);
                break;
            }
            case SC_NUMBER: {
                if (isdigit(c)) {
                    str_add_char(&scanner->buffer, c);
                } else if (c == '.') {
                    str_add_char(&scanner->buffer, c);
                    const int c2 = fgetc(scanner->input);
                    if (isdigit(c2)) {
                        ungetc(c2, scanner->input);
                        scanner->state = SC_FLOAT;
                    } else {
                        error_exit(ERR_LEX);
                    }
                } else if (c == 'e' || c == 'E') {
                    str_add_char(&scanner->buffer, c);
                    scanner->state = SC_EXPONENT_SIGN;
                } else if (isalpha(c)) {
                    error_exit(ERR_LEX);
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_START;
                    return token_new_with_int(TOK_INT_LIT, &scanner->buffer, scanner->line_nr,
                                              scanner->col_nr);
                }
                break;
            }
            case SC_FLOAT: {
                if (isdigit(c)) {
                    str_add_char(&scanner->buffer, c);
                } else if (c == 'e' || c == 'E') {
                    str_add_char(&scanner->buffer, 'e');
                    scanner->state = SC_EXPONENT_SIGN;
                } else if (isalpha(c)) {
                    error_exit(ERR_LEX);
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_START;
                    return token_new_with_float(TOK_FLOAT_LIT, &scanner->buffer, scanner->line_nr,
                                                scanner->col_nr);
                }
                break;
            }

            case SC_EXPONENT_SIGN: {
                if (c == '+' || c == '-') {
                    const int c2 = getc(scanner->input);
                    if (isdigit(c2)) {
                        ungetc(c2, scanner->input);

                        str_add_char(&scanner->buffer, c);
                        scanner->state = SC_EXPONENT;
                    } else {
                        error_exit(ERR_LEX);
                    }
                } else if (isdigit(c)) {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_EXPONENT;
                } else {
                    error_exit(ERR_LEX);
                }
                break;
            }
            case SC_EXPONENT: {
                if (isdigit(c)) {
                    str_add_char(&scanner->buffer, c);
                } else if (isalpha(c)) {
                    error_exit(ERR_LEX);
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    scanner->state = SC_START;
                    return token_new_with_float(TOK_FLOAT_LIT, &scanner->buffer, scanner->line_nr,
                                                scanner->col_nr);
                }
                break;
            }
            case SC_TYPE_OPTIONAL: {
                if (isalpha(c)) {
                    str_add_char(&scanner->buffer, c);
                } else {
                    scanner->col_nr--;
                    ungetc(c, scanner->input);
                    int keyword = get_keyword_token_type(&scanner->buffer);
                    if (keyword != -1 &&
                        (keyword != TOK_FLOAT || keyword != TOK_INT || keyword != TOK_STRING)) {
                        scanner->state = SC_START;
                        str_clear(&scanner->buffer);
                        return token_new_with_bool(keyword, true, scanner->line_nr,
                                                   scanner->col_nr);
                    } else {
                        error_exit(ERR_LEX);
                    }
                }
                break;
            }
        }
    }
}

scanner_t scanner_new(FILE* input) {
    return (scanner_t){
        .buffer = str_new(), .state = SC_CODE_START, .input = input, .line_nr = 1, .col_nr = 0};
}

void scanner_free(scanner_t* scanner) {
    str_free(&scanner->buffer);
}
