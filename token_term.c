#include "token_term.h"
#include <stdio.h>
#include "error.h"

token_term_t* token_term_new(token_t value, bool terminal) {
    token_term_t* new = malloc(sizeof(token_term_t));
    if (!new) {
        error_exit(ERR_INTERNAL);
    }
    new->result = value.type;
    new->left = NULL;
    new->right = NULL;

    new->value = value;
    new->terminal = terminal;

    return new;
}

void token_term_pprint(token_term_t token) {
    printf("%s[%d]\n", token_to_string(token.result), token.terminal);
}

void token_graph_print(token_term_t* token, int depth, int foo) {
    if (token == NULL) {
        return;
    }

    if (token->left != NULL) {
        printf("\"%s (%d_%d)\" -> \"%s (%d_%d)\"\n", token_to_string(token->value.type), depth,
               foo, token_to_string(token->left->value.type), depth + 1, foo * 2);
    }
    if (token->right != NULL) {
        printf("\"%s (%d_%d)\" -> \"%s (%d_%d)\"\n", token_to_string(token->value.type), depth,
               foo, token_to_string(token->right->value.type), depth + 1, foo * 2 + 1);
    }
    token_graph_print(token->left, depth + 1, foo * 2);
    token_graph_print(token->right, depth + 1, foo * 2 + 1);
}

void token_term_free(token_term_t* root) {
    if (!root) {
        return;
    }
    token_term_free(root->left);
    token_term_free(root->right);
    root->left = NULL;
    root->right = NULL;

    token_free(&root->value);

    free(root);
}