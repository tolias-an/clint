#include "clint/util.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

token_list* token_list_create(char *str) {
    const char *token = strtok(str, " ");
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

void token_append(token_list *token, const char *value) {
    token_list *next = token->next;
    token_list *new_token = malloc(sizeof *new_token);

    size_t new_value_size = strlen(value) + 1;
    char *new_value = malloc(new_value_size);
    strncpy(new_value, value, new_value_size);
    new_token->token = new_value;

    token->next = new_token;
    new_token->next = next;
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
    const token_list *token = (token_list *) list;

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

char* path_canonicalise(const char *base_path, const char *relative_path) {
    if (!strncmp(relative_path, "/", 1)) {
        return NULL;
    }

    char *big_path = malloc(PATH_MAX);
    if (!big_path) return NULL;

    sprintf(big_path, "%s/%s", base_path, relative_path);

    char *path = malloc(PATH_MAX);
    if (!path) { free(big_path); big_path = NULL; return NULL; }

    const char *token = strtok(big_path, "/");
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

char path_in_env(const char *file) {
    char result = 0;
    /* Duplicate env variable to ensure environment integrity */
    char *env_path = strdup(getenv("PATH"));
    if (!env_path) return result;
    char *path = malloc(PATH_MAX);
    if (!path) { free(env_path); env_path = NULL; return result; }

    const char *path_token = strtok(env_path, ":");

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

char* path_upward_search(const char *filename) {
    char *path = malloc(PATH_MAX);
    if (!path) return NULL;

    char *cwd = getcwd(NULL, PATH_MAX);
    if (!cwd) { free(path); path = NULL; return NULL; }

    while(1) {
        sprintf(path, "%s/%s", cwd, filename);

        if (access(path, R_OK) == 0)
            break;

        *(strrchr(cwd, '/')) = 0;

        if (*cwd == 0) {
            free(path);
            path = NULL;
            break;
        }
    }

    free(cwd); cwd = NULL;
    return path;
}
