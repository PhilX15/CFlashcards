#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include "error_handling.h"

enum err_code_t get_option(int *option, int options_available);
char *get_input(enum err_code_t *error);

#endif