#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <stdbool.h>
#include <string.h>

typedef struct htab htab_t;

// Types
typedef const char* htab_key_t;  // Key type
typedef int htab_value_t;        // Value type

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

// TODO: Documentation
htab_t* htab_init(size_t n);
size_t htab_size(const htab_t* t);

htab_pair_t* htab_find(htab_t* t, htab_key_t key);
htab_pair_t* htab_lookup_add(htab_t* t, htab_key_t key);

bool htab_erase(htab_t* t, htab_key_t key);

void htab_for_each(const htab_t* t, void (*f)(htab_pair_t* data));

void htab_clear(htab_t* t);
void htab_free(htab_t* t);

#endif  // __SYMTABLE_H__
