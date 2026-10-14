/**
* Project: IFJ26 Compiler
 *
 * @file symtable.c
 * @brief Implementation of symbol table
 * @author Lukas Zelenak (xzelenl00)
 */

#include "symtable.h"
#include "error.h"
#include "hashtable.h"

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

//SymTable *symTableCreate(void);
// void symTablePushScope(SymTable *symtable);
// void symTablePopScope(SymTable *symtable);
// Symbol *symTableInsertVariable(SymTable *symtable, const char *name, DType
// type,
//                                bool isMutable);
// Symbol *symTableDefineFunction(SymTable *symtable, const char *name,
//                                size_t paramCount, const DType *paramTypes,
//                                DType returnType, bool isMain);
// Symbol *symTableSearch(const SymTable *symtable, const char *name);
// void symTableDebugPrint(SymTable *symtable);
void symTableFree(SymTable *symtable) {
  if (symtable == NULL)
    return;
  for (size_t i = 0; i < symtable->count; i++) {
    freeScope(symtable->scopes[i]);
  }
  free(symtable->scopes);
  free(symtable);
}