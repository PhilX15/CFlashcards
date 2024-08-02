#include "dict_utils.h"

struct dictionary_t *init_dict() {
    struct dictionary_t *result = malloc(sizeof(struct dictionary_t));
    if (result == NULL) {
        return NULL;
    }

    result->head = NULL;

    return result;
}

struct word_t *read_word(FILE *file, enum err_code_t *error) {
    struct word_t *word = malloc(sizeof(struct word_t));
    if (word == NULL) {
        if (error != NULL) {
            *error = ERROR_FAILED_TO_ALLOCATE_MEMORY;
        }
        return NULL;
    }
    word->original = NULL;
    word->head = NULL;
    word->prev = NULL;
    word->next = NULL;

    size_t length = 0;
    if (fread(&length, sizeof(size_t), 1, file) < 1) {
        if (feof(file)) {
            free(word);
            return NULL;
        }
        if (error != NULL) {
            *error = ERROR_FILE_CORRUPTED;
        }
        return NULL;
    }

    word->original = calloc(length + 1, sizeof(char));
    if (word->original == NULL) {
        free(word);
        if (error != NULL) {
            *error = ERROR_FAILED_TO_ALLOCATE_MEMORY;
        }
        return NULL;
    }
    if (fread(word->original, sizeof(char), length, file) < length) {
        free(word->original);
        free(word);
        if (error != NULL) {
            *error = ERROR_FILE_CORRUPTED;
        }
        return NULL;
    }

    *(word->original + length) = '\0';

    length = 1;
    while (length > 0) {
        if (fread(&length, sizeof(size_t), 1, file) < 1) {
            free_word(word);
            if (error != NULL) {
                *error = ERROR_FILE_CORRUPTED;
            }
            return NULL;
        }

        if (length == 0) {
            break;
        }

        struct translation_t *translation = malloc(sizeof(struct translation_t));
        if (translation == NULL) {
            free_word(word);
            if (error != NULL) {
                *error = ERROR_FAILED_TO_ALLOCATE_MEMORY;
            }
            return NULL;
        }
        translation->translation = NULL;
        translation->next = NULL;
        translation->prev = NULL;

        translation->translation = calloc(length + 1, sizeof(char));
        if (translation->translation == NULL) {
            free(translation);
            free_word(word);
            if (error != NULL) {
                *error = ERROR_FAILED_TO_ALLOCATE_MEMORY;
            }
            return NULL;
        }

        if (fread(translation->translation, sizeof(char), length, file) < length) {
            free(translation->translation);
            free(translation);
            free_word(word);
            if (error != NULL) {
                *error = ERROR_FILE_CORRUPTED;
            }
            return NULL;
        }

        word_push_front(word, translation);
    }

    return word;
}

struct dictionary_t *read_dict(char *path, enum err_code_t *error) {
    if (path == NULL) {
        if (error != NULL) {
            *error =ERROR_INCORRECT_PARAMETERS;
        }
        return NULL;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        if (error != NULL) {
            *error = ERROR_CANNOT_OPEN_FILE;
        }
        return NULL;
    }

    struct dictionary_t *dict = init_dict();
    if (dict == NULL) {
        fclose(file);
        if (error != NULL) {
            *error = ERROR_FAILED_TO_ALLOCATE_MEMORY;
        }
        return NULL;
    }

    while (!feof(file)) {
        struct word_t *word = read_word(file, error);
        if (word == NULL && !feof(file)) {
            fclose(file);
            free_dict(dict);
            return NULL;
        }
        dict_push_front(dict, word);
    }
    
    fclose(file);
    if (error != NULL) {
        *error = ERROR_OK;
    }
    return dict;
}

void write_word(FILE *file, struct word_t *word) {
    if (file == NULL || word == NULL) {
        return;
    }

    size_t length = strlen(word->original);
    fwrite(&length, sizeof(size_t), 1, file);
    fwrite(word->original, sizeof(char), length, file);
    struct translation_t *current = word->head;
    while (current != NULL) {
        length = strlen(current->translation);
        fwrite(&length, sizeof(size_t), 1, file);
        fwrite(current->translation, sizeof(char), length, file);
        current = current->next;
    }
    size_t terminator = 0;
    fwrite(&terminator, sizeof(size_t), 1, file);
}

enum err_code_t write_dict(char *path, struct dictionary_t *dict) {
    if (path == NULL || dict == NULL) {
        return ERROR_INCORRECT_PARAMETERS;
    }

    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        return ERROR_CANNOT_CREATE_FILE;
    }

    struct word_t *current = dict->head;
    while (current != NULL) {
        write_word(file, current);
        current = current->next;
    }
    
    fclose(file);
    return ERROR_OK;
}

enum err_code_t add_word(struct dictionary_t *dict, char *word, char *translation) {
    if (dict == NULL || word == NULL || translation == NULL || strlen(word) == 0 || strlen(translation) == 0) {
        return ERROR_INCORRECT_PARAMETERS;
    }

    struct word_t *current = dict->head;
    while (current != NULL) {
        if (strcmp(current->original, word) == 0) {
            struct translation_t *current_translation = current->head;
            while (current_translation != NULL) {
                if (strcmp(current_translation->translation, translation) == 0) {
                    return ERROR_INCORRECT_PARAMETERS;
                }
                if (current_translation->next == NULL) {
                    break;
                }
                current_translation = current_translation->next;
            }
            struct translation_t *translation_struct = malloc(sizeof(struct translation_t));
            if (translation_struct == NULL) {
                return ERROR_FAILED_TO_ALLOCATE_MEMORY;
            }
            translation_struct->translation = strdup(translation);
            if (translation_struct->translation == NULL) {
                free(translation_struct);
                return ERROR_FAILED_TO_ALLOCATE_MEMORY;
            }
            translation_struct->prev = NULL;
            translation_struct->next = current->head;
            current->head->prev = translation_struct;
            current->head = translation_struct;

            return ERROR_OK;
        }
        current = current->next;
    }

    struct word_t *word_struct = malloc(sizeof(struct word_t));
    if (word == NULL) {
        return ERROR_FAILED_TO_ALLOCATE_MEMORY;
    }
    word_struct->original = NULL;
    word_struct->head = NULL;
    word_struct->next = NULL;
    word_struct->prev = NULL;

    struct translation_t *translation_struct = malloc(sizeof(struct translation_t));
    if (translation_struct == NULL) {
        free(word_struct);
        return ERROR_FAILED_TO_ALLOCATE_MEMORY;
    }

    translation_struct->translation = strdup(translation);
    if (translation_struct->translation == NULL) {
        free(word_struct);
        free(translation_struct);
        return ERROR_FAILED_TO_ALLOCATE_MEMORY;
    }

    word_struct->original = strdup(word);
    if (word_struct->original == NULL) {
        free(word_struct);
        free(translation_struct->translation);
        free(translation_struct);
        return ERROR_FAILED_TO_ALLOCATE_MEMORY;
    }

    translation_struct->next = NULL;
    translation_struct->prev = NULL;
    word_struct->head = translation_struct;

    dict_push_front(dict, word_struct);

    return ERROR_OK;
}

struct word_t *pop_word(struct dictionary_t *dict, char *word) {
    if (dict == NULL || word == NULL) {
        return NULL;
    }

    struct word_t *current = dict->head;
    while (current != NULL) {
        if (strcmp(current->original, word) == 0) {
            if (current->prev == NULL) {
                dict->head = current->next;
                if (dict->head != NULL) {
                    dict->head->prev = NULL;
                }
            } else {
                current->prev->next = current->next;
                if (current->next != NULL) {
                    current->next->prev = current->prev;
                }
            }
            return current;
        }
        current = current->next;
    }

    return NULL;
}

enum err_code_t remove_word(struct dictionary_t *dict, char *word) {
    if (dict == NULL || word == NULL) {
        return ERROR_INCORRECT_PARAMETERS;
    }

    struct word_t *current = dict->head;
    while (current != NULL) {
        if (strcmp(current->original, word) == 0) {
            if (current->prev == NULL) {
                dict->head = current->next;
                if (dict->head != NULL) {
                    dict->head->prev = NULL;
                }
            } else {
                current->prev->next = current->next;
                if (current->next != NULL) {
                    current->next->prev = current->prev;
                }
            }
            free_word(current);
            return ERROR_OK;
        }
        current = current->next;
    }

    return ERROR_INCORRECT_PARAMETERS;
}

enum err_code_t remove_translation(struct dictionary_t *dict, char *word, char *translation) {
    if (dict == NULL || word == NULL) {
        return ERROR_INCORRECT_PARAMETERS;
    }

    struct word_t *current = dict->head;
    while (current != NULL) {
        if (strcmp(current->original, word) == 0) {
            struct translation_t *current_translation = current->head;
            while (current_translation != NULL) {
                if (strcmp(current_translation->translation, translation) == 0) {
                    if (current_translation->prev == NULL) {
                        current->head = current_translation->next;
                        if (current->head != NULL) {
                            current->head->prev = NULL;
                        } else {
                            if (current->prev == NULL) {
                                dict->head = current->next;
                                if (dict->head != NULL) {
                                    dict->head->prev = NULL;
                                }
                            } else {
                                current->prev->next = current->next;
                                if (current->next != NULL) {
                                    current->next->prev = current->prev;
                                }
                            }
                            free(current_translation->translation);
                            free(current_translation);
                            free_word(current);
                            return ERROR_OK;
                        }
                    } else {
                        current_translation->prev->next = current_translation->next;
                        if (current_translation->next != NULL) {
                            current_translation->next->prev = current_translation->prev;
                        }
                    }
                    free(current_translation->translation);
                    free(current_translation);
                    return ERROR_OK;
                }
                current_translation = current_translation->next;
            }
            return ERROR_INCORRECT_PARAMETERS;
        }
        current = current->next;
    }

    return ERROR_INCORRECT_PARAMETERS;
}

struct word_t *get_word(struct dictionary_t *dict, char *word) {
    if (dict == NULL || word == NULL) {
        return NULL;
    }

    struct word_t *current = dict->head;
    while (current != NULL) {
        if (strcmp(current->original, word) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

struct translation_t *get_translation(struct dictionary_t *dict, char *word, char *translation) {
    if (dict == NULL || word == NULL || translation == NULL) {
        return NULL;
    }

    struct word_t *current = dict->head;
    while (current != NULL) {
        if (strcmp(current->original, word) == 0) {
            struct translation_t *current_translation = current->head;
            while (current_translation != NULL) {
                if (strcmp(current_translation->translation, translation) == 0) {
                    return current_translation;
                }
                current_translation = current_translation->next;
            }
            return NULL;
        }
        current = current->next;
    }

    return NULL;
}

struct word_t *get_element(struct dictionary_t *dict, int index) {
    if (dict == NULL || index < 0 || index >= get_size(dict)) {
        return NULL;
    }

    struct word_t *current = dict->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    return current;
}

int get_size(struct dictionary_t *dict) {
    if (dict == NULL) {
        return -1;
    }

    int size = 0;
    struct word_t *current = dict->head;

    while (current != NULL) {
        size++;
        current = current->next;
    }

    return size;
}

void print_word(struct word_t *word) {
    if (word == NULL) {
        return;
    }

    struct translation_t *current = word->head;
    while (current != NULL) {
        printf("%s - %s\n", word->original, current->translation);
        current = current->next;
    }
}

void print_dict(struct dictionary_t *dict) {
    if (dict == NULL || dict->head == NULL) {
        return;
    }

    struct word_t *current = dict->head;
    while (current != NULL) {
        print_word(current);
        current = current->next;
    }
}

void free_word(struct word_t *word) {
    if (word == NULL) {
        return;
    }

    while (word->head != NULL) {
        struct translation_t *temp = word->head;
        word->head = word->head->next;
        if (word->head != NULL) {
            word->head->prev = NULL;
        }
        free(temp->translation);
        free(temp);
    }

    if (word->original != NULL) {
        free(word->original);
    }
    free(word);
    word = NULL;
}

void free_dict(struct dictionary_t *dict) {
    if (dict == NULL) {
        return;
    }

    while (dict->head != NULL) {
        struct word_t *temp = dict->head;
        dict->head = dict->head->next;
        if (dict->head != NULL) {
            dict->head->prev = NULL;
        }
        free_word(temp);
    }

    free(dict);
}

void dict_push_front(struct dictionary_t *dict, struct word_t *word) {
    if (dict == NULL || word == NULL) {
        return;
    }

    word->next = dict->head;
    if (dict->head != NULL) {
        dict->head->prev = word;
    }
    word->prev = NULL;
    dict->head = word;
}

void word_push_front(struct word_t *word, struct translation_t *translation) {
    if (word == NULL || translation == NULL) {
        return;
    }

    translation->next = word->head;
    if (word->head != NULL) {
        word->head->prev = translation;
    }
    translation->prev = NULL;
    word->head = translation;
}