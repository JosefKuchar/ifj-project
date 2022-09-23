#include <stdio.h>
#include "scanner.h"

int main() {
  scanner_t scanner = scanner_new();

  while (true) {
    token_t token = scanner_get_next(&scanner);
    print_token(&token);
  }

  return 0;
}
