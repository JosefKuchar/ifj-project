#include "scanner.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "str.h"

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

        error_not_implemented();
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
