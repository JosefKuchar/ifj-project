#include "str.h"
#include <stdlib.h>
#include "error.h"

// Default string size (including \0)
#define DEFAULT_SIZE 8

str_t str_new() {
  // Create new struct
  str_t str = {
      .val = malloc(DEFAULT_SIZE),
      .len = 0,
  };

  // Check if malloc failed
  if (str.val == NULL) {
    error_exit(ERR_INTERNAL);
  }

  // Make string valid
  str.val[0] = '\0';

  return str;
}

void str_free(str_t str) {
  // Free the string
  free(str.val);
  // Reset values just to be sure
  str.val = NULL;
  str.len = 0;
  str.size = 0;
}
