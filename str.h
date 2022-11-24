/**
 * Implementace překladače imperativního jazyka IFJ22
 *
 * @file str.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Declaration of helper functions for working with dynamic strings
 */

#ifndef __STR_H__
#define __STR_H__

#include <stdbool.h>
#include <stdlib.h>

// String
typedef struct {
    char* val;    // Actual string
    size_t len;   // Length
    size_t size;  // Buffer size
} str_t;

/**
 * @brief Initialize new empty string
 *
 * @return Initiazed string
 */
str_t str_new();

/**
 * @brief Initialize string from existing string
 *
 * @param str Existing string (source)
 * @return Copied string
 */
str_t str_new_from_str(str_t* str);

/**
 * @brief Free existing string
 *
 * @param str String to be freed
 */
void str_free(str_t* str);

/**
 * @brief Add character to string
 *
 * @param str String to which the character will be added
 */
void str_add_char(str_t* str, char c);

/**
 * @brief Add c-string to existing string
 *
 * @param str String to which the other string will be added
 * @param cstr C-String
 */
void str_add_cstr(str_t* str, char* cstr);

void str_add_str(str_t* str, str_t* str2);

void str_add_int(str_t* str, int i);

/**
 * @brief Clear string
 */
void str_clear(str_t* str);

/**
 * @brief Print string
 *
 */
void str_print(str_t* str);

#endif  // __STR_H__
