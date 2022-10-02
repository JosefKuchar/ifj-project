#include "symtable.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "token.h"

#define AVG_LEN_MIN 0.5
#define AVG_LEN_MAX 2.0

size_t htab_hash_function(const char* str) {
    uint32_t h = 0;  // musí mít 32 bitů
    const unsigned char* p;
    for (p = (const unsigned char*)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

void htab_resize(htab_t* t, size_t newn) {
    if (t == NULL) {
        return;
    }

    // Allocate new array
    struct htab_item** new_arr = malloc(newn * sizeof(struct htab_item*));
    if (new_arr == NULL) {
        error_exit(ERR_INTERNAL);
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
            if (item->pair.value.type == HTAB_FUNCTION) {
                for (int j = 0; j < item->pair.value.function.param_count; j++) {
                    str_free(&item->pair.value.function.params[j].name);
                }
                free(item->pair.value.function.params);
            }
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

htab_t* htab_init(size_t n) {
    // Allocate memory for the table
    htab_t* t = malloc(sizeof(htab_t));
    if (t == NULL) {
        error_exit(ERR_INTERNAL);
    }

    // Allocate memory for the array
    t->arr_ptr = malloc(n * sizeof(struct htab_item*));
    if (t->arr_ptr == NULL) {
        free(t);
        error_exit(ERR_INTERNAL);
    }

    // Initialize everything
    for (size_t i = 0; i < n; i++) {
        t->arr_ptr[i] = NULL;
    }
    t->arr_size = n;
    t->size = 0;

    return t;
}

htab_pair_t* htab_add(htab_t* t, htab_key_t key, htab_value_t value) {
    if (t == NULL || key == NULL) {
        return NULL;
    }

    // Look if pair already exists
    htab_pair_t* pair = htab_find(t, key);

    // This shouldn't happen
    if (pair != NULL) {
        fprintf(stderr, "Error: Key already exists in the table\n");
        error_exit(ERR_INTERNAL);
    }

    // Create new item
    struct htab_item* item = malloc(sizeof(struct htab_item));
    if (item == NULL) {
        error_exit(ERR_INTERNAL);
    }

    // Initialize item
    item->next = NULL;
    item->pair.key = malloc(strlen(key) + 1);
    if (item->pair.key == NULL) {
        free(item);
        error_exit(ERR_INTERNAL);
    }
    strcpy((char*)item->pair.key, key);
    item->pair.value = value;

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

htab_pair_t* htab_add_function(htab_t* t, token_t* token) {
    token_print(token);
    printf("%p\n", (void*)htab_find(t, token->attr.val_s.val));
    printf("%s\n", token->attr.val_s.val);
    // Check if function is already defined
    if (htab_find(t, token->attr.val_s.val) != NULL) {
        error_exit(ERR_SEM_FUN);
    }
    // Add function to symbol table
    return htab_add(t, token->attr.val_s.val,
                    (htab_value_t){.type = HTAB_FUNCTION,
                                   .function = {
                                       .param_count = 0,
                                   }});
}

void htab_function_add_param(htab_pair_t* fun, token_t* token) {
    fun->value.function.param_count++;
    htab_param_t* params =
        realloc(fun->value.function.params, fun->value.function.param_count * sizeof(htab_param_t));
    if (params == NULL) {
        error_exit(ERR_INTERNAL);
    }
    fun->value.function.params = params;
    fun->value.function.params[fun->value.function.param_count - 1].type = token->type;
    fun->value.function.params[fun->value.function.param_count - 1].required = token->attr.val_b;
}

void htab_function_add_param_name(htab_pair_t* fun, token_t* token) {
    for (int i = 0; i < fun->value.function.param_count - 1; i++) {
        if (strcmp(fun->value.function.params[i].name.val, token->attr.val_s.val) == 0) {
            error_exit(ERR_SEM_FUN);
        }
    }

    fun->value.function.params[fun->value.function.param_count - 1].name =
        str_new_from_str(&token->attr.val_s);
}

size_t htab_size(const htab_t* t) {
    if (t == NULL) {
        return 0;
    }

    return t->size;
}
