#ifndef _CLINT_UTIL_H
#define _CLINT_UTIL_H

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

typedef struct token_list token_list;

struct token_list {
    token_list *next;
    char *token;
} ;

/*
* Creates a singly-linked token list from _str_ separated by spaces
* Should be freed with token_list_free
*/
token_list* token_list_create(char *str);

/*
* Replaces _old_ with _new_
*/
int token_replace(token_list *token, const char *old, const char *new);

/*
* Parses the token list _list_ and returns a string from its tokens
* It does not free the token list and the caller is expected to free
* the result
*/
char* token_list_bake(const token_list *list);

/*
* Frees token list _list_
*/
void token_list_free(token_list *list);

/*
* Checks if _file_ is in the PATH environment variable. Returns 1 if found
* and zero if not found
*/
char in_path(const char *file);

/*
* Calculates the absolute path by appending _relative_path_ to
* _base_path_. Result is malloc'd. Returns NULL if _relative_path_ is
* already canonical
*/
char* canonicalise_path(const char *base_path, const char *relative_path);

#endif /* _CLINT_UTIL_H */
