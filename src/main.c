#include "clint/clint.h"

#include <getopt.h>
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
    json_object *command_entry = NULL;

    for (unsigned long i = 0; i < json_object_array_length(root); i++) {
        command_entry = json_object_array_get_idx(root, i);

        json_object *directory = json_object_object_get(command_entry, "directory");
        json_object *file = json_object_object_get(command_entry, "file");

        unsigned long path_size = (unsigned long) (json_object_get_string_len(directory) + json_object_get_string_len(file) + 2);

        char *relative_path = (char *) malloc(path_size);

        strcpy(relative_path, json_object_get_string(directory));
        strcat(relative_path, "/");
        strcat(relative_path, json_object_get_string(file));

        char *absolute_path = realpath(relative_path, NULL);

        if (!strncmp(absolute_path, file_path, path_size)) {
            free(relative_path);
            free(absolute_path);
            json_object_get(command_entry);
            break;
        }

        free(relative_path);
        free(absolute_path);
        command_entry = NULL;
    }

    json_object_put(root);
    free(file_path);

    return command_entry;
}

static char * _transform(json_object *command_entry) {
    const char *output = json_object_get_string(json_object_object_get(command_entry, "output"));
    const char * command = json_object_get_string(json_object_object_get(command_entry, "command"));
    token_list *list = (token_list *) token_list_create((char *) command);

    /* Replace output file to /dev/null */
    if (!token_list_replace(list, output, "/dev/null")) {
        token_list_free(list);
        return NULL;
    }

    char *new_command = token_list_bake(list);

    token_list_free(list);

    return new_command;
}

static int _lint_file(char *filename) {
    /* Check if file exists */
    if (access(filename, R_OK)) {
        return CLINT_FILE_NOT_FOUND;
    }

    /* TODO: Search recursively for compilation database */
    if (access(filename, R_OK)) {
        return CLINT_COMP_DB_NOT_FOUND;
    }

    json_object *command_entry = _find_in_comp_db(filename);
    if (!command_entry)
        return CLINT_FILE_NOT_IN_COMP_DB;

    /* Allocate a new string with the length of the command + output
     * and apply transformations */
    char *command = _transform(command_entry);
    if (!command) {
        return CLINT_TOKEN_ERROR;
    }

    /* Run command */
    char buffer[256];
    FILE *fp = popen(command, "r");
    if (!fp)
        return CLINT_COMMAND_ERROR;

    while(fgets(buffer, sizeof(buffer), fp)) {
        printf("%s\n", buffer);
    }

    pclose(fp);
    free(command);
    json_object_put(command_entry);
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
