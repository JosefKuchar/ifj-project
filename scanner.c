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

    return -1;
}

token_t scanner_get_next(scanner_t* scanner) {
    while (true) {
        const int c = fgetc(stdin);

        switch (scanner->state) {
            case SC_CODE_START: {
                // If we encounter EOF in this state it's an error
                if (c == EOF) {
                    error_exit(ERR_LEX);
                }

                if (isspace(c)) {
                    // TODO: there can be whitespaces between php and declare
                    if (strcmp(scanner->buffer.val, "<?php") != 0) {
                        error_exit(ERR_LEX);  // TODO: Check if this error is right
                    } else {
                        str_clear(&scanner->buffer);
                        scanner->state = SC_PROLOG_SPACE;
                        break;
                    }
                }
                str_add_char(&scanner->buffer, c);
                break;
            }
            case SC_PROLOG_SPACE: {
                // If we encounter EOF in this state it's an error
                if (c == EOF) {
                    error_exit(ERR_LEX);
                }

                if (isspace(c)) {
                    break;
                }

                if (c == '/') {
                    const int c2 = fgetc(stdin);
                    if (c2 == '*') {
                        scanner->state = SC_PROLOG_MCOMMENT;
                    } else if (c2 == '/') {
                        scanner->state = SC_PROLOG_LCOMMENT;
                    } else {
                        // Division token can't be here so it's an error
                        error_exit(ERR_LEX);
                    }
                } else {
                    ungetc(c, stdin);
                    scanner->state = SC_DECLARE;
                }

                break;
            }
            case SC_PROLOG_LCOMMENT: {
                if (c == '\n' || c == EOF) {
                    scanner->state = SC_PROLOG_SPACE;
                }
                break;
            }
            case SC_PROLOG_MCOMMENT: {
                if (c == EOF) {
                    error_exit(ERR_LEX);
                }

                if (c == '*') {
                    const int c2 = fgetc(stdin);
                    if (c2 == '/') {
                        scanner->state = SC_PROLOG_SPACE;
                    } else {
                        ungetc(c2, stdin);
                    }
                }
                break;
            }
            case SC_DECLARE: {
                if (c == EOF) {
                    error_exit(ERR_LEX);
                }

                str_add_char(&scanner->buffer, c);

                // 24 is length of declare(strict_types=1);
                if (scanner->buffer.len == 24) {
                    if (strcmp(scanner->buffer.val, "declare(strict_types=1);") == 0) {
                        str_clear(&scanner->buffer);
                        scanner->state = SC_START;
                    } else {
                        error_exit(ERR_LEX);
                    }
                }
                break;
            }
            case SC_START: {
                // Skip whitespaces
                if (isspace(c)) {
                    break;
                }

                // Single char tokens (that are for sure end states)
                switch (c) {
                    case '+':
                        return token_new(TOK_PLUS);
                    case '-':
                        return token_new(TOK_MINUS);
                    case '*':
                        return token_new(TOK_MULTIPLY);
                    case '(':
                        return token_new(TOK_LPAREN);
                    case ')':
                        return token_new(TOK_RPAREN);
                    case '{':
                        return token_new(TOK_LBRACE);
                    case '}':
                        return token_new(TOK_RBRACE);
                    case '.':
                        return token_new(TOK_DOT);
                    case ',':
                        return token_new(TOK_COMMA);
                    case ';':
                        return token_new(TOK_SEMICOLON);
                    case ':':
                        return token_new(TOK_COLON);
                    case EOF:
                        return token_new(TOK_EOF);
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

                error_not_implemented();
                break;
            }
            case SC_ASSIGN: {
                if (c == '=') {
                    scanner->state = SC_EQUALS;
                } else {
                    ungetc(c, stdin);
                    scanner->state = SC_START;
                    return token_new(TOK_ASSIGN);
                }
                break;
            }
            case SC_EQUALS: {
                if (c == '=') {
                    scanner->state = SC_START;
                    return token_new(TOK_EQUALS);
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
                    return token_new(TOK_NEQUALS);
                } else {
                    // We don't have != token
                    error_exit(ERR_LEX);
                }
                break;
            }
            case SC_LESS: {
                scanner->state = SC_START;
                if (c == '=') {
                    return token_new(TOK_LESS_E);
                } else {
                    ungetc(c, stdin);
                    return token_new(TOK_LESS);
                }
                break;
            }
            case SC_GREATER: {
                scanner->state = SC_START;
                if (c == '=') {
                    return token_new(TOK_GREATER_E);
                } else {
                    ungetc(c, stdin);
                    return token_new(TOK_GREATER);
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
                    ungetc(c, stdin);
                    scanner->state = SC_START;
                    return token_new_with_string(TOK_VAR, &scanner->buffer);
                }
                break;
            }
            case SC_FUNCTION: {
                if (isalnum(c) || c == '_') {
                    str_add_char(&scanner->buffer, c);
                } else {
                    ungetc(c, stdin);
                    scanner->state = SC_START;
                    // Check if string is keyword
                    int keyword = get_keyword_token_type(&scanner->buffer);
                    if (keyword != -1) {
                        str_clear(&scanner->buffer);
                        return token_new(keyword);
                    } else {
                        return token_new_with_string(TOK_FUN_NAME, &scanner->buffer);
                    }
                }
                break;
            }
            case SC_STRING_LIT: {
                if (c == EOF) {
                    error_exit(ERR_LEX);
                }

                if (c == '"') {
                    scanner->state = SC_START;
                    return token_new_with_string(TOK_STR_LIT, &scanner->buffer);
                } else {
                    str_add_char(&scanner->buffer, c);
                }
                break;
            }
            case SC_DIVIDE: {
                if (c == '*') {
                    scanner->state = SC_MCOMMENT;
                } else if (c == '/') {
                    scanner->state = SC_LCOMMENT;
                } else {
                    ungetc(c, stdin);
                    scanner->state = SC_START;
                    return token_new(TOK_DIVIDE);
                }
                break;
            }
            case SC_LCOMMENT: {
                if (c == '\n' || c == EOF) {
                    scanner->state = SC_START;
                }
                break;
            }
            case SC_MCOMMENT: {
                if (c == EOF) {
                    scanner->state = SC_START;
                } else if (c == '*') {
                    const int c2 = fgetc(stdin);
                    if (c2 == '/') {
                        scanner->state = SC_START;
                    } else {
                        ungetc(c2, stdin);
                    }
                }
                break;
            }
            case SC_QUESTION_MARK: {
                if (c == '>') {
                    scanner->state = SC_END;
                } else {
                    error_not_implemented();
                }
                break;
            }
            case SC_END: {
                if (c == EOF) {
                    return token_new(TOK_EOF);
                }

                if (isspace(c)) {
                    break;
                } else {
                    // There can't be anything after ?>
                    error_exit(ERR_LEX);
                }
            }
        }
    }
}

scanner_t scanner_new() {
    scanner_t scanner = {.buffer = str_new(), .state = SC_CODE_START};
    return scanner;
}

void scanner_free(scanner_t* scanner) {
    str_free(&scanner->buffer);
}
