#include "input_utils.h"

enum err_code_t get_option(int *option, int options_available) {
    printf("Choose option: ");

    if (scanf("%d", option) < 1) {
        while(getchar() != '\n');
        return ERROR_INCORRECT_INPUT;
    }

    if (*option < 0 || *option >= options_available) {
        return ERROR_INCORRECT_INPUT;
    }

    while(getchar() != '\n');

    return ERROR_OK;
}

char *get_input(enum err_code_t *error) {
    char *input = calloc(1, sizeof(char));
    if (input == NULL) {
        if (error != NULL) {
            *error = ERROR_FAILED_TO_ALLOCATE_MEMORY;
        }
        return NULL;
    }

    int ch;
    int length = 0;
    while ((ch = getchar()) && ch != '\n') {
        char *input_temp = realloc(input, (length + 2));
        if (input_temp == NULL) {
            free(input);
            if (error != NULL) {
                *error = ERROR_FAILED_TO_ALLOCATE_MEMORY;
            }
            return NULL;
        }

        input = input_temp;
        *(input + length) = ch;
        *(input + length + 1) = '\0';

        length++;
    }

    if (error != NULL) {
        *error = ERROR_OK;
    }
    return input;
}