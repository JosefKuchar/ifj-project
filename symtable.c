#include "symtable.h"
#include <stdint.h>
#include <stdlib.h>

#define AVG_LEN_MIN 0.5
#define AVG_LEN_MAX 2.0

size_t htab_bucket_count(const htab_t* t) {
    if (t == NULL) {
        return 0;
    }

    return t->arr_size;
}

void htab_clear(htab_t* t) {
    if (t == NULL) {
        return;
    }

    // Iterate over all buckets
    for (size_t i = 0; i < t->arr_size; i++) {
        struct htab_item* item = t->arr_ptr[i];
        // Item exists
        while (item != NULL) {
            struct htab_item* next = item->next;

            // Free the item
            free((char*)item->pair.key);
            free(item);

            // Update pointer
            item = next;
        }
        // Clear bucket pointer
        t->arr_ptr[i] = NULL;
    }
    // Clear size
    t->size = 0;
}

bool htab_erase(htab_t* t, htab_key_t key) {
    if (t == NULL || key == NULL) {
        return false;
    }

    // Find item bucket
    size_t index = htab_hash_function(key) % t->arr_size;
    struct htab_item* item = t->arr_ptr[index];
    struct htab_item* prev = NULL;

    // Find item
    while (item != NULL) {
        // Found the item
        if (strcmp(item->pair.key, key) == 0) {
            // If item is first then update head
            if (prev == NULL) {
                t->arr_ptr[index] = item->next;
            } else {
                prev->next = item->next;
            }

            // Free item
            free((char*)item->pair.key);
            free(item);

            // Update size
            t->size--;

            return true;
        }
        // Update pointers
        prev = item;
        item = item->next;
    }

    // Update hash table size
    if (htab_size(t) / (double)htab_bucket_count(t) < AVG_LEN_MIN) {
        htab_resize(t, htab_bucket_count(t) / 2);
    }

    // Item not found
    return false;
}

htab_pair_t* htab_find(htab_t* t, htab_key_t key) {
    if (t == NULL || key == NULL) {
        return NULL;
    }

    // Find item bucket
    size_t index = htab_hash_function(key) % t->arr_size;
    struct htab_item* item = t->arr_ptr[index];

    // Find item
    while (item != NULL) {
        // Found the item
        if (strcmp(item->pair.key, key) == 0) {
            return &(item->pair);
        }
        // Update pointer
        item = item->next;
    }

    // Item not found
    return NULL;
}

void htab_for_each(const htab_t* t, void (*f)(htab_pair_t* data)) {
    if (t == NULL || f == NULL) {
        return;
    }

    // Iterate over all buckets
    for (size_t i = 0; i < t->arr_size; i++) {
        struct htab_item* item = t->arr_ptr[i];
        // Item exists
        while (item != NULL) {
            // Call supplied function
            f(&(item->pair));

            // Update pointer
            item = item->next;
        }
    }
}

void htab_free(htab_t* t) {
    if (t == NULL) {
        return;
    }

    // Clear the table
    htab_clear(t);

    // Free the array
    free(t->arr_ptr);

    // Free the table
    free(t);
}

size_t htab_hash_function(const char* str) {
    uint32_t h = 0;  // musí mít 32 bitů
    const unsigned char* p;
    for (p = (const unsigned char*)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

htab_t* htab_init(size_t n) {
    // Allocate memory for the table
    htab_t* t = malloc(sizeof(htab_t));
    if (t == NULL) {
        return NULL;
    }

    // Allocate memory for the array
    t->arr_ptr = malloc(n * sizeof(struct htab_item*));
    if (t->arr_ptr == NULL) {
        free(t);
        return NULL;
    }

    // Initialize everything
    for (size_t i = 0; i < n; i++) {
        t->arr_ptr[i] = NULL;
    }
    t->arr_size = n;
    t->size = 0;

    return t;
}

htab_pair_t* htab_lookup_add(htab_t* t, htab_key_t key) {
    if (t == NULL || key == NULL) {
        return NULL;
    }

    // Look if pair already exists
    htab_pair_t* pair = htab_find(t, key);

    // Return existing pair if found
    if (pair != NULL) {
        return pair;
    }

    // Create new item
    struct htab_item* item = malloc(sizeof(struct htab_item));
    if (item == NULL) {
        return NULL;
    }

    // Initialize item
    item->next = NULL;
    item->pair.key = malloc(strlen(key) + 1);
    if (item->pair.key == NULL) {
        free(item);
        return NULL;
    }
    strcpy((char*)item->pair.key, key);
    item->pair.value = 0;

    // Add to list
    size_t index = htab_hash_function(key) % t->arr_size;
    struct htab_item* target = t->arr_ptr[index];

    // Add to head
    if (target == NULL) {
        t->arr_ptr[index] = item;
    } else {
        // Add to tail
        while (target->next != NULL) {
            target = target->next;
        }
        target->next = item;
    }

    // Update size
    t->size++;

    // Update hash table size
    if (htab_size(t) / (double)htab_bucket_count(t) > AVG_LEN_MAX) {
        htab_resize(t, htab_bucket_count(t) * 2);
    }

    // Return new item
    return &(item->pair);
}

void htab_resize(htab_t* t, size_t newn) {
    if (t == NULL) {
        return;
    }

    // Allocate new array
    struct htab_item** new_arr = malloc(newn * sizeof(struct htab_item*));
    if (new_arr == NULL) {
        return;  // Nothing happens
    }

    // Initialize new array
    for (size_t i = 0; i < newn; i++) {
        new_arr[i] = NULL;
    }

    // Rehash old items into new array
    for (size_t i = 0; i < t->arr_size; i++) {
        struct htab_item* item = t->arr_ptr[i];
        while (item != NULL) {
            // Get reference to next item
            struct htab_item* next = item->next;

            // Reset item's next pointer
            item->next = NULL;

            // Find new bucket
            size_t index = htab_hash_function(item->pair.key) % newn;

            struct htab_item* current = new_arr[index];

            // Add to head
            if (current == NULL) {
                new_arr[index] = item;
            } else {
                // Add to tail
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = item;
            }

            // Update pointer to next item
            item = next;
        }
    }

    // Free old array
    free(t->arr_ptr);

    // Update array pointer and size inside table
    t->arr_ptr = new_arr;
    t->arr_size = newn;
}

size_t htab_size(const htab_t* t) {
    if (t == NULL) {
        return 0;
    }

    return t->size;
}
