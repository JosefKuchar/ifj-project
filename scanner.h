#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "token.h"

enum scanner_state {
  SC_CODE_START,  // Start of code - should start with <?php
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

#endif  // __SCANNER_H__
