/**
 * @file symtable.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Header file for symbol table
 */

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <stdbool.h>
#include <string.h>
#include "str.h"
#include "token.h"

typedef struct htab htab_t;

typedef enum { HTAB_VARIABLE, HTAB_FUNCTION } htab_value_type_t;

typedef struct {
    token_type_t type;
    token_attribute_t value;
} htab_var_t;

typedef struct {
    str_t name;
    token_type_t type;
    bool required;
} htab_param_t;

typedef struct {
    token_type_t type;
    bool required;
} htab_return_t;

typedef struct {
    int param_count;
    bool defined;
    htab_param_t* params;
    htab_return_t returns;
} htab_fun_t;

typedef const char* htab_key_t;  // Key type
typedef struct {
    htab_value_type_t type;
    union {
        htab_fun_t function;
        htab_var_t variable;
    };
} htab_value_t;  // Value type

// Key value pair
typedef struct htab_pair {
    htab_key_t key;      // Key
    htab_value_t value;  // Value
} htab_pair_t;

// One item in hash table
struct htab_item {
    struct htab_pair pair;
    struct htab_item* next;
};

// Hash table representation
struct htab {
    size_t size;
    size_t arr_size;
    struct htab_item** arr_ptr;
};

/**
 * @brief Create new hash table
 *
 * @return htab_t*
 */
htab_t* htab_new();

/**
 * @brief Get number of items in hash table
 *
 * @param t Hash table
 * @return Number of items
 */
size_t htab_size(const htab_t* t);

/**
 * @brief Find item in hash table
 *
 * @param t Hash table
 * @param key  Key of searched item
 * @return Pointer to item or NULL if not found
 */
htab_pair_t* htab_find(htab_t* t, htab_key_t key);

/**
 * @brief Add new item to hash table
 *
 * @param t Hash table
 * @param key Key (e.g. variable name)
 * @param value Value (e.g. variable type)
 * @return htab_pair_t*
 */
htab_pair_t* htab_add(htab_t* t, htab_key_t key, htab_value_t value);

/**
 * @brief Add new function item to hash table
 *
 * @param t Hash table
 * @param token Current token (function name)
 * @return Pointer to new function inside hash table
 */
htab_pair_t* htab_add_function(htab_t* t, token_t* token, bool definition);

/**
 * @brief Add parameter to existing function
 *
 * @param fun Pointer to function inside hash table
 * @param token Current token (type)
 */
void htab_function_add_param(htab_pair_t* fun, token_t* token);

/**
 * @brief Add parameter name to existing function parameter
 *
 * @param fun Pointer to function inside hash table
 * @param token Curren token (variable name)
 */
void htab_function_add_param_name(htab_pair_t* fun, token_t* token);

bool htab_add_variable(htab_t* t, token_t* token);

void htab_for_each(const htab_t* t, void (*f)(htab_pair_t* data));

/**
 * @brief Remove all items from hash table
 *
 * @param t Hash table
 */
void htab_clear(htab_t* t);

/**
 * @brief Free hash table
 *
 * @param t Hash table
 */
void htab_free(htab_t* t);

void htab_define_buildin(htab_t* t);

#endif  // __SYMTABLE_H__
