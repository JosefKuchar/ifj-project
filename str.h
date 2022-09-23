#ifndef __STR_H__
#define __STR_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  char* val;
  uint32_t len;
  uint32_t size;
} str_t;

/**
 * @brief Initialize new empty string
 *
 * @return Initiazed string
 */
str_t str_new();

/**
 * @brief Free existing string
 *
 * @param str String to be freed
 */
void str_free(str_t str);

#endif  // __STR_H__
