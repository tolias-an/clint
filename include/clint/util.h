#ifndef _CLINT_UTIL_H
#define _CLINT_UTIL_H

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
* Parses the token list _list_ and replaces each occurence of _old_
* with _new_. Returns the number of replacements
*/
int token_list_replace(const token_list *list, const char *old, const char *new);

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

#endif /* _CLINT_UTIL_H */
