/**
 * Project: IFJ26 Compiler
 *
 * @file symtable.c
 * @brief Implementation of symbol table
 * @author Lukas Zelenak (xzelenl00)
 */

#include "error.h"
#include "hashtable.h"
#include "symtable.h"

#include <stdlib.h>

#define SYMTABLE_INITIAL_SIZE 8 /**< Initial capacity of scopes */

/**
 * @brief Frees scope and all symbols in the same scope
 *
 * @param scope scope to be freed
 */
static void freeScope(HashTable *scope);

/**
 * @brief Stack for scopes, one scope is one hashtable, scopes[0] is global
 */
struct SymTable {
  HashTable **scopes; /**< Array of pointers to hashtable */
  size_t count;       /**< Number of open scopes */
  size_t size;        /**< Capacity of scopes array */
};

static void freeScope(HashTable *scope) {
  size_t htSize = hashTableSize(scope);
  for (size_t i = 0; i < htSize; i++) {
    Symbol *symbol = hashTableIndexValue(scope, i);
    if (symbol != NULL) {
      free(symbol->paramTypes);
      free(symbol);
    }
  }
  hashTableFree(scope);
}

SymTable *symTableCreate(void) {
  SymTable *symtable = malloc(sizeof(SymTable));
  if (symtable == NULL) {
    exit(INT_ERR);
  }
  symtable->scopes = calloc(SYMTABLE_INITIAL_SIZE, sizeof(HashTable *));
  if (symtable->scopes == NULL) {
    free(symtable);
    exit(INT_ERR);
  }
  HashTable *global = hashTableCreate();
  if (global == NULL) {
    free(symtable->scopes);
    free(symtable);
    exit(INT_ERR);
  }
  symtable->scopes[0] = global;
  symtable->size = SYMTABLE_INITIAL_SIZE;
  symtable->count = 1;

  DType printParams[] = {DTYPE_ANY};
  symTableDefineFunction(symtable, "print", 1, printParams, DTYPE_UNIT, false);

  symTableDefineFunction(symtable, "readLine", 0, NULL, DTYPE_STRING, false);

  symTableDefineFunction(symtable, "readInt", 0, NULL, DTYPE_INT, false);

  symTableDefineFunction(symtable, "readDouble", 0, NULL, DTYPE_DOUBLE, false);

  DType toStrParams[] = {DTYPE_ANY};
  symTableDefineFunction(symtable, "toStr", 1, toStrParams, DTYPE_STRING,
                         false);

  DType toIntParams[] = {DTYPE_DOUBLE};
  symTableDefineFunction(symtable, "toInt", 1, toIntParams, DTYPE_INT, false);

  DType strLengthParams[] = {DTYPE_STRING};
  symTableDefineFunction(symtable, "strLength", 1, strLengthParams, DTYPE_INT,
                         false);

  DType subStrParams[] = {DTYPE_STRING, DTYPE_INT, DTYPE_INT};
  symTableDefineFunction(symtable, "subStr", 3, subStrParams, DTYPE_STRING,
                         false);

  DType ordParams[] = {DTYPE_STRING, DTYPE_INT};
  symTableDefineFunction(symtable, "ord", 2, ordParams, DTYPE_INT, false);

  DType chrParams[] = {DTYPE_INT};
  symTableDefineFunction(symtable, "chr", 1, chrParams, DTYPE_STRING, false);

  return symtable;
}

void symTablePushScope(SymTable *symtable) {
  if (symtable == NULL)
    return;
  size_t newSize = symtable->size * 2;
  if (symtable->count == symtable->size) {
    HashTable **newScopes =
        realloc(symtable->scopes, newSize * sizeof(HashTable *));
    if (newScopes == NULL)
      exit(INT_ERR);
    symtable->scopes = newScopes;
    symtable->size = newSize;
  }
  HashTable *scope = hashTableCreate();
  if (scope == NULL)
    exit(INT_ERR);
  symtable->scopes[symtable->count] = scope;
  symtable->count++;
}

void symTablePopScope(SymTable *symtable) {
  if (symtable == NULL)
    return;
  if (symtable->count == 1)
    exit(INT_ERR);
  symtable->count--;
  freeScope(symtable->scopes[symtable->count]);
  symtable->scopes[symtable->count] = NULL;
}

// Symbol *symTableInsertVariable(SymTable *symtable, const char *name, DType
// type,
//                                bool isMutable);
// Symbol *symTableDefineFunction(SymTable *symtable, const char *name,
//                                size_t paramCount, const DType *paramTypes,
//                                DType returnType, bool isMain);

Symbol *symTableSearch(const SymTable *symtable, const char *name) {
  if (symtable == NULL)
    return NULL;
  for (size_t i = symtable->count; i > 0; i--) {
    Symbol *symbol = hashTableSearch(symtable->scopes[i - 1], name);
    if (symbol != NULL) {
      return symbol;
    }
  }
  return NULL;
}

void symTableFree(SymTable *symtable) {
  if (symtable == NULL)
    return;
  for (size_t i = 0; i < symtable->count; i++) {
    freeScope(symtable->scopes[i]);
  }
  free(symtable->scopes);
  free(symtable);
}