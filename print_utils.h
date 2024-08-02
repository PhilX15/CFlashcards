#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

void print_in_frame(char *text, int margin_x, int margin_y);
void print_options(int n, ...);
void print_line(char *text);

int get_lines(char *text);
int get_max_line_length(char *text);
int get_line_length(char *text);

#endif