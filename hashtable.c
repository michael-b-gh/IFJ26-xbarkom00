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

#define HASHTABLE_INITIAL_SIZE 100 /**< Initial size of hashtable */

/**
 * @brief Hashes the key to index by using DJB2 (Dan Bernstein).
 *
 * @param key Name under which the item is stored.
 * @param size size of the table, the result is in interval <0,size-1>
 *
 * @return Starting index for searching or inserting.
 */
static size_t hashingFunction(const char *key, size_t size);

/**
 * @brief Doubles the size of the hashtable and rehashes all items.
 *
 * @param table Table that will be resized.
 *
 * @return true If allocation was successful.
 *         false If allocation was unsuccessful, old table is left unchanged.
 */
static bool resizingFunction(HashTable *table);

/**
 * @brief One item in hashtable.
 */
typedef struct {
  char *key;   /**< Copy of key */
  void *value; /**< Value of the item */
  bool used;   /**< True if item on index is used */
} HashTableItem;

/**
 * @brief Hashtable with implicit chaining
 */
struct HashTable {
  HashTableItem *items; /**< Array of items in hashtable */
  size_t size;          /**< Number of items in array */
  size_t count;         /**< Number of occupied items */
};

static size_t hashingFunction(const char *key, size_t size) {
  size_t value = 5381;
  for (size_t i = 0; key[i] != '\0'; i++) {
    value = value * 33 + (unsigned char)key[i];
  }
  return value % size;
}

static bool resizingFunction(HashTable *table) {
  size_t newSize = table->size * 2;
  HashTableItem *newItems = calloc(newSize, sizeof(HashTableItem));
  if (newItems == NULL)
    return false;
  for (size_t i = 0; i < table->size; i++) {
    if (table->items[i].used) {
      size_t index = hashingFunction(table->items[i].key, newSize);
      while (newItems[index].used) {
        index = (index + 1) % newSize;
      }
      newItems[index] = table->items[i];
    }
  }
  free(table->items);

  table->items = newItems;
  table->size = newSize;

  return true;
}

HashTable *hashTableCreate(void) {
  HashTable *table = malloc(sizeof(HashTable));
  if (table == NULL)
    return NULL;
  table->items = calloc(HASHTABLE_INITIAL_SIZE, sizeof(HashTableItem));
  if (table->items == NULL) {
    free(table);
    return NULL;
  }

  table->size = HASHTABLE_INITIAL_SIZE;
  table->count = 0;
  return table;
}

HashTableInsertResult hashTableInsert(HashTable *table, const char *key,
                                      void *value) {
  size_t length = strlen(key);
  if ((table->count + 1) * 10 > table->size * 7) {
    if (!resizingFunction(table))
      return HASHTABLE_ITEM_ALLOC_FAILED;
  }

  size_t index = hashingFunction(key, table->size);

  for (size_t i = 0; i < table->size; i++) {
    if (!table->items[index].used) {
      char *copy = malloc(length + 1);
      if (copy == NULL)
        return HASHTABLE_ITEM_ALLOC_FAILED;
      strcpy(copy, key);
      table->items[index].key = copy;
      table->items[index].value = value;
      table->items[index].used = true;
      table->count++;
      return HASHTABLE_ITEM_INSERTED;
    } else if (strcmp(key, table->items[index].key) == 0) {
      return HASHTABLE_ITEM_EXISTS;
    } else {
      index = (index + 1) % table->size;
    }
  }
  return HASHTABLE_ITEM_ALLOC_FAILED;
}

void *hashTableSearch(const HashTable *table, const char *key) {
  size_t index = hashingFunction(key, table->size);
  for (size_t i = 0; i < table->size; i++) {
    if (!table->items[index].used) {
      return NULL;
    } else if (strcmp(key, table->items[index].key) == 0) {
      return table->items[index].value;
    } else {
      index = (index + 1) % table->size;
    }
  }
  return NULL;
}

void hashTableFree(HashTable *table) {
  if (table == NULL)
    return;
  for (size_t i = 0; i < table->size; i++) {
    if (table->items[i].key) {
      free(table->items[i].key);
    }
  }
  free(table->items);
  free(table);
}

size_t hashTableSize(const HashTable *table) {
  if (table == NULL)
    return 0;
  return table->size;
}

void *hashTableIndexValue(const HashTable *table, size_t index) {
  if (table == NULL)
    return NULL;
  if (index >= table->size)
    return NULL;
  if (table->items[index].used) {
    return table->items[index].value;
  }
  return NULL;
}
