#include <stdio.h>
#include "error.h"
#include "scanner.h"

int main() {
  scanner_t scanner = scanner_new();
  token_t token = scanner_get_next(&scanner);

  while (token.type != TOK_EOF) {
    token = scanner_get_next(&scanner);
    print_token(&token);
  }

  scanner_free(&scanner);

  return RET_OK;
}
