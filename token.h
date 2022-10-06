#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "str.h"

/**
 * @brief All token types
 */
typedef enum {
    TOK_PLUS,          // +
    TOK_MINUS,         // -
    TOK_MULTIPLY,      // *
    TOK_DIVIDE,        // /
    TOK_LESS,          // <
    TOK_LESS_E,        // <=
    TOK_GREATER,       // >
    TOK_GREATER_E,     // >=
    TOK_EQUALS,        // ===
    TOK_NEQUALS,       // !==
    TOK_LPAREN,        // (
    TOK_RPAREN,        // )
    TOK_VAR,           // Identifier (eg. $foo)
    TOK_INT_LIT,       // Integer literal (eg. 0)
    TOK_FLOAT_LIT,     // Float literal (eg. 0.0)
    TOK_STR_LIT,       // String literal (eg. "foo")
    TOK_DOT,           // .
    TOK_NULL,          // Null
    TOK_DOLLAR,        // BOTTOM OF THE STACK (INTERNAL TO EXPR PARSING)
    TOK_HANDLE_START,  // HANDLE_START (INTERNAL TO EXPR PARSING)
    TOK_EXP_END,       // Expression end (Internal to expr parsing)
    TOK_E,             // expression (Internal to expr parsing)

    TOK_EOF,        // End of file
    TOK_BOOL_LIT,   // Bool literal (eg. true)
    TOK_FUN_NAME,   // Function name (eg. write)
    TOK_COMMA,      // ,
    TOK_COLON,      // :
    TOK_SEMICOLON,  // ;
    TOK_ASSIGN,     // =
    TOK_LBRACE,     // {
    TOK_RBRACE,     // }

    /* KEYWORDS */
    TOK_ELSE,      // else
    TOK_FLOAT,     // float
    TOK_FUNCTION,  // function
    TOK_IF,        // if
    TOK_INT,       // int
    TOK_RETURN,    // return
    TOK_STRING,    // string
    TOK_VOID,      // void
    TOK_WHILE,     // while

} token_type_t;

/**
 * @brief Token attribute
 */
typedef union {
    bool val_b;    // Bool value (for TOK_BOOL_LIT)
    int val_i;     // Integer value (for TOK_INT_LIT)
    double val_f;  // Float value (for TOK_FLOAT_LIT)
    str_t val_s;   // String value (for TOK_STR_LIT, TOK_ID, TOK_FUN_NAME)
} token_attribute_t;

/**
 * @brief Token struct
 */
typedef struct {
    token_type_t type;
    token_attribute_t attr;
} token_t;

/**
 * @brief Print token in pretty format
 *
 * @param token Token to be printed
 */
void token_print(token_t* token);

/**
 * @brief Create new token
 *
 * @param type Token type
 * @return New token
 */
token_t token_new(token_type_t type);

/**
 * @brief Create new token with supplied string AND CONSUME THE STRING
 *
 * @param type Token type
 * @param str String
 * @return New token with string
 */
token_t token_new_with_string(token_type_t type, str_t* str);

/**
 * @brief Create new token with supplied string literal AND CONSUME THE STRING
 *
 * @param type Token type
 * @param str String
 * @return New token with string
 */
token_t token_new_with_string_literal(token_type_t type, str_t* str);

/**
 * @brief Create new token with supplied string value of an int AND CONSUME THE STRING
 *
 * @param type Token type
 * @param str String
 * @return New token with int
 */
token_t token_new_with_int(token_type_t type, str_t* str);

/**
 * @brief Create new token with supplied string value of a float AND CONSUME THE STRING
 *
 * @param type Token type
 * @param str String
 * @return New token with float
 */
token_t token_new_with_float(token_type_t type, str_t* str);

/**
 * @brief Create new token with supplied bool
 *
 * @param type Token type
 * @param val Bool value
 * @return New token with bool
 */
token_t token_new_with_bool(token_type_t type, bool val);

/**
 * @brief Check if token is a data type (eg. int, float, string)
 *
 * @param token Token
 * @return true if token is a data type
 */
bool token_is_datatype(token_t* token);

/**
 * @brief Check if token is a literal (eg. int literal etc.)
 *
 * @param token Token
 * @return true if token is a literal
 */
bool token_is_literal(token_t* token);

/**
 * @brief Check if token is a valid expression start
 *
 * @param token Token
 * @return true if token can be an expression start
 */
bool token_is_expression(token_t* token);

/**
 * @brief Free existing token
 *
 * @param token Token
 */
void token_free(token_t* token);

/**
 * @brief Print enum value as a string
 *
 * @param token_type type
 * @return String representation of the enum value
 */
char* token_to_string(token_type_t type);

/**
 * @brief Checks if type is a number type (eg. float, int etc.)
 *
 * @param token_type Type
 * @return true if token is a number type
 */
bool type_is_number(token_type_t type);

/**
 * @brief Checks if type is an arithmetic operator (eg. plus, minus etc.)
 *
 * @param token Token
 * @return true if token is a number type
 */
bool token_is_ar_operator(token_t* token);

/**
 * @brief Checks if type is a logical operator (eg. less than, not equals etc.)
 *
 * @param token Token
 * @return true if token is a number type
 */
bool token_is_comparator(token_t* token);

#endif  // __TOKEN_H__
