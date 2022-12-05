/**
 * Implementace překladače imperativního jazyka IFJ22
 *
 * @file scanner.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Header file for lexical analysis
 */

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include "token.h"

enum scanner_state {
    SC_CODE_START,      // Start of code - should start with <?php
    SC_ASSIGN,          // Tokens that start with =
    SC_EQUALS,          // Tokens that start with ==
    SC_EXCLAMATION,     // Tokens that start with !
    SC_NEQUALS,         // Tokens that start with !=
    SC_LESS,            // Tokens that start with <
    SC_GREATER,         // Tokens that start with >
    SC_VARIABLE_START,  // Tokens that start with $
    SC_VARIABLE,        // Correct variables
    SC_FUNCTION,        // Tokens that start with alpha or _
    SC_STRING_LIT,      // Tokens that start with "
    SC_DIVIDE,          // Tokens that start with /
    SC_MCOMMENT,        // Multiline comments
    SC_LCOMMENT,        // Oneline comments
    SC_QUESTION_MARK,   // Tokens that start with ?
    SC_END,             // Everything after ?>
    SC_HARDEND,         // End of file
    SC_START,           // Initial state
    SC_NUMBER,          // Everything that begins with digit
    SC_FLOAT,           // Everything after decimal point
    SC_EXPONENT_SIGN,   // Exponent sign
    SC_EXPONENT,        // Exponent
    SC_TYPE_OPTIONAL,   // For example ?int
};

typedef struct {
    enum scanner_state state;  // Current state
    str_t buffer;              // Buffer for previous characters
    FILE* input;               // Input stream
    size_t line_nr;
    size_t col_nr;
} scanner_t;

/**
 * @brief Get next token from input stream
 *
 * @param state Current state of the scanner
 * @return Next token
 */
token_t scanner_get_next(scanner_t* state);

/**
 * @brief Initialize scanner
 *
 * @return Initialized scanner
 */
scanner_t scanner_new(FILE* input);

/**
 * @brief Free existing scanner
 *
 * @param scanner Scanner
 */
void scanner_free(scanner_t* scanner);

#endif  // __SCANNER_H__
