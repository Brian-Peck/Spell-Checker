#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Calculates and returns the levenshtein distance between two strings
 * Source Cited: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
 * @param string 1
 * @param string 2
 * @return levenshtein distance between two strings in integer form
 */
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char* s1, char* s2) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = strlen(s1);
	s2len = strlen(s2);
	unsigned int column[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	return(column[s1len]);
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
	char* key = nextWord(file);

	while (key != NULL)
	{
		hashMapPut(map, key, -1);
		free(key);
		key = nextWord(file);
	}
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

		//Convert word to lower case to satisfy assignment requirement
		//that the spell checker is case insensitive
		for (int i = 0; inputBuffer[i]; i++)
		{
			inputBuffer[i] = tolower(inputBuffer[i]);
		}

		if (strcmp(inputBuffer, "quit") == 0)
		{
			quit = 1;
		}
		else if (hashMapContainsKey(map, inputBuffer) == 1) //Correctly spelled word
		{
			printf("\nThe inputted word %s is spelled correctly. \n", inputBuffer);
		}
		else //User input an invalid word. Provide 5 alternatives based on Levenshtein score
		{
			//Array to hold 5 most similar words to user input. Each value initialized to NULL.
			HashLink** alternateWords = malloc(sizeof(HashLink*) * 5);

			for (int i = 0; i < 5; i++)
			{
				alternateWords[i] = NULL;
			}
			
			//Loop through each word in dictionary
			for (int i = 0; i < map->capacity; i++)
			{
				HashLink* temp = map->table[i];

				while (temp != 0)
				{
					//Calculate levenshtein distance between user input and current dictionary word
					int levDistance = levenshtein(inputBuffer, temp->key);

					for (int i = 0; i < 5; i++)
					{
						if (alternateWords[i] == 0) //No words yet being suggested
						{
							alternateWords[i] = malloc(sizeof(HashLink));
							alternateWords[i]->key = malloc(sizeof(char) * 256);
							strcpy(alternateWords[i]->key, temp->key);
							alternateWords[i]->value = levDistance;
							alternateWords[i]->next = NULL;

							break;
						}
						else if (levDistance < alternateWords[i]->value) //Current word in dictionary has smaller levenshtein distance than a word being suggested
						{
							strcpy(alternateWords[i]->key, temp->key);
							alternateWords[i]->value = levDistance;

							break;
						}
					}
					temp = temp->next;
				}
			}

			//Display alterante words to user
			printf("\nThe inputted word %s is spelled incorrectly.\n", inputBuffer);
			printf("Did you mean ");
			for (int i = 0; i < 5; i++)
			{
				if (i < 4)
				{
					printf("%s, ", alternateWords[i]->key);
				}
				else
				{
					printf("or %s?\n", alternateWords[i]->key);
				}
			}

			//Free allocated memory
			for (int i = 0; i < 5; i++)
			{
				free(alternateWords[i]->key);
				alternateWords[i]->key = NULL;

				free(alternateWords[i]);
				alternateWords[i] = NULL;
			}
			free(alternateWords);
		}
    }
    hashMapDelete(map);
    return 0;
}
