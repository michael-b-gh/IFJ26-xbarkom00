/**
 * Project: IFJ26 Compiler
 *
 * @file hashtable.h
 * @brief Key-value lookup table
 * @author Lukas Zelenak (xzelenl00)
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stddef.h>

typedef struct HashTable HashTable;

/**
 * @brief Result from hashTableInsert function
 */
typedef enum {
  HASHTABLE_ITEM_INSERTED,    /**< Item is inserted successfully */
  HASHTABLE_ITEM_EXISTS,      /**< Item already exists in hashtable */
  HASHTABLE_ITEM_ALLOC_FAILED /**< Memory allocation failed */
} HashTableInsertResult;

/**
 * @brief Allocates empty hashtable.
 *
 * @return Pointer to the new table, or NULL if allocation fails.
 */
HashTable *hashTableCreate(void);

/**
 * @brief Inserts a new item into the hashtable
 *
 * @param table Table to insert into.
 * @param key Name under which the item is stored, ends with '\0', must not be
 * NULL, the table owns its own copy.
 * @param value Pointer to data, associated with the key, only the pointer is
 * saved.
 *
 * @return HASHTABLE_ITEM_INSERTED If item is successfully inserted.
 *         HASHTABLE_ITEM_EXISTS If the key already exists in the table.
 *         HASHTABLE_ITEM_ALLOC_FAILED If allocation failed.
 */
HashTableInsertResult hashTableInsert(HashTable *table, const char *key,
                                      void *value);

/**
 * @brief Searches the table for item with the given key.
 *
 * @param table Table to search in.
 * @param key Name to search for, must not be NULL.
 *
 * @return Value If it is found.
 *         NULL If it is not found
 */
void *hashTableSearch(const HashTable *table, const char *key);

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
void hashTableFree(HashTable *table);

/**
 * @brief Returns hashtable capacity.
 *
 * @param table Table to get capacity of.
 *
 * @return Capacity of hashtable, if table is NULL returns 0.
 */
size_t hashTableSize(const HashTable *table);

/**
 * @brief Value of used item in hashtable, together with hashTableSize function
 * it can be used to iterate over whole table.
 *
 * @param table Table to get value of an item in it.
 * @param index Index of the item in hashtable.
 *
 * @return If table is NULL returns NULL.
 * If index is greater than or equal to size returns NULL.
 * If item on index is used returns its value.
 * If item on index is not used returns NULL.
 */
void *hashTableIndexValue(const HashTable *table, size_t index);

#endif
