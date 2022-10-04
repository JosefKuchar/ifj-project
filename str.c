#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

// Default string size (including \0)
#define DEFAULT_SIZE 8

str_t str_new() {
    // Create new struct
    str_t str = {
        .val = malloc(DEFAULT_SIZE),
        .size = DEFAULT_SIZE,
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

str_t str_new_from_str(str_t* str) {
    // Create new struct
    str_t new_str = {.val = malloc(str->len + 1),  // Just enough to fit
                     .size = str->len + 1,
                     .len = str->len};

    // Check if malloc failed
    if (new_str.val == NULL) {
        error_exit(ERR_INTERNAL);
    }

    // Copy data to new string
    strcpy(new_str.val, str->val);

    return new_str;
}

void str_free(str_t* str) {
    // Free the string
    free(str->val);
    // Reset values just to be sure
    str->val = NULL;
    str->len = 0;
    str->size = 0;
}

void str_add_char(str_t* str, char c) {
    // Enlarge buffer if needed
    if (str->len + 1 >= str->size) {
        char* new_val = realloc(str->val, str->size * 2);
        if (new_val == NULL) {
            error_exit(ERR_INTERNAL);
        }
        str->val = new_val;
        str->size *= 2;
    }

    str->val[str->len] = c;
    str->len++;
    str->val[str->len] = '\0';
}

void str_add_cstr(str_t* str, char* cstr) {
    size_t len = strlen(cstr);
    if (str->len + len + 1 >= str->size) {
        char* new_val = realloc(str->val, str->size + len);
        if (new_val == NULL) {
            error_exit(ERR_INTERNAL);
        }
        str->val = new_val;
        str->size += len;
    }
    str->len += len;
    strcat(str->val, cstr);
}

void str_clear(str_t* str) {
    str->val[0] = '\0';
    str->len = 0;
}

void str_print(str_t* str) {
    printf("%s", str->val);
}
