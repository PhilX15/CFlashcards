#include "print_utils.h"

void print_in_frame(char *text, int margin_x, int margin_y) {
    if (text == NULL || margin_x < 0 || margin_y < 0) {
        return;
    }

    int lines = get_lines(text);
    int max_line_length = get_max_line_length(text);
    int i = 0;

    for (size_t y = 0; y < 2 * margin_y + lines + 2; y++) {
        if (y == 0 || y == 2 * margin_y + lines + 1) {
            for (size_t x = 0; x < max_line_length + 2 * margin_x + 2; x++) {
                printf("#");
            }   
        } else if (y <= margin_y || y >= 2 * margin_y + lines + 1 - margin_y) {
            printf("#");
            for (size_t x = 0; x < max_line_length + 2 * margin_x; x++) {
                printf(" ");
            }
            printf("#");
        } else {
            int line_length = get_line_length(text);
            int margin_left = max_line_length + margin_x * 2 - line_length;
            int margin_right = (margin_left + 1) / 2;
            margin_left /= 2;

            printf("#");
            for (size_t x = 0; x < margin_left; x++) {
                printf(" ");
            }
            print_line(text);
            text += line_length + 1;
            for (size_t x = 0; x < margin_right; x++) {
                printf(" ");
            }
            printf("#");
        }
        printf("\n");
    }
}

void print_options(int n, ...) {
    if (n <= 0) {
        return;
    }

    va_list args;
    va_start(args, n);

    for (int i = 0; i < n; i++) {
        printf("[%d] %s\n", i, va_arg(args, char*));
    }
}

void print_line(char *text) {
    for (int i = 0; *(text + i) != '\n' && *(text + i) != '\0'; i++) {
        printf("%c", *(text + i));
    }
}

int get_lines(char *text) {
    if (text == NULL) {
        return -1;
    }

    int lines = 1;

    for (int i = 0; *(text + i) != '\0'; i++) {
        if (*(text + i) == '\n') {
            lines++;
        }
    }

    return lines;
}

int get_max_line_length(char *text) {
    if (text == NULL) {
        return -1;
    }

    int max_line_length = 0;

    for (int i = 0; i < strlen(text); i++) {
        int line_length = get_line_length(text + i);
        if (line_length > max_line_length) {
            max_line_length = line_length;
        }
        i += line_length;
    }

    return max_line_length;
}

int get_line_length(char *text) {
    if (text == NULL) {
        return -1;
    }

    int line_length;
    for (line_length = 0; *(text + line_length) != '\n' && *(text + line_length) != '\0'; line_length++);

    return line_length;
}