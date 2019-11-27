/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Brian Peck
 * Date: 11-22-19
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink)); //Allocate space for a new HashLink struct called link
    link->key = malloc(sizeof(char) * (strlen(key) + 1)); //Use strlen to get the length of the string. Add 1 for null char at end. Allocate char * string length
    strcpy(link->key, key); //Source is key on the right. Destination is link->key on the left
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity; //Size of the array
    map->size = 0; //Number of links in the table. Obviously starts at 0
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL; //Set each value in table array to NULL
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
	assert(map != 0);
	
	//Delete each struct HashLink in the table
	for (int i = 0; i < map->capacity; i++)
	{
		HashLink* temp = map->table[i]; //Point to the link in the bucket at position i

		while (temp != 0)
		{
			HashLink* del = temp;
			temp = temp->next;
			hashLinkDelete(del);
			map->size--;
		}
	}
	free(map->table);
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
	HashMap* map = malloc(sizeof(HashMap));
	hashMapInit(map, capacity);
	return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
	assert(map != 0);

	int hashIndex = (HASH_FUNCTION(key) % (map->capacity));
	if (hashIndex < 0)
	{
		hashIndex = hashIndex + map->capacity;
	}

	HashLink* temp = map->table[hashIndex];
	
	while (temp != 0)
	{
		if (strcmp(temp->key, key) == 0)
		{
			return &(temp->value);
		}
		else
		{
			temp = temp->next;
		}
	}
	return NULL;
}


/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
	assert(map != 0);

	HashLink** oldTable = map->table; //Temp table
	int oldCapacity = map->capacity;

	//Create new table and set buckets = NULL
	map->table = malloc(sizeof(HashLink*) * capacity);
	for (int i = 0; i < capacity; i++)
	{
		map->table[i] = NULL; //Set each value in table array to NULL
	}

	//Reset size and capacity for new table
	map->size = 0;
	map->capacity = capacity;

	//Move links from old table to new table
	for (int i = 0; i < oldCapacity; i++)
	{
		HashLink* temp = oldTable[i];

		while (temp != 0)
		{
			hashMapPut(map, temp->key, temp->value);
			temp = temp->next;
		}
	}

	//Delete each struct HashLink in the old table
	for (int i = 0; i < oldCapacity; i++)
	{
		HashLink* temp = oldTable[i];

		while (temp != 0)
		{
			hashLinkDelete(temp);
			temp = temp->next;
		}
	}

	//Delete old table
	free(oldTable);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
	assert(map != 0);

	if (hashMapTableLoad(map) >= MAX_TABLE_LOAD)
	{
		resizeTable(map, (map->capacity * 2));
	}

	int hashIndex = (HASH_FUNCTION(key) % (map->capacity));
	if (hashIndex < 0)
	{
		hashIndex = hashIndex + map->capacity;
	}

	HashLink* temp = map->table[hashIndex];
	HashLink* beginning = map->table[hashIndex];
	if (temp == NULL) //No link at current bucket
	{
		map->table[hashIndex] = hashLinkNew(key, value, NULL);
		map->size++;
	}
	else
	{
		while (temp != 0)
		{
			if (strcmp(temp->key, key) == 0)
			{
				temp->value = value;
				temp = 0;
				beginning = 0;
			}
			else if (temp->next == 0)
			{
				map->table[hashIndex] = hashLinkNew(key, value, NULL);
				map->table[hashIndex]->next = beginning;
				map->size++;
				temp = 0;
				beginning = 0;
			}
			else
			{
				temp = temp->next;
			}
		}
	}
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
	assert(map != 0);

	int hashIndex = (HASH_FUNCTION(key) % (map->capacity));
	if (hashIndex < 0)
	{
		hashIndex = hashIndex + (map->capacity);
	}

	HashLink* curr = map->table[hashIndex];
	HashLink* prev = NULL;

	while (curr != 0)
	{
		if (strcmp(curr->key, key) == 0) //Found a key match
		{
			if (curr->next == NULL && prev == NULL) //Only 1 link in bucket
			{
				hashLinkDelete(curr);
				map->table[hashIndex] = NULL;
				map->size--;
			}
			else if (curr->next != NULL && prev == NULL) //Delete first link. Subsequent links do exist
			{
				map->table[hashIndex] = curr->next;
				hashLinkDelete(curr);
				map->size--;
			}
			else if (curr->next == NULL && prev != NULL) //Deleting last link in the list
			{
				prev->next = NULL;
				hashLinkDelete(curr);
				map->size--;
				prev = NULL;
			}
			else if (curr->next != NULL && prev != NULL) //Removing a link in the middle
			{
				prev->next = curr->next;
				hashLinkDelete(curr);
				map->size--;
				prev = NULL;
			}
		}
		else
		{
			prev = curr;
			curr = curr->next;
		}
	}
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
	assert(map != 0);

	int hashIndex = (HASH_FUNCTION(key) % (map->capacity));
	if (hashIndex < 0)
	{
		hashIndex = hashIndex + map->capacity;
	}

	HashLink* temp = map->table[hashIndex];

	while (temp != 0)
	{
		if (strcmp(temp->key, key) == 0)
		{
			return 1;
		}
		else
		{
			temp = temp->next;
		}
	}
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
	assert(map != 0);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
	assert(map != 0);
	return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
	assert(map != 0);

	int emptyBuckets = 0;
	for (int i = 0; i < map->capacity; i++)
	{
		if (map->table[i] == NULL)
		{
			emptyBuckets++;
		}
	}
    return emptyBuckets;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
	assert(map != 0);
	return ((float)map->size / (float)map->capacity);   
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
	assert(map != 0);

	for (int i = 0; i < map->capacity; i++)
	{
		HashLink* temp = map->table[i];

		while (temp != 0)
		{
			printf("%d ", temp->value);
			temp = temp->next;
		}
	}
}
