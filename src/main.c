#include "clint/clint.h"

#include <limits.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <json-c/json_util.h>
#include <json-c/json_object.h>

#include "clint/util.h"

static json_object* _find_in_comp_db(char *filename) {
    json_object *root = json_object_from_file("compile_commands.json");
    if (!root)
        return NULL;

    /* Get filename path */
    char *file_path = realpath(filename, NULL);
    char *relative_path = malloc(PATH_MAX);
    char *absolute_path = malloc(PATH_MAX);
    json_object *command_entry = NULL;

    for (unsigned long i = 0; i < json_object_array_length(root); i++) {
        command_entry = json_object_array_get_idx(root, i);

        json_object *directory = json_object_object_get(command_entry, "directory");
        json_object *file = json_object_object_get(command_entry, "file");

        sprintf(relative_path, "%s/%s",
                json_object_get_string(directory), json_object_get_string(file));

        realpath(relative_path, absolute_path);

        if (!strncmp(absolute_path, file_path, PATH_MAX)) {
            json_object_get(command_entry);
            break;
        }

        command_entry = NULL;
    }

    json_object_put(root); root = NULL;
    free(file_path); file_path = NULL;
    free(absolute_path); absolute_path = NULL;
    free(relative_path); relative_path = NULL;

    return command_entry;
}

static char * _transform(json_object *command_entry) {
    const char *directory = json_object_get_string(json_object_object_get(command_entry, "directory"));
    const char *file = json_object_get_string(json_object_object_get(command_entry, "file"));
    const char *output = json_object_get_string(json_object_object_get(command_entry, "output"));
    const char *command = json_object_get_string(json_object_object_get(command_entry, "command"));

    char *absolute_path = canonicalise_path(directory, file);

    token_list *list = token_list_create((char *) command);
    token_list *token = list;

    while (token) {
        /* Replace input file with the file's absolute path */
        token_replace(token, file, absolute_path);

        /* Replace include paths */
        if (!strncmp(token->token, "-I", 2)) {
            char *include_path = canonicalise_path(directory, token->token + 2);

            if (include_path) {
                char *include_command = malloc(strlen(include_path) + 3);

                sprintf(include_command, "-I%s", include_path);
                token_replace(token, token->token, include_command);
                free(include_command);
                free(include_path);
            }
        }

        /* Replace output file with /dev/null */
        token_replace(token, output, "/dev/null");

        /* Replace paths */
        if (strncmp(token->token, "-", 1)) {
            /* Leave as is if it is path */
            if (!in_path(token->token)) {
                char *path = canonicalise_path(directory, token->token);

                if (path) {
                    token_replace(token, token->token, path);
                    free(path);
                }
            }
        }

        token = token->next;
    }

    char *new_command = token_list_bake(list);

    free(absolute_path); absolute_path = NULL;
    token_list_free(list); list = NULL;

    return new_command;
}

static int _lint_file(char *filename) {
    /* Check if file exists */
    if (access(filename, R_OK)) {
        return CLINT_FILE_NOT_FOUND;
    }

    /* TODO: Search recursively for compilation database */

    json_object *command_entry = _find_in_comp_db(filename);
    if (!command_entry)
        return CLINT_FILE_NOT_IN_COMP_DB;

    /* Allocate a new string with the length of the command + output
     * and apply transformations */
    char *command = _transform(command_entry);
    if (!command) {
        json_object_put(command_entry);
        return CLINT_TOKEN_ERROR;
    }

    /* Run command */
    FILE *fp = popen(command, "r");
    if (!fp) {
        json_object_put(command_entry);
        free(command);
        return CLINT_COMMAND_ERROR;
    }

    char buffer[512];

    while(fgets(buffer, 512, fp)) {
        printf("%s\n", buffer);
    }

    pclose(fp);
    json_object_put(command_entry);
    free(command);
    return CLINT_OK;
}

int main(int argc, char *argv[]) {
    int index, err = CLINT_OK;

    for (index = optind; index < argc; index++) {
        err = _lint_file(argv[index]);
        if (err)
            break;
    }

    switch (err) {
        case CLINT_OK:
            break;
        case CLINT_FILE_NOT_FOUND:
            fprintf(stderr, "Error: File %s was not found\n", argv[index]);
            break;
        case CLINT_COMP_DB_NOT_FOUND:
            fprintf(stderr, "Error: compile_commands.json was not found\n");
            break;
        case CLINT_FILE_NOT_IN_COMP_DB:
            fprintf(stderr, "Error: File %s was not found in compile_commands.json\n", argv[index]);
            break;
        case CLINT_TOKEN_ERROR:
            fprintf(stderr, "Error: Tokenising failed\n");
            break;
        case CLINT_COMMAND_ERROR:
            fprintf(stderr, "Error: Unable to reproduce command\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown error\n");
            break;
    }
    return err;
}
