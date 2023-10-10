#ifndef _CLINT_H
#define _CLINT_H

#define _GNU_SOURCE

enum clint_codes {
    CLINT_OK,
    CLINT_FILE_NOT_FOUND,
    CLINT_COMP_DB_NOT_FOUND,
    CLINT_FILE_NOT_IN_COMP_DB,
    CLINT_TOKEN_ERROR,
    CLINT_COMMAND_ERROR,
};

#endif /* _CLINT_H */
