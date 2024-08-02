#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

enum err_code_t {
    ERROR_OK,
    ERROR_INCORRECT_INPUT,
    ERROR_INCORRECT_PARAMETERS,
    ERROR_CANNOT_OPEN_FILE,
    ERROR_CANNOT_CREATE_FILE,
    ERROR_FILE_CORRUPTED,
    ERROR_FAILED_TO_ALLOCATE_MEMORY,
    ERROR_WORD_NOT_FOUND
};

#endif