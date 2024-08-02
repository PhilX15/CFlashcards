#include <time.h>
#include <math.h>
#include "dict_utils.h"
#include "print_utils.h"
#include "input_utils.h"

enum state_t {
    MAIN_MENU,
    MOD_MENU,
    ADDING_WORDS,
    REMOVING_WORDS,
    REMOVING_TRANSLATION,
    WORD_LIST,
    LEARNING,
    EXIT
} state = MAIN_MENU;

void clear() {
    system("clear");
}

enum err_code_t learning_mode(struct dictionary_t *dict_learned, struct dictionary_t *dict_unlearned) {
    char *input = NULL;
    int total = 0;
    int correct = 0;
    int incorrect = 0;
    do {
        int random = rand() % get_size(dict_unlearned);
        struct word_t *word = get_element(dict_unlearned, random);
        printf("Enter translation of word '%s': ", word->original);
        enum err_code_t input_error;
        input = get_input(&input_error);
        if (input == NULL) {
            return input_error;
        }

        if (strlen(input) == 0) {
            free(input);
            break;
        }

        if (get_translation(dict_unlearned, word->original, input) == NULL) {
            incorrect++;

            printf("Wrong answer!\n");
            print_word(word);
            printf("Press ENTER to continue");
            getchar();
        } else {
            correct++;

            pop_word(dict_unlearned, word->original);
            dict_push_front(dict_learned, word);

            write_dict("unlearned.bin", dict_unlearned);
            write_dict("learned.bin", dict_learned);

            printf("Correct answer!\nPress ENTER to continue");
            getchar();
        }

        clear();
        total++;
        free(input);
    } while (get_size(dict_unlearned) > 0);

    clear();
    print_in_frame("STATS", 2, 1);
    printf("Total words: %d\n", total);
    printf("Correct: %d (%.0f%%)\n", correct, (float) correct / (float) total * 100);
    printf("Incorrect: %d (%.0f%%)\n", incorrect, (float) incorrect / (float) total * 100);

    printf("Press ENTER to go back to menu");
    getchar();
}

int main() {

    srand(time(0));

    enum err_code_t error = ERROR_OK;

    struct dictionary_t *dict_unlearned = read_dict("unlearned.bin", &error);
    switch (error) {
        case ERROR_CANNOT_OPEN_FILE:
            dict_unlearned = init_dict();
            break;
        case ERROR_FAILED_TO_ALLOCATE_MEMORY:
            printf("Failed to allocate memory\n");
            return ERROR_FAILED_TO_ALLOCATE_MEMORY;
    }

    struct dictionary_t *dict_learned = read_dict("learned.bin", &error);
    switch (error) {
        case ERROR_CANNOT_OPEN_FILE:
            dict_learned = init_dict();
            break;
        case ERROR_FAILED_TO_ALLOCATE_MEMORY:
            free_dict(dict_unlearned);
            printf("Failed to allocate memory\n");
            return ERROR_FAILED_TO_ALLOCATE_MEMORY;
    }
    
    int option;
    char *original;
    char *translated;
    
    while (state != EXIT) {
        clear();
        error = ERROR_OK;
        switch (state) {

            case MAIN_MENU:
                print_in_frame("FLASHCARDS", 2, 1);
                printf("\n");
                print_options(4, "Learning mode", "Modification mode", "Display words", "Exit");
                while (get_option(&option, 4) == ERROR_INCORRECT_INPUT) {
                    printf("Incorrect input!\n");
                }

                switch (option) {
                    case 0:
                        state = LEARNING;
                        break;
                    case 1:
                        state = MOD_MENU;
                        break;
                    case 2:
                        state = WORD_LIST;
                        break;
                    case 3:
                        state = EXIT;
                        break;
                }

                break;

            case LEARNING:
                if (dict_unlearned == NULL || get_size(dict_unlearned) == 0) {
                    printf("Nothing to learn\n");
                    printf("Press ENTER to exit");
                    getchar();
                    state = MAIN_MENU;
                    break;
                }
                learning_mode(dict_learned, dict_unlearned);
                state = MAIN_MENU;
                break;

            case MOD_MENU:
                print_options(5, "Add word", "Remove word", "Remove translation", "Go back", "Exit");
                while (get_option(&option, 5) == ERROR_INCORRECT_INPUT) {
                    printf("Incorrect input!\n");
                }

                switch (option) {
                    case 0:
                        state = ADDING_WORDS;
                        break;
                    case 1:
                        state = REMOVING_WORDS;
                        break;
                    case 2:
                        state = REMOVING_TRANSLATION;
                        break;
                    case 3:
                        state = MAIN_MENU;
                        break;
                    case 4:
                        state = EXIT;
                        break;
                }
                break;

            case ADDING_WORDS:
                printf("Enter word:");
                original = get_input(&error);
                if (error == ERROR_FAILED_TO_ALLOCATE_MEMORY) {
                    printf("Failed to allocate memory\n");
                    free_dict(dict_unlearned);
                    free_dict(dict_learned);
                    return ERROR_FAILED_TO_ALLOCATE_MEMORY;
                }

                if (strlen(original) == 0) {
                    state = MOD_MENU;
                    break;
                }

                printf("Enter translated word:");
                translated = get_input(&error);
                if (error == ERROR_FAILED_TO_ALLOCATE_MEMORY) {
                    printf("Failed to allocate memory\n");
                    free_dict(dict_unlearned);
                    free_dict(dict_learned);
                    return ERROR_FAILED_TO_ALLOCATE_MEMORY;
                }

                if (get_translation(dict_learned, original, translated) != NULL) {
                    printf("Word and translation already in dictionary\n");
                    printf("Press ENTER to continue");
                    getchar();
                    break;
                }

                if (get_word(dict_learned, original)) {
                    struct word_t *word = pop_word(dict_learned, original);
                    dict_push_front(dict_unlearned, word);
                    write_dict("learned.bin", dict_learned);
                }

                error = add_word(dict_unlearned, original, translated);
                switch (error) {
                    case ERROR_INCORRECT_PARAMETERS:
                        printf("\nWord and translation already in dictionary\n");
                        printf("Press ENTER to continue");
                        getchar();
                        break;
                    case ERROR_FAILED_TO_ALLOCATE_MEMORY:
                        printf("Failed to allocate memory\n");
                        free_dict(dict_unlearned);
                        free_dict(dict_learned);
                        return ERROR_FAILED_TO_ALLOCATE_MEMORY;
                    case ERROR_OK:
                        write_dict("unlearned.bin", dict_unlearned);
                        free(original);
                        free(translated);
                        printf("\nWord successfully added\n");
                        printf("Press ENTER to continue");
                        getchar();
                        break;
                }

                break;

            case REMOVING_WORDS:
                printf("Enter word:");
                original = get_input(&error);
                if (error == ERROR_FAILED_TO_ALLOCATE_MEMORY) {
                    printf("Failed to allocate memory\n");
                    free_dict(dict_unlearned);
                    free_dict(dict_learned);
                    return ERROR_FAILED_TO_ALLOCATE_MEMORY;
                }

                if (strlen(original) == 0) {
                    state = MOD_MENU;
                    break;
                }

                error = remove_word(dict_unlearned, original);
                if (error == ERROR_INCORRECT_PARAMETERS) {
                    error = remove_word(dict_learned, original);
                    if (error == ERROR_INCORRECT_PARAMETERS) {
                        printf("\nWord not found\n");
                        printf("Press ENTER to continue");
                        getchar();
                        break;
                    }
                    write_dict("learned.bin", dict_unlearned);
                } else {
                    write_dict("unlearned.bin", dict_unlearned);
                }
                
                free(original);
                printf("\nWord successfully removed\n");
                printf("Press ENTER to continue");
                getchar();
                break;

            case REMOVING_TRANSLATION:
                printf("Enter word:");
                original = get_input(&error);
                if (error == ERROR_FAILED_TO_ALLOCATE_MEMORY) {
                    printf("Failed to allocate memory\n");
                    free_dict(dict_unlearned);
                    free_dict(dict_learned);
                    return ERROR_FAILED_TO_ALLOCATE_MEMORY;
                }

                if (strlen(original) == 0) {
                    state = MOD_MENU;
                    break;
                }

                printf("Enter translated word:");
                translated = get_input(&error);
                if (error == ERROR_FAILED_TO_ALLOCATE_MEMORY) {
                    printf("Failed to allocate memory\n");
                    free_dict(dict_unlearned);
                    free_dict(dict_learned);
                    return ERROR_FAILED_TO_ALLOCATE_MEMORY;
                }

                error = remove_translation(dict_unlearned, original, translated);
                if (error == ERROR_INCORRECT_PARAMETERS) {
                    error = remove_translation(dict_learned, original, translated);
                    if (error == ERROR_INCORRECT_PARAMETERS) {
                        printf("Translation not found\n");
                        printf("Press ENTER to continue");
                        getchar();
                        break;
                    }
                    write_dict("learned.bin", dict_unlearned);
                } else {
                    write_dict("unlearned.bin", dict_unlearned);
                }

                printf("Translation successfully removed\n");
                printf("Press ENTER to continue");
                getchar();

                free(original);
                free(translated);
                break;

            case WORD_LIST:
                if (get_size(dict_unlearned) == 0 && get_size(dict_learned) == 0) {
                    printf("Nothing to show\n");
                } else {
                    print_dict(dict_unlearned);
                    print_dict(dict_learned);
                } 
                printf("Press ENTER to exit\n");
                getchar();
                state = MAIN_MENU;
                break;
        }
    }

    free_dict(dict_unlearned);
    free_dict(dict_learned);
    return 0;
}
