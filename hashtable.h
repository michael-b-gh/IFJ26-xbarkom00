/**
* Project: IFJ26 Compiler
 *
 * @file hashtable.h
 * @brief Key-value lookup table
 * @author Lukas Zelenak (xzelenl00)
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct HashTable HashTable;

typedef enum {
    HASHTABLE_ITEM_INSERTED,
    HASHTABLE_ITEM_EXISTS,
    HASHTABLE_ITEM_ALLOC_FAILED
  } HashTableInsertResult;

/**
 * @brief Allocates empty hashtable.
 *
 * @return Pointer to the new table, or NULL if allocation fails.
 */
HashTable *hashTableInit(void);

/**
 * @brief Inserts a new item into the hashtable.
 *
 * @param table Table to insert into.
 * @param key Name under which the item is stored, ends with '\0', must not be
 * NULL, the table owns its own copy .
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

#endif
