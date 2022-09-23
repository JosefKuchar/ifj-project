#include "scanner.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "str.h"

token_t scanner_get_next(scanner_t* scanner) {
  const int c = fgetc(stdin);

  switch (scanner->state) {
    case SC_CODE_START: {
      if (c == EOF || c == ';') {
        // TODO: there can be whitespaces between php and declare
        if (strcmp(scanner->buffer.val, "<?php\ndeclare(strict_types=1)") != 0) {
          error_exit(ERR_LEX);  // TODO: Check if this error is right
        } else {
          scanner->state = SC_START;
          break;
        }
      }
      str_add_char(&scanner->buffer, c);
      break;
    }
    case SC_START: {
      fprintf(stderr, "Not implemented!\n");
      error_exit(ERR_INTERNAL);
    }
  }

  return scanner_get_next(scanner);
}

scanner_t scanner_new() {
  scanner_t scanner = {.buffer = str_new(), .state = SC_CODE_START};
  return scanner;
}
