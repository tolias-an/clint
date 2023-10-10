#include "clint/util.h"

#include <stdlib.h>
#include <string.h>

token_list* token_list_create(char *str) {
    char *token = strtok(str, " ");
    if (!token) return NULL;

    token_list *list = malloc(sizeof * list);

    list->token = strdup(token);

    token_list *current = list;
    while (1) {
        token = strtok(NULL, " ");
        if (!token) break;

        token_list *next = malloc(sizeof * next);

        next->token = strdup(token);
        next->next = NULL;

        current->next = next;
        current = next;
    }

    return list;
}

int token_list_replace(const token_list *list, const char *old, const char *new) {
    token_list *token = (token_list *) list;
    int replaced = 0;

    while(token) {
        if (strncmp(token->token, old, strlen(token->token))) {
            token = token->next;
            continue;
        }

        free(token->token);
        token->token = strdup(new);
        replaced++;
    }

    return replaced;
}

char *token_list_bake(const token_list *list) {
    /* Allocated memory for the result starts from 32 bytes and doubles as needed. */
    size_t current_size = 32;
    char *result = malloc(current_size);
    size_t index = 0;
    token_list *token = (token_list *) list;

    while(token) {
        if (strlen(token->token) > current_size - index) {
            current_size *= 2;

            char *tmp = realloc(result, current_size);
            if (!tmp) {
                free(result);
                result = NULL;
                return NULL;
            }

            result = tmp;
        }

        memcpy(result + index, token->token, strlen(token->token));
        index += strlen(token->token);
        result[index++] = ' ';
        token = token->next;
    }

    index = index ? index - 1 : 0;
    result[index] = '\0';

    return result;
}

void token_list_free(token_list *list) {
    while (list) {
        token_list *next = list->next;
        free(list->token);
        list->token = NULL;
        list->next = NULL;
        free(list);
        list = next;
    }
}
