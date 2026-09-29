/**
* Project: IFJ26 Compiler
 *
 * @file hashtable.c
 * @brief Implementation of hashtable
 * @author Lukas Zelenak (xzelenl00)
 */

#include "hashtable.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    void *value;
    bool used;
} HashTableItem;

struct HashTable {
    HashTableItem *items;
    size_t size;
    size_t count;
};

HashTable *hashTableInit(void) { return NULL; }

HashTableInsertResult hashTableInsert(HashTable *table, const char *key,
                                      void *value) {
    return HASHTABLE_ITEM_ALLOC_FAILED;
}

void *hashTableSearch(const HashTable *table, const char *key) { return NULL; }

void hashTableFree(HashTable *table) {}