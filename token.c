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
                             [TOK_DOLLAR] = "dollar(parser)",
                             [TOK_HANDLE_START] = "handle_start",
                             [TOK_EXP_END] = "expression_end",
                             [TOK_E] = "expression(parser)"};

void token_print(token_t* token) {
    const char* name = token_names[token->type];

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

token_t token_new(token_type_t type) {
    return (token_t){.type = type};
}

token_t token_new_with_string(token_type_t type, str_t* str) {
    token_t token = {.type = type, .attr.val_s = str_new_from_str(str)};
    str_clear(str);
    return token;
}

token_t token_new_with_string_literal(token_type_t type, str_t* str) {
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
                    int number = (char)strtol(hex, NULL, 16);
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
    return (token_t){.type = type, .attr.val_s = new_str};
}

token_t token_new_with_int(token_type_t type, str_t* str) {
    // TODO: error checks
    long num = strtoul(str->val, NULL, 10);

    str_clear(str);
    return (token_t){.type = type, .attr.val_i = (int)num};
}

token_t token_new_with_float(token_type_t type, str_t* str) {
    // TODO: error checks
    // TODO: fix exponent parsing
    double num = strtod(str->val, NULL);

    str_clear(str);
    return (token_t){.type = type, .attr.val_f = num};
}

token_t token_new_with_bool(token_type_t type, bool val) {
    return (token_t){.type = type, .attr.val_b = val};
}

bool token_is_datatype(token_t* token) {
    return token->type == TOK_INT || token->type == TOK_FLOAT || token->type == TOK_STRING;
}

bool token_is_literal(token_t* token) {
    return token->type == TOK_INT_LIT || token->type == TOK_FLOAT_LIT || token->type == TOK_STR_LIT;
}

bool token_is_expression(token_t* token) {
    return token->type == TOK_LPAREN || token_is_literal(token) || token->type == TOK_VAR;
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
