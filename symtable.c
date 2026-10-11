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

#define SYMTABLE_INITIAL_SIZE 8

struct SymTable {
  HashTable **scopes;
  size_t count;
  size_t size;
};


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
//void symTableFree(SymTable *symtable);