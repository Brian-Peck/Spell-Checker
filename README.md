# Spell-Checker
A case insensitive spell checker application programmed in C. It uses a hash table with chaining to achieve fast lookup times.

If a user enters a word found in the dictionary, a message will display that the word is correct. If a user types in a word that doesn't exist in the dictionary, the [Levenshtein Distance](https://en.wikipedia.org/wiki/Levenshtein_distance) is calculated and the words associated to the 5 lowest Levenshtein values are displayed to the user as suggestions.


### Instructions

1) Run `make` to compile
2) Run `spellChecker.exe` to execute program
