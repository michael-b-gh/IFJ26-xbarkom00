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

/**
 * @brief Saved local scopes of a functions while parser analyzes other function
 */
struct SymTableContext {
  HashTable **scopes; /**< Saved local scopes (without global) */
  size_t count;       /**< Number of saved scopes */
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
  symTableDefineFunction(symtable, "print", 1, printParams, DTYPE_UNIT, false,
                         true, 0);

  symTableDefineFunction(symtable, "readLine", 0, NULL, DTYPE_STRING, false,
                         true, 0);

  symTableDefineFunction(symtable, "readInt", 0, NULL, DTYPE_INT, false, true,
                         0);

  symTableDefineFunction(symtable, "readDouble", 0, NULL, DTYPE_DOUBLE, false,
                         true, 0);

  DType toStrParams[] = {DTYPE_ANY};
  symTableDefineFunction(symtable, "toStr", 1, toStrParams, DTYPE_STRING, false,
                         true, 0);

  DType toIntParams[] = {DTYPE_DOUBLE};
  symTableDefineFunction(symtable, "toInt", 1, toIntParams, DTYPE_INT, false,
                         true, 0);

  DType strLengthParams[] = {DTYPE_STRING};
  symTableDefineFunction(symtable, "strLength", 1, strLengthParams, DTYPE_INT,
                         false, true, 0);

  DType subStrParams[] = {DTYPE_STRING, DTYPE_INT, DTYPE_INT};
  symTableDefineFunction(symtable, "subStr", 3, subStrParams, DTYPE_STRING,
                         false, true, 0);

  DType ordParams[] = {DTYPE_STRING, DTYPE_INT};
  symTableDefineFunction(symtable, "ord", 2, ordParams, DTYPE_INT, false, true,
                         0);

  DType chrParams[] = {DTYPE_INT};
  symTableDefineFunction(symtable, "chr", 1, chrParams, DTYPE_STRING, false,
                         true, 0);

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

Symbol *symTableInsertVariable(SymTable *symtable, const char *name, DType type,
                               bool isMutable) {
  if (symtable == NULL)
    exit(INT_ERR);
  Symbol *newSymbol = calloc(1, sizeof(Symbol));
  if (newSymbol == NULL)
    exit(INT_ERR);
  newSymbol->kind = SYM_VARIABLE;
  newSymbol->type = type;
  newSymbol->isMutable = isMutable;
  HashTableInsertResult result =
      hashTableInsert(symtable->scopes[symtable->count - 1], name, newSymbol);
  if (result == HASHTABLE_ITEM_INSERTED) {
    return newSymbol;
  } else if (result == HASHTABLE_ITEM_EXISTS) {
    free(newSymbol);
    return NULL;
  } else {
    free(newSymbol);
    exit(INT_ERR);
  }
}

Symbol *symTableDefineFunction(SymTable *symtable, const char *name,
                               size_t paramCount, const DType *paramTypes,
                               DType returnType, bool isMain,
                               bool hasExplicitReturn, size_t bodyStartIdx) {
  if (symtable == NULL)
    exit(INT_ERR);
  Symbol *newSymbol = calloc(1, sizeof(Symbol));
  if (newSymbol == NULL)
    exit(INT_ERR);
  newSymbol->kind = SYM_FUNCTION;
  newSymbol->paramCount = paramCount;
  newSymbol->returnType = returnType;
  newSymbol->isMain = isMain;
  newSymbol->hasExplicitReturn = hasExplicitReturn;
  newSymbol->bodyStartIdx = bodyStartIdx;
  if (hasExplicitReturn) {
    newSymbol->inferState = INFER_RESOLVED;
  } else {
    newSymbol->inferState = INFER_UNVISITED;
  }
  newSymbol->isDefined = true;
  if (paramCount > 0) {
    newSymbol->paramTypes = calloc(paramCount, sizeof(DType));
    if (newSymbol->paramTypes == NULL) {
      free(newSymbol);
      exit(INT_ERR);
    }
    for (size_t i = 0; i < paramCount; i++) {
      newSymbol->paramTypes[i] = paramTypes[i];
    }
  }

  HashTableInsertResult result =
      hashTableInsert(symtable->scopes[0], name, newSymbol);
  if (result == HASHTABLE_ITEM_INSERTED) {
    return newSymbol;
  } else if (result == HASHTABLE_ITEM_EXISTS) {
    free(newSymbol->paramTypes);
    free(newSymbol);
    return NULL;
  } else {
    free(newSymbol->paramTypes);
    free(newSymbol);
    exit(INT_ERR);
  }
}

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

size_t symTableMainCount(const SymTable *symtable) {
  if (symtable == NULL)
    return 0;
  size_t count = 0;
  size_t htSize = hashTableSize(symtable->scopes[0]);
  for (size_t i = 0; i < htSize; i++) {
    Symbol *symbol = hashTableIndexValue(symtable->scopes[0], i);
    if (symbol != NULL && symbol->kind == SYM_FUNCTION &&
        symbol->isMain == true) {
      count++;
    }
  }
  return count;
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

SymTableContext *symTableSaveContext(SymTable *symtable) {
  if (symtable == NULL)
    exit(INT_ERR);
  SymTableContext *context = malloc(sizeof(SymTableContext));
  if (context == NULL)
    exit(INT_ERR);
  context->count = symtable->count - 1;
  if (context->count > 0) {
    context->scopes = malloc(context->count * sizeof(HashTable *));
    if (context->scopes == NULL) {
      free(context);
      exit(INT_ERR);
    }
    for (size_t i = 0; i < context->count; i++) {
      context->scopes[i] = symtable->scopes[i + 1];
    }

  } else {
    context->scopes = NULL;
  }
  symtable->count = 1;
  return context;
}

void symTableRestoreContext(SymTable *symtable, SymTableContext *context) {
  if (symtable == NULL || context == NULL)
    exit(INT_ERR);
  if (symtable->count != 1)
    exit(INT_ERR);
  for (size_t i = 0; i < context->count; i++) {
    symtable->scopes[i + 1] = context->scopes[i];
  }
  symtable->count = 1 + context->count;
  free(context->scopes);
  free(context);
}
