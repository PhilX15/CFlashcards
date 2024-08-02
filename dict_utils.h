#ifndef DICT_UTILS_H
#define DICT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handling.h"

struct word_t {
    char *original;
    struct translation_t *head;

    struct word_t *next;
    struct word_t *prev;
};

struct translation_t {
    char *translation;

    struct translation_t *next;
    struct translation_t *prev;
};

struct dictionary_t {
    struct word_t *head;
};

struct dictionary_t *init_dict();

struct word_t *read_word(FILE *file, enum err_code_t *error);
struct dictionary_t *read_dict(char *path, enum err_code_t *error);
void write_word(FILE *file, struct word_t *word);
enum err_code_t write_dict(char *path, struct dictionary_t *dict);

enum err_code_t add_word(struct dictionary_t *dict, char *word, char *translation);
struct word_t *pop_word(struct dictionary_t *dict, char *word);
enum err_code_t remove_word(struct dictionary_t *dict, char *word);
enum err_code_t remove_translation(struct dictionary_t *dict, char *word, char *translation);

struct word_t *get_word(struct dictionary_t *dict, char *word);
struct translation_t *get_translation(struct dictionary_t *dict, char *word, char *translation);
struct word_t *get_element(struct dictionary_t *dict, int index);
int get_size(struct dictionary_t *dict);

void print_word(struct word_t *word);
void print_dict(struct dictionary_t *dict);

void free_word(struct word_t *word);
void free_dict(struct dictionary_t *dict);

void dict_push_front(struct dictionary_t *dict, struct word_t *word);
void word_push_front(struct word_t *word, struct translation_t *translation);

#endif