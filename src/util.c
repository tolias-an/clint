#include "clint/util.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int token_replace(token_list *token, const char *old, const char *new) {
    if (!strncmp(token->token, old, strlen(token->token))) {
        free(token->token);
        token->token = strdup(new);
        return 1;
    }

    return 0;
}

char *token_list_bake(const token_list *list) {
    char *result = malloc(PATH_MAX);
    size_t index = 0;
    token_list *token = (token_list *) list;

    while(token) {
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

char in_path(const char *file) {
    char result = 0;
    /* Duplicate env variable to ensure environment integrity */
    char *env_path = strdup(getenv("PATH"));
    if (!env_path) return result;
    char *path = malloc(PATH_MAX);
    if (!path) { free(env_path); env_path = NULL; return result; }

    char *path_token = strtok(env_path, ":");

    while (path_token) {
        sprintf(path, "%s/%s", path_token, file);

        if (!access(path, X_OK)) {
            result = 1;
            break;
        }

        path_token = strtok(NULL, ":");
    }

    free(env_path); env_path = NULL;
    free(path); path = NULL;

    return result;
}

char* canonicalise_path(const char *base_path, const char *relative_path) {
    if (!strncmp(relative_path, "/", 1)) {
        return NULL;
    }

    char *big_path = malloc(PATH_MAX);
    if (!big_path) return NULL;

    sprintf(big_path, "%s/%s", base_path, relative_path);

    char *path = malloc(PATH_MAX);
    if (!path) { free(big_path); big_path = NULL; return NULL; }

    char *token = strtok(big_path, "/");
    char *path_ptr = path;

    while(token) {
        if (!strcmp(token, "..")) {
            path_ptr = strrchr(path, '/');
        } else if (!strcmp(token, ".")) {
            ;
        } else {
            int chars_written = sprintf(path_ptr, "/%s", token);
            path_ptr += chars_written;
        }

        token = strtok(NULL, "/");
    }

    free(big_path); big_path = NULL;

    return path;
}
