#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "token.h"

enum scanner_state {
  SC_CODE_START,       // Start of code - should start with <?php
  SC_PROLOG_SPACE,     // Space between php and declare
  SC_PROLOG_MCOMMENT,  // Multiline comment (between php and declare)
  SC_PROLOG_LCOMMENT,  // Oneline comment (between php and declare)
  SC_DECLARE,          // Should be declare(strict_types=1);
  SC_ASSIGN,           // Tokens that start with =
  SC_EQUALS,           // Tokens that start with ==
  SC_EXCLAMATION,      // Tokens that start with !
  SC_NEQUALS,          // Tokens that start with !=
  SC_LESS,             // Tokens that start with <
  SC_GREATER,          // Tokens that start with >
  SC_VARIABLE_START,   // Tokens that start with $
  SC_VARIABLE,         // Correct variables
  SC_FUNCTION,         // Tokens that start with alpha or _
  SC_STRING_LIT,       // Tokens that start with "
  SC_START
};

typedef struct {
  enum scanner_state state;  // Current state
  str_t buffer;              // Buffer for previous characters
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
scanner_t scanner_new();

/**
 * @brief Free existing scanner
 *
 * @param scanner Scanner
 */
void scanner_free(scanner_t* scanner);

#endif  // __SCANNER_H__
