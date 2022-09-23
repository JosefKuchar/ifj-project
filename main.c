#include <stdio.h>
#include "error.h"
#include "str.h"
#include "token.h"

int main() {
  token_t tok = {.type = TOK_EQUALS};

  print_token(&tok);

  return RET_OK;
}
