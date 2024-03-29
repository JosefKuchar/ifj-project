/**
 * Implementace překladače imperativního jazyka IFJ22
 *
 * @file token.c
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Implementation of token functions
 */

#include "token.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

bool is_valid_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool is_valid_octal(char c) {
    return c >= '0' && c <= '7';
}

// Names for all token types
const char* token_names[] = {[TOK_EOF] = "EOF",
                             [TOK_VAR] = "variable",
                             [TOK_STR_LIT] = "string literal",
                             [TOK_INT_LIT] = "integer literal",
                             [TOK_FLOAT_LIT] = "float literal",
                             [TOK_BOOL_LIT] = "bool literal",
                             [TOK_FUN_NAME] = "function name",
                             [TOK_DOT] = ".",
                             [TOK_COMMA] = ",",
                             [TOK_COLON] = ":",
                             [TOK_SEMICOLON] = ";",
                             [TOK_ASSIGN] = "=",
                             [TOK_EQUALS] = "===",
                             [TOK_NEQUALS] = "!==",
                             [TOK_PLUS] = "+",
                             [TOK_MINUS] = "-",
                             [TOK_MULTIPLY] = "*",
                             [TOK_DIVIDE] = "/",
                             [TOK_LESS] = "<",
                             [TOK_GREATER] = ">",
                             [TOK_LESS_E] = "<=",
                             [TOK_GREATER_E] = ">=",
                             [TOK_LPAREN] = "(",
                             [TOK_RPAREN] = ")",
                             [TOK_LBRACE] = "{",
                             [TOK_RBRACE] = "}",
                             [TOK_ELSE] = "else",
                             [TOK_FLOAT] = "float",
                             [TOK_FUNCTION] = "function",
                             [TOK_IF] = "if",
                             [TOK_INT] = "int",
                             [TOK_NULL] = "null",
                             [TOK_RETURN] = "return",
                             [TOK_STRING] = "string",
                             [TOK_VOID] = "void",
                             [TOK_WHILE] = "while",
                             [TOK_FOR] = "for",
                             [TOK_BREAK] = "break",
                             [TOK_CONTINUE] = "continue",
                             [TOK_DOLLAR] = "dollar(parser)",
                             [TOK_HANDLE_START] = "handle_start",
                             [TOK_EXP_END] = "expression_end",
                             [TOK_E] = "expression(parser)"};

void token_print(token_t* token) {
    const char* name = token_names[token->type];

    fprintf(stderr, "[%zu:%zu]", token->line_nr, token->col_nr);

    if (token->type == TOK_VAR || token->type == TOK_STR_LIT || token->type == TOK_FUN_NAME) {
        fprintf(stderr, "{ %s, \"%s\" }\n", name, token->attr.val_s.val);
    } else if (token->type == TOK_INT || token->type == TOK_FLOAT || token->type == TOK_STRING) {
        fprintf(stderr, "{ %s, %s }\n", name, token->attr.val_b ? "optional" : "required");
    } else if (token->type == TOK_INT_LIT) {
        fprintf(stderr, "{ %s, %d }\n", name, token->attr.val_i);
    } else if (token->type == TOK_FLOAT_LIT) {
        fprintf(stderr, "{ %s, %f }\n", name, token->attr.val_f);
    } else if (token->type == TOK_BOOL_LIT) {
        fprintf(stderr, "{ %s, %s }\n", name, token->attr.val_b ? "true" : "false");
    } else {
        fprintf(stderr, "{ %s }\n", token_names[token->type]);
    }
}

token_t token_new(token_type_t type, size_t line_nr, size_t col_nr) {
    return (token_t){.type = type, .line_nr = line_nr, .col_nr = col_nr};
}

token_t token_new_with_string(token_type_t type, str_t* str, size_t line_nr, size_t col_nr) {
    token_t token = {
        .type = type, .attr.val_s = str_new_from_str(str), .line_nr = line_nr, .col_nr = col_nr};
    str_clear(str);
    return token;
}

token_t token_new_with_string_literal(token_type_t type,
                                      str_t* str,
                                      size_t line_nr,
                                      size_t col_nr) {
    str_t new_str = str_new();

    // Loop through all characters in the string literal
    for (size_t i = 0; i < str->len; i++) {
        // Escape sequences
        if (str->val[i] == '\\') {
            // Characters in hex format (e.g. \x42)
            if (str->val[i + 1] == 'x') {
                // Check if the next two characters are valid hex
                if (i + 3 < str->len && is_valid_hex(str->val[i + 2]) &&
                    is_valid_hex(str->val[i + 3])) {
                    // Convert the hex to a character
                    char hex[3] = {str->val[i + 2], str->val[i + 3], '\0'};
                    int number = (int)strtol(hex, NULL, 16);
                    // Check if the number is in the valid range
                    if (number >= 1 && number <= 255) {
                        char c = (char)number;
                        str_add_char(&new_str, c);
                        i += 3;
                    } else {
                        // If not in the valid range, add it as is
                        str_add_char(&new_str, '\\');
                    }
                } else {
                    str_add_char(&new_str, '\\');
                }
                // Characters in decimal format (e.g. \064)
            } else if (is_valid_octal(str->val[i + 1])) {
                // Check if the next three characters are valid decimal
                if (i + 3 < str->len && is_valid_octal(str->val[i + 2]) &&
                    is_valid_octal(str->val[i + 3])) {
                    // Convert the decimal to a character
                    char num[4] = {str->val[i + 1], str->val[i + 2], str->val[i + 3], '\0'};
                    int number = (int)strtol(num, NULL, 8);
                    // Check if the number is in the valid range
                    if (number >= 1 && number <= 255) {
                        char c = (char)number;
                        str_add_char(&new_str, c);
                        i += 3;
                    } else {
                        // If not in the valid range, add it as is
                        str_add_char(&new_str, '\\');
                    }
                } else {
                    str_add_char(&new_str, '\\');
                }
                // Other escape sequences
            } else {
                switch (str->val[i + 1]) {
                    case 'n':
                        str_add_char(&new_str, '\n');
                        break;
                    case 'r':
                        str_add_char(&new_str, '\r');
                        break;
                    case 't':
                        str_add_char(&new_str, '\t');
                        break;
                    case 'v':
                        str_add_char(&new_str, '\v');
                        break;
                    case 'e':
                        str_add_char(&new_str, 27);  // \e is not supported by C
                        break;
                    case 'f':
                        str_add_char(&new_str, '\f');
                        break;
                    case '\\':
                        str_add_char(&new_str, '\\');
                        break;
                    case '$':
                        str_add_char(&new_str, '$');
                        break;
                    default:
                        // If the escape sequence is not valid, just add it as is
                        str_add_char(&new_str, '\\');
                        str_add_char(&new_str, str->val[i + 1]);
                }
                i++;
            }
        } else if (str->val[i] == '$') {
            // $ can't be used directly
            error_exit(ERR_LEX);
        } else {
            // Normal characters are added as is
            str_add_char(&new_str, str->val[i]);
        }
    }

    str_clear(str);
    return (token_t){.type = type, .attr.val_s = new_str, .line_nr = line_nr, .col_nr = col_nr};
}

token_t token_new_with_int(token_type_t type, str_t* str, size_t line_nr, size_t col_nr) {
    char* err;
    long num = strtoul(str->val, &err, 10);
    if (err == str->val || *err != '\0') {
        error_exit(ERR_LEX);
    }

    str_clear(str);
    return (token_t){.type = type, .attr.val_i = (int)num, .line_nr = line_nr, .col_nr = col_nr};
}

token_t token_new_with_float(token_type_t type, str_t* str, size_t line_nr, size_t col_nr) {
    char* err;
    double num = strtod(str->val, &err);
    if (err == str->val || *err != '\0') {
        error_exit(ERR_LEX);
    }

    str_clear(str);
    return (token_t){.type = type, .attr.val_f = num, .line_nr = line_nr, .col_nr = col_nr};
}

token_t token_new_with_bool(token_type_t type, bool val, size_t line_nr, size_t col_nr) {
    return (token_t){.type = type, .attr.val_b = val, .line_nr = line_nr, .col_nr = col_nr};
}

bool type_is_datatype(token_type_t type) {
    return type == TOK_INT || type == TOK_FLOAT || type == TOK_STRING;
}

bool type_is_literal(token_type_t type) {
    return type == TOK_INT_LIT || type == TOK_FLOAT_LIT || type == TOK_STR_LIT ||
           type == TOK_NULL || type == TOK_BOOL_LIT;
}

bool type_is_expression(token_type_t type) {
    return type == TOK_LPAREN || type_is_literal(type) || type == TOK_VAR;
}

bool type_is_ar_operator(token_type_t type) {
    return type == TOK_PLUS || type == TOK_MINUS || type == TOK_DIVIDE || type == TOK_MULTIPLY ||
           type == TOK_DOT;
}

bool type_is_comparator(token_type_t type) {
    return type == TOK_LESS || type == TOK_LESS_E || type == TOK_GREATER || type == TOK_GREATER_E ||
           type == TOK_EQUALS || type == TOK_NEQUALS;
}

void token_free(token_t* token) {
    if (token->type == TOK_VAR || token->type == TOK_STR_LIT || token->type == TOK_FUN_NAME) {
        str_free(&token->attr.val_s);
    }
}

char* token_to_string(token_type_t type) {
    char* name = (char*)token_names[type];
    return name;
}

bool type_is_number(token_type_t type) {
    return type == TOK_FLOAT_LIT || type == TOK_INT_LIT;
}

bool token_is_datatype(token_t* token) {
    return type_is_datatype(token->type);
}

bool token_is_returntype(token_t* token) {
    return token->type == TOK_INT || token->type == TOK_FLOAT || token->type == TOK_STRING ||
           token->type == TOK_VOID;
}

bool token_is_literal(token_t* token) {
    return type_is_literal(token->type);
}

bool token_is_expression(token_t* token) {
    return type_is_expression(token->type);
}

bool token_is_ar_operator(token_t* token) {
    return type_is_ar_operator(token->type);
}

bool token_is_comparator(token_t* token) {
    return type_is_comparator(token->type);
}
