# Flashcards #
Learn vocabulary with Flashcards made in C.

## How it works? ##
### Modification Mode ###
Modification Mode is a mode where you can add and remove words or translations to your dictionary.
#### Adding words ####
In Modification Mode, select the `Add Word` option, then enter the word in a language you know and provide its translation in a foreign language. The new word will be automatically saved to the file if possible.
If you add a word that is already in the dictionary but has a different translation than the one given in the dictionary, the new translation will be added to the translation list.
#### Removing words ####
In Modification Mode, select the `Remove Word` option, then enter the word you want to remove from the dictionary in a language you know. This will remove the word with all translations and automatically update the files if possible.
#### Removing translations ####
In Modification Mode, select the `Remove Translation` option, then enter the word in a language you know, then enter the translation of that word you want to remove. This will remove the translation and automatically update the files if possible.

----------

### Learning Mode ###
Learning Mode is a mode where you can learn words from your dictionary.
It displays a word in your language and waits for input. If you enter a correct translation, the word will be marked as learned and will not appear again.

----------

### To-Do List ###
List of features that will be added in the near future.

- Vocabulary consolidating (In learning mode, learned words sometimes appear among unlearned words and may be marked as unlearned again if they are not translated correctly)
- Reading .txt files with words and translations
