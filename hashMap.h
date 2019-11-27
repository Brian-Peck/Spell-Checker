#ifndef HASH_MAP_H
#define HASH_MAP_H

/*
 * CS 261 Data Structures
 * Assignment 5
 */

#define HASH_FUNCTION hashFunction1
#define MAX_TABLE_LOAD 1

typedef struct HashMap HashMap; //Can type HashMap instead of struct HashMap
typedef struct HashLink HashLink; //Can type HashLink instead of struct HashLink

struct HashLink
{
    char* key;
    int value;
    HashLink* next;
};

struct HashMap
{
    HashLink** table; //Array of pointers to link structs
	int size; //Number of links in the table
    int capacity; //Amount of space in the array
};

HashMap* hashMapNew(int capacity);
void hashMapDelete(HashMap* map);
int* hashMapGet(HashMap* map, const char* key);
void hashMapPut(HashMap* map, const char* key, int value);
void hashMapRemove(HashMap* map, const char* key);
int hashMapContainsKey(HashMap* map, const char* key);

int hashMapSize(HashMap* map);
int hashMapCapacity(HashMap* map);
int hashMapEmptyBuckets(HashMap* map);
float hashMapTableLoad(HashMap* map);
void hashMapPrint(HashMap* map);

#endif