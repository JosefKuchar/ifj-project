#include <stdio.h>
#include "error.h"
#include "str.h"

int main() {
  str_t test = str_new();
  printf("%s\n", test.val);

  return RET_OK;
}
